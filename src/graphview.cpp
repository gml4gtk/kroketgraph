/*
 * graphview.cpp
 *
 * Implementation of the GraphView object's methods.
 *
 * GraphView widget handles the display of a graph.
 *
 * It uses the parser/ and graph/ packages to build a
 * interactive representation of the requested graph.
 *
 * This file is distributed as part of Kroket.
 * Copyright (c) 2010 Nicolas BENOIT
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "application.h"
#include "parser/entitylist.h"
#include "parser/defs.h"
#include "graphview.h"
#include "nodeshapes/node-ellipse.h"
#include "nodeshapes/node-rectangle.h"
#include "nodeshapes/node-circle.h"
#include "edge-simple.h"
#include "group-simple.h"


/*
 * Constructor
 */
GraphView::GraphView ( QWidget *parent )
  : QGraphicsView ( parent )
{
  this->setScene ( new QGraphicsScene() );

  this->setRenderHint ( QPainter::Antialiasing );
  this->setViewportUpdateMode ( QGraphicsView::FullViewportUpdate ); /* QGraphicsView::MinimalViewportUpdate */
  this->setOptimizationFlag ( QGraphicsView::DontSavePainterState );
  this->setTransformationAnchor ( QGraphicsView::AnchorUnderMouse );
  this->setBackgroundBrush ( QColor(255, 255, 255) );

  this->timeline.setCurveShape ( QTimeLine::EaseInOutCurve );
  connect ( &this->timeline, SIGNAL(valueChanged(qreal)), this, SLOT(changeViewPos(qreal)) );

  this->graph = NULL;

  this->undoStack = new QUndoStack ( this );

  this->gridCellHCenter = new qreal [ 12 ];
  this->gridCellVCenter = new qreal [ 12 ];
  this->gridCellHSize = new qreal [ 12 ];
  this->gridCellVSize = new qreal [ 12 ];
  this->gridSize = QPair<quint32, quint32> ( 12, 12 );
}


/*
 * Load a graph from a file.
 */
void
GraphView::loadGraphFromFile ( const char *file_name )
{
  EntityList *el;
  Entity *e;
  EntityList::iterator iter;
  const char *attr_v;
  AbstractNodeShape *node;

  el = new EntityList ( );
  el->parseFromFile ( file_name );

  /* close current graph if any */
  this->closeGraph ( );

  /* fresh start */
  this->graph = new Graph ( );
  this->fname = QString::fromLatin1 ( file_name ); // QString::fromAscii ( file_name );
  this->finfo = QFileInfo ( file_name );
  this->title = QString::fromLatin1 ( "untitled" ); // QString::fromAscii ( "untitled" );

  /* info loading */
  iter = el->iterate ( ENTITY_INFO );

  while ( iter.hasNext() )
    {
      e = iter.next ( );
      attr_v = e->getValueOfAttribute ( ATTR_INFO_TITLE );

      if ( attr_v != NULL )
        this->title = QString::fromLatin1 ( attr_v ); // QString::fromAscii
    }

  /* node loading */
  iter = el->iterate ( ENTITY_NODE );

  while ( iter.hasNext() )
    {
      e = iter.next ( );
      attr_v = e->getValueOfAttribute ( ATTR_NODE_SHAPE );

      if ( attr_v == NULL )
        node = new NodeEllipse ( this );
      else if ( strcmp(attr_v,NODE_SHAPE_ELLIPSE) == 0 )
        node = new NodeEllipse ( this );
      else if ( strcmp(attr_v,NODE_SHAPE_RECTANGLE)  == 0 )
        node = new NodeRectangle ( this );
      else if ( strcmp(attr_v,NODE_SHAPE_CIRCLE)  == 0 )
        node = new NodeCircle ( this );
      else
        {
          node = new NodeEllipse ( this );
          std::cerr << "warning: node shape \'" << attr_v << "\' is invalid.\n";
        }

      node->initFromEntity ( e );
      this->graph->addNode ( node );
      this->scene()->addItem ( (AbstractNodeShape *) node );
    }

  /* edge loading */
  EdgeSimple *edge;
  AbstractNodeShape *src, *dest;
  QString qstr;

  iter = el->iterate ( ENTITY_EDGE );

  while ( iter.hasNext() )
    {
      e = iter.next ( );

      qstr = Node::extractNodeName( e->getValueOfAttribute(ATTR_EDGE_SRC_PORT) );
      src = (AbstractNodeShape *) this->graph->getNode ( qstr );

      if ( src == NULL )
        continue;

      qstr = Node::extractNodeName( e->getValueOfAttribute(ATTR_EDGE_DEST_PORT) );
      dest = (AbstractNodeShape *) this->graph->getNode ( qstr );

      if ( dest == NULL )
        continue;

      edge = new EdgeSimple ( src, dest );
      edge->initFromEntity ( e );

      if ( src != dest )
        {
          this->scene()->addItem ( edge );
          this->graph->addEdge ( edge );
          src->addChild ( dest );
        }
      else
        src->addLoopingEdge ( edge );
    }

  /* group loading */
  AbstractGroupShape *group;
  Attribute *a;
  QStringList *contents;
  QHash<AbstractGroupShape *, QString> toBeLoaded;

  iter = el->iterate ( ENTITY_GROUP );

  while ( iter.hasNext() )
    {
      e = iter.next ( );

      a = e->getAttribute ( ATTR_GROUP_CONTENTS );
      contents = a->getValueAsListOfString ( );

      if ( contents->size() == 0 )
        {
          delete contents;
          continue;
        }

      group = new GroupSimple ( this );
      group->initFromEntity ( e );

      foreach ( qstr, *contents )
        {
          node = (AbstractGroupShape *) this->graph->getNode ( qstr );

          if ( node != NULL )
            {
              group->addShape ( node );
              node->setGroup ( group );
            }
          else
            toBeLoaded.insertMulti ( group, qstr ); /* will be loaded after all groups are created */
        }

      this->graph->addNode ( group );
      this->scene()->addItem ( group );
      this->groups.insert ( group->id, group );

      delete contents;
    }

  /* insert subgroups */
  QHashIterator<AbstractGroupShape *, QString> iterSubGroups ( toBeLoaded );
  AbstractGroupShape *subgroup;

  while ( iterSubGroups.hasNext() )
    {
      iterSubGroups.next ( );
      group = iterSubGroups.key ( );
      subgroup = this->groups.value ( iterSubGroups.value() );

      if ( subgroup != NULL )
        {
          group->addShape ( subgroup );
          subgroup->setGroup ( group );
        }
    }

  /* freeze groups content and collapse them */
  QList<AbstractEdgeShape *> *l;

  foreach ( group, this->groups )
    {
      l = group->freezeContent ( );

      foreach ( AbstractEdgeShape *aes, (*l) )
        {
          this->graph->addEdge ( aes );
          this->scene()->addItem ( aes );
        }

      delete l;
    }

  /* assign subgraph IDs */
  this->graph->assignSubgraphIDs ( );

  /* destroy parser */
  delete el;

  /* collapse all the groups without syncing the view */
  this->collapseAll ( false );
}


/*
 * Compute the minimal spacing between cells of the grid.
 */
void
GraphView::computeGridHVSpacing ( const QPair<quint32, quint32> &gridMax )
{
  quint32 i;
  QSizeF size;
  AbstractNodeShape *node;
  qreal pos;

  /* default values */
  for ( i=0; i<gridMax.first+1; ++i )
    this->gridCellHSize[i] = 22.0; /* dummy node width */

  for ( i=0; i<gridMax.second+1; ++i )
    this->gridCellVSize[i] = 20.0; /* dummy node height */

  /* inspect nodes */
  QHashIterator<QString, Node *> iter = this->graph->iterateOverNodes ( );

  while ( iter.hasNext() )
    {
      iter.next ( );
      node = (AbstractNodeShape *) iter.value ( );

      if ( !((Node *)node)->isActive() )
        continue;

      size = node->size ( );

      if ( gridCellHSize[node->grid_x] < size.width() )
        this->gridCellHSize[node->grid_x] = size.width();

      if ( gridCellVSize[node->grid_y] < size.height() )
        this->gridCellVSize[node->grid_y] = size.height();
    }

  /* handle horizontal spacing */
  for ( i=0; i<gridMax.first+1; ++i )
    this->gridCellHSize[i] /= 2.0;

  this->gridCellHCenter[0] = this->HSpacing() + this->gridCellHSize[0];
  pos = this->gridCellHCenter[0];

  for ( i=1; i<gridMax.first+1; ++i )
    {
      pos += this->gridCellHSize[i-1] + this->HSpacing() + this->gridCellHSize[i];
      this->gridCellHCenter[i] = pos;
    }

  /* handle vertical spacing */
  for ( i=0; i<gridMax.second+1; ++i )
    this->gridCellVSize[i] /= 2.0;

  this->gridCellVCenter[0] = this->VSpacing() + this->gridCellVSize[0];
  pos = this->gridCellVCenter[0];

  for ( i=1; i<gridMax.second+1; ++i )
    {
      pos += this->gridCellVSize[i-1] + this->VSpacing() + this->gridCellVSize[i];
      this->gridCellVCenter[i] = pos;
    }
}

/*
 * Synchronize the graph and its display.
 */
void
GraphView::synchronizeView ( )
{
  AbstractNodeShape *node;
  QPair<quint32, quint32> gridMax;

  /* graph layering */
  gridMax = this->graph->assignGridCoordinates ( );

  /* compute grid cells size */
  if ( (gridMax.first+1) > gridSize.first )
    {
      this->gridCellHCenter = (qreal *) realloc ( this->gridCellHCenter, sizeof(qreal)*(gridMax.first+1) );
      this->gridCellHSize = (qreal *) realloc ( this->gridCellHSize, sizeof(qreal)*(gridMax.first+1) );
      this->gridSize.first = gridMax.first+1;
    }

  if ( (gridMax.second+1) > gridSize.second )
    {
      this->gridCellVCenter = (qreal *) realloc ( this->gridCellVCenter, sizeof(qreal)*(gridMax.second+1) );
      this->gridCellVSize = (qreal *) realloc ( this->gridCellVSize, sizeof(qreal)*(gridMax.second+1) );
      this->gridSize.second = gridMax.second+1;
    }

  this->computeGridHVSpacing ( gridMax );

  /* nodes display */
  QHashIterator<QString, Node *> iter = this->graph->iterateOverNodes ( );

  while ( iter.hasNext() )
    {
      iter.next ( );
      node = (AbstractNodeShape *) iter.value ( );

      if ( !((Node *)node)->isActive() )
        continue;

      if ( IS_GROUP(node) )
        node->pack ( );

      node->animateCenterOn ( this->gridCellHCenter[node->grid_x], this->gridCellVCenter[node->grid_y] );
    }

  /* viewport resizing */
  const qreal w = this->gridCellHCenter[gridMax.first]+this->gridCellHSize[gridMax.first]+this->HSpacing();
  const qreal h = this->gridCellVCenter[gridMax.second]+this->gridCellVSize[gridMax.second]+this->VSpacing();
  this->scene()->setSceneRect ( this->HSpacing()/1.7, 0.0, w, h );
}


/*
 * Output the view to an image.
 */
QImage *
GraphView::getImage ( )
{
  QImage *img = new QImage ( this->width(), this->height(), QImage::Format_ARGB32_Premultiplied );
  img->fill ( 0 );

  QPainter *p  = new QPainter ( img );
  p->setRenderHint ( QPainter::Antialiasing );
  p->setRenderHint ( QPainter::TextAntialiasing );
  p->setRenderHint ( QPainter::SmoothPixmapTransform );
  this->render ( p );
  delete p;

  return img;
}


/*
 * Print the view.
 */
void
GraphView::print ( QPrinter *printer )
{
  QPainter *p  = new QPainter ( printer );
  p->setRenderHint ( QPainter::Antialiasing );
  p->setRenderHint ( QPainter::TextAntialiasing );
  p->setRenderHint ( QPainter::SmoothPixmapTransform );
  this->render ( p );
  p->drawText ( 0, 0, this->title );
  delete p;
}


/*
 * Expand the specified group
 */
void
GraphView::expand ( QString &gname,
                    bool autoSync )
{
  AbstractNodeShape *n = (AbstractNodeShape *) this->graph->getNode ( gname );

  if ( n != NULL )
    if ( IS_GROUP(n) )
      ((AbstractGroupShape *) n)->expand ( );

  if ( autoSync )
    this->synchronizeView ( );
}


/*
 * Collapse the specified group
 */
void
GraphView::collapse ( QString &gname,
                      bool autoSync )
{
  AbstractNodeShape *n = (AbstractNodeShape *) this->graph->getNode ( gname );

  if ( n != NULL )
    if ( IS_GROUP(n) )
      ((AbstractGroupShape *) n)->collapse ( );

  if ( autoSync )
    this->synchronizeView ( );
}


/*
 * Focus (center & zoom) the view on the specified node.
 */
bool
GraphView::focusOn ( QString &nname )
{
  if ( this->graph == NULL )
    return false;

  AbstractNodeShape *n = (AbstractNodeShape *) this->graph->getNode ( nname );

  if ( n != NULL )
    {
      this->viewPos = QLineF ( this->center(), n->finalCenter() );

      if ( this->timeline.state() == QTimeLine::Running )
        this->timeline.stop ( );

      this->timeline.start ( );
    }
  else
    return false;

  return true;
}


/*
 * Trace from the specified node.
 * Every nodes belonging to the same subgraph will be hidden.
 */
bool
GraphView::traceFrom ( QString &nname,
                       bool autoSync )
{
  if ( this->graph == NULL )
    return false;

  AbstractNodeShape *n = (AbstractNodeShape *) this->graph->getNode ( nname );

  if ( n == NULL )
    return false;

  this->traceFrom ( n, autoSync );
  return true;
}

void
GraphView::traceFrom ( AbstractNodeShape *n,
                       bool autoSync )
{
  Node *node;
  QHashIterator<QString,Node *>  iter = this->graph->iterateOverNodes ( );

  if ( n->hasGroup() )
    n->group->expand ( );

  while ( iter.hasNext() )
    {
      iter.next ( );
      node = iter.value ( );

      if ( n->subgraph_id == node->subgraph_id )
        {
          node->setActive ( false );
          ((AbstractNodeShape *)node)->hide ( );
        }
    }

  if ( IS_NODE(n) )
    ((Node *)n)->setActive ( true );
  else if ( IS_GROUP(n) )
    ((AbstractGroupShape *)n)->collapse ( );

  n->show ( );

  if ( autoSync )
    this->synchronizeView ( );
}


/*
 * Filter the view with the provided tag (nodes with)
 */
void
GraphView::filterWithTag ( QString &tag,
                           bool autoSync )
{
  AbstractNodeShape *n;
  QHashIterator<QString,Node *> iter = this->graph->iterateOverNodes ( );

  while ( iter.hasNext() )
    {
      iter.next ( );
      n = (AbstractNodeShape *) iter.value ( );

      if ( IS_GROUP(n) )
        continue;

      if ( n->tags.contains(tag) )
        {
          ((Node *)n)->setActive ( true );
          n->show ( );

          if ( n->hasGroup() )
            n->group->expand ( false );
        }
      else
        {
          ((Node *)n)->setActive ( false );
          n->hide ( );
        }
    }

  foreach ( AbstractGroupShape *group, this->groups )
    {
      if ( group->isCollapsed() )
        {
          ((Node *)group)->setActive ( false );
          group->hide ( );
        }
      else
        group->pack ( ); /* if the group has no more active nodes, it will hide itself */
    }

  if ( autoSync )
    this->synchronizeView ( );
}


/*
 * Filter the view with the provided tag (nodes without)
 */
void
GraphView::filterWithoutTag ( QString &tag,
                              bool autoSync )
{
  AbstractNodeShape *n;
  QHashIterator<QString,Node *> iter = this->graph->iterateOverNodes ( );

  while ( iter.hasNext() )
    {
      iter.next ( );
      n = (AbstractNodeShape *) iter.value ( );

      if ( IS_GROUP(n) )
        continue;

      if ( !n->tags.contains(tag) )
        {
          ((Node *)n)->setActive ( true );
          n->show ( );

          if ( n->hasGroup() )
            n->group->expand ( false );
        }
      else
        {
          ((Node *)n)->setActive ( false );
          n->hide ( );
        }
    }

  foreach ( AbstractGroupShape *group, this->groups )
    {
      if ( group->isCollapsed() )
        {
          ((Node *)group)->setActive ( false );
          group->hide ( );
        }
    }

  if ( autoSync )
    this->synchronizeView ( );
}


/*
 * Add an undo command to the commands stack.
 */
void
GraphView::addUndoCommand ( QUndoCommand *uc )
{
  this->undoStack->push ( uc );

  if ( this->hasApp() )
    this->app->updateUndoActions ( );
}


/*
 * Callback for the undo signal.
 */
void
GraphView::undo ( )
{
  this->undoStack->undo ( );
  this->synchronizeView ( );

  if ( this->hasApp() )
    this->app->updateUndoActions ( );
}


/*
 * Callback for the redo signal.
 */
void
GraphView::redo ( )
{
  this->undoStack->redo ( );
  this->synchronizeView ( );

  if ( this->hasApp() )
    this->app->updateUndoActions ( );
}


/*
 * Callback for the time line, when changing the view position.
 */
void
GraphView::changeViewPos ( qreal val )
{
  this->centerOn ( this->viewPos.pointAt(val) );
}


/*
 * Callback for the ZoomIn signal.
 */
void
GraphView::zoomIn ( )
{
  this->scale ( 1.2, 1.2 );
  this->adaptEdgesWidth ( );
}


/*
 * Callback for the ZoomOut signal.
 */
void
GraphView::zoomOut ( )
{
  this->scale ( 0.8, 0.8 );
  this->adaptEdgesWidth ( );
}


/*
 * Callback for the ZoomFit signal.
 */
void
GraphView::zoomFit ( )
{
  this->fitInView ( this->scene()->sceneRect(), Qt::KeepAspectRatio );
  this->adaptEdgesWidth ( );
}


/*
 * Adapt the width of edges according to the zoom factor.
 */
void
GraphView::adaptEdgesWidth ( )
{
  QPoint p = this->mapFromScene(64,0) - this->mapFromScene(0,0);
  QListIterator<Edge *> iter = this->graph->iterateOverEdges ( );

  if ( p.x() <= 60 )
    {
      if ( p.x() <= 0 )
        p.setX ( 1 );

      const qreal pw = 100.0 / (qreal)p.x();

      while ( iter.hasNext() )
        ((AbstractEdgeShape *)iter.next())->adaptPenWidth ( pw );
    }
  else
    {
      while ( iter.hasNext() )
        ((AbstractEdgeShape *)iter.next())->resetPenWidth ( );
    }
}


/*
 * Callback for the ExpandAll signal.
 */
void
GraphView::expandAll ( bool autoSync )
{
  foreach ( AbstractGroupShape *group, this->groups )
    group->expand ( );

  if ( autoSync )
    this->synchronizeView ( );
}


/*
 * Callback for the CollapseAll signal.
 */
void
GraphView::collapseAll ( bool autoSync )
{
  foreach ( AbstractGroupShape *group, this->groups )
    group->collapse ( );

  if ( autoSync )
    this->synchronizeView ( );
}


/*
 * Callback for the Reset signal.
 * This reloads the input file.
 */
void
GraphView::reset ( )
{
  if ( this->graph != NULL )
    {
      this->loadGraphFromFile ( this->fname.toLatin1() ); // this->fname.toAscii()
      this->synchronizeView ( );
    }
}


/*
 * Paint callback.
 */
void
GraphView::paintEvent ( QPaintEvent *event )
{
  QGraphicsView::paintEvent ( event );
}


/*
 * Mousewheel callback.
 */
void
GraphView::wheelEvent ( QWheelEvent *event )
{
  if ( event->modifiers() == Qt::ControlModifier )
    {
      if ( event->delta() < 0 ) /* downward => zoom in */
        this->zoomIn ( );
      else                      /* upward => zoom out */
        this->zoomOut ( );
    }
  else
    QGraphicsView::wheelEvent ( event );
}


/*
 * Mouse move callback.
 */
void
GraphView::mouseMoveEvent ( QMouseEvent *event )
{
  if ( ( event->modifiers() & Qt::ControlModifier ) &&
       ( event->buttons() & Qt::LeftButton ) )
    {
      QPointF v, c;

      v = this->mapToScene ( event->x(), event->y() );
      c = this->center ( );

      /* calculate the movement vector and store the new center's coordinate */
      v.setX ( c.x() + (mousePos.x() - v.x()) );
      v.setY ( c.y() + (mousePos.y() - v.y()) );

      /* center on the new coordinates */
      this->centerOn ( v.x(), v.y() );

      /* store the updated (because relative to the scene) mouse position */
      mousePos = this->mapToScene ( event->x(), event->y() );

      /* because the GraphicsScene::centerOn() function is not precise, we save the precision loss */
      c = this->center ( );
      mousePos.setX ( mousePos.x() - (c.x() - v.x()) );
      mousePos.setY ( mousePos.y() - (c.y() - v.y()) );
    }
  else
    QGraphicsView::mouseMoveEvent ( event );
}


/*
 * Mouse button press event callback.
 */
void
GraphView::mousePressEvent ( QMouseEvent *event )
{
  if ( ( event->modifiers() & Qt::ControlModifier ) &&
       ( event->buttons() & Qt::LeftButton ) )
    {
      mousePos = this->mapToScene ( event->x(), event->y() );
      this->setCursor ( Qt::OpenHandCursor );
    }
  else
    QGraphicsView::mousePressEvent ( event );
}


/*
 * Mouse button release event callback.
 */
void
GraphView::mouseReleaseEvent ( QMouseEvent *event )
{
  this->setCursor ( Qt::ArrowCursor );
  QGraphicsView::mouseReleaseEvent ( event );
}


/*
 * Size Hint.
 */
QSize
GraphView::sizeHint ( ) const
{
  return QGraphicsView::sizeHint();
}


/*
 * Calculate the coordinates of the center of the visible part of the scene.
 */
QPointF
GraphView::center ( ) const
{
  return this->mapToScene ( this->rect().center() );
}


/*
 * Close the current graph.
 */
void
GraphView::closeGraph ( )
{
  if ( this->graph != NULL )
    delete this->graph;

  this->groups.clear ( );

  this->undoStack->clear ( );

  delete this->scene();

  this->setScene ( new QGraphicsScene() );

  this->resetMatrix ( );
  this->resetTransform ( );
}


/*
 * Destructor
 */
GraphView::~GraphView ( )
{
  this->closeGraph ( );
  delete this->undoStack;
  delete this->scene();

  delete gridCellHCenter;
  delete gridCellVCenter;
  delete gridCellHSize;
  delete gridCellVSize;
}
