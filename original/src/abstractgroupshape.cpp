/*
 * abstractgroupshape.cpp
 *
 * Implementation of the AbstractGroupShape object's methods.
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

#include "abstractedgeshape.h"
#include "graphview.h"
#include "parser/defs.h"
#include "abstractanchor.h"
#include "abstractgroupshape.h"
#include "undocommands/cmd-expand.h"
#include "undocommands/cmd-collapse.h"
#include "convexhull.h"

#define DEBUG_ABSTRACT_GROUP_SHAPE_SUBGRP 0

/*
 * Constructor
 */
AbstractGroupShape::AbstractGroupShape ( GraphView *gv )
  : AbstractNodeShape ( gv )
{
  this->setZValue ( 1 );

  this->group = NULL;
  this->mode = GRP_COLLAPSED;
  this->n_shape = GRP_NODE_SHAPE_RECTANGLE;

  this->setData ( DATA_TYPE, TYPE_GROUP );
}


/*
 * Initialize a group shape using an Entity object.
 */
void
AbstractGroupShape::initFromEntity ( const Entity *e )
{
  const char *attr_v;

  AbstractNodeShape::initFromEntity ( e );

  if ( this->hasText() )
    {
      QRectF r ( this->text->boundingRect() );
      this->width = r.width() * 1.5;
      this->height = r.height() * 1.8;

      if ( this->width < 24.0 )
        this->width = 24.0;

      qreal tx = ( this->width - (int)r.width() ) / 2;
      qreal ty = ( this->height - (int)r.height() ) / 2;
      this->text->setPos ( tx, ty );
      this->txtPos = QPointF ( tx, ty );
    }
  else
    {
      this->width = 24.0;
      this->height = 16.0;
    }

  attr_v = e->getValueOfAttribute ( ATTR_GROUP_SHAPE );

  if ( attr_v == NULL )
    this->n_shape = GRP_NODE_SHAPE_RECTANGLE;
  else if ( strcmp(attr_v,GRP_NODE_SHAPE_ELLIPSE_TXT) == 0 )
    this->n_shape = GRP_NODE_SHAPE_ELLIPSE;
  else
    this->n_shape = GRP_NODE_SHAPE_RECTANGLE;
}


/*
 * Add a shape to an AbstractGroupShape.
 */
void
AbstractGroupShape::addShape ( AbstractNodeShape *ans )
{
  if ( IS_GROUP(ans) )
    {
      this->addGroupShape ( (AbstractGroupShape *) ans );
      return;
    }

#if DEBUG_ABSTRACT_GROUP_SHAPE_SUBGRP >= 1
  std::cout << "INSERTING node \'" << qPrintable(ans->id) << "\' in group \'" << qPrintable(this->id) << "\'\n";
#endif

  this->content.append ( ans );
  this->insertedSubNode ( ans );

#if DEBUG_ABSTRACT_GROUP_SHAPE_SUBGRP >= 1
  std::cout << "\n";
#endif
}


/*
 * Callback for the insertion of a new node.
 */
void
AbstractGroupShape::insertedSubNode ( AbstractNodeShape *ans )
{
  /* the edges that go out of the group are registered */
  foreach ( AbstractEdgeShape *e, ans->edges )
    {
      if ( e->src == ans )
        {
          /* the added shape is a parent, do this contains its child ? */
          if ( !this->containsShape((AbstractNodeShape*)e->dest) )
            {
              this->externalEdges.append ( e ); /* this does not contain the child, this is an outgoing edge */
              this->addChild ( e->dest );

              if ( this->internalEdges.contains(e) )
                qlist_remove ( this->internalEdges, e );
            }
          else if ( this->externalEdges.contains(e) )
            {
              qlist_remove ( this->externalEdges, e ); /* this contains the child, this is an internal edge */
              this->internalEdges.append ( e );
              this->removeChild ( e->dest );           /* the child is inside the group, we do not register it */
              this->removeParent ( e->src );           /* the parent is inside the group, we do not register it */
            }
        }
      else
        {
          /* the added shape is a child, do this contains its parent ? */
          if ( !this->containsShape((AbstractNodeShape*)e->src) )
            {
              this->externalEdges.append ( e );   /* this does not contain the parent, this is an outgoing edge */
              this->addParent ( e->src );

              if ( this->internalEdges.contains(e) )
                qlist_remove ( this->internalEdges, e );
            }
          else if ( this->externalEdges.contains(e) )
            {
              qlist_remove ( this->externalEdges, e ); /* this contains the parent, this is an internal edge */
              this->internalEdges.append ( e );
              this->removeParent ( e->src );           /* the parent is inside the group, we do not register it */
              this->removeChild ( e->dest );           /* the child is inside the group, we do not register it */
            }
        }
    }

  /* this tells its parent group that it holds a new node */
  if ( this->hasGroup() )
    this->group->insertedSubNode ( ans );

  
#if DEBUG_ABSTRACT_GROUP_SHAPE_SUBGRP >= 1
  std::cout << "group " << qPrintable(this->id) << " has external edges : ";
  foreach ( AbstractEdgeShape *e, this->externalEdges )
    std::cout << qPrintable(e->id) << "(" << qPrintable(e->k_id) << ") ";
  std::cout << "\n";
#endif
}


/*
 * Add a group to an AbstractGroupShape.
 */
void
AbstractGroupShape::addGroupShape ( AbstractGroupShape *ags )
{
#if DEBUG_ABSTRACT_GROUP_SHAPE_SUBGRP >= 1
  std::cout << "INSERTING group \'" << qPrintable(ags->id) << "\' in group \'" << qPrintable(this->id) << "\'\n";
#endif

  this->content.append ( ags );
  this->subgroups.append ( ags );
  this->insertedSubGroup ( ags, ags->zValue() );

#if DEBUG_ABSTRACT_GROUP_SHAPE_SUBGRP >= 1
  std::cout << "\n";
#endif
}


/*
 * Callback for the insertion of a new subgroup.
 * The zValue is used to make sure the parent group is under its subgroups.
 */
void
AbstractGroupShape::insertedSubGroup ( AbstractGroupShape *ags,
                                       qreal zValue )
{
  /* the edges that go out of the subgroup are registered */
  foreach ( AbstractEdgeShape *e, ags->externalEdges )
    {
      if ( !this->containsShape((AbstractNodeShape*)e->dest) )
        {
          /* this has a child outside */
          this->externalEdges.append ( e );
          this->addChild ( e->dest );

          if ( this->internalEdges.contains(e) )
            qlist_remove ( this->internalEdges, e );
        }
      else if ( !this->containsShape((AbstractNodeShape*)e->src) )
        {
          /* this has parent outside */
          this->externalEdges.append ( e );
          this->addParent ( e->src );

          if ( this->internalEdges.contains(e) )
            qlist_remove ( this->internalEdges, e );
        }

      else if ( this->externalEdges.contains(e) )
        {
          /* the edge is internal, let's clean up everything */
          qlist_remove ( this->externalEdges, e );
          this->removeParent ( e->src );
          this->removeChild ( e->dest );

          /* internal edges are only considered for nodes directly owned by the group */
          if ( ( ((AbstractNodeShape *) e->src)->group == this ) ||
               ( ((AbstractNodeShape *) e->dest)->group == this ) )
            this->internalEdges.append ( e );
        }
    }

  /* make sure the subgroup is above this group */
  if ( zValue <= this->zValue() )
    this->setZValue ( zValue - 0.01 );

  /* this tells its parent group that it holds a new group */
  if ( this->hasGroup() )
    this->group->insertedSubGroup ( ags, this->zValue() );

#if DEBUG_ABSTRACT_GROUP_SHAPE_SUBGRP >= 1
  std::cout << "group " << qPrintable(this->id) << " has external edges : ";
  foreach ( AbstractEdgeShape *e, this->externalEdges )
    std::cout << qPrintable(e->id) << "(" << qPrintable(e->k_id) << ") ";
  std::cout << "\n";
#endif
}


/*
 * Does an AbstractGroupShape contains the provided shape.
 */
bool
AbstractGroupShape::containsShape ( AbstractNodeShape *ans )
{
  if ( this->content.contains(ans) )
    return true;

  foreach ( AbstractGroupShape *ags, this->subgroups )
    if ( ags->containsShape(ans) )
      return true;

  return false;
}


/*
 * Freeze the content of a group (generate group's outgoing edges)
 *
 * When the group switches to node mode, it has to draw outgoing edges.
 * Those can not be shared with the contained nodes.
 * This function creates the group's own edges shapes.
 */
QList<AbstractEdgeShape *> *
AbstractGroupShape::freezeContent ( )
{
  QList<AbstractEdgeShape *> *ret = new QList<AbstractEdgeShape *> ( );
  AbstractEdgeShape *e;

  foreach ( AbstractEdgeShape *aes, this->externalEdges )
    {
      if ( this->containsShape((AbstractNodeShape*)aes->src) )
        {
          /* the edge should go from the group to an external node */
          e = aes->clone ( this, (AbstractNodeShape*)aes->dest );
        }
      else
        {
          /* the edge should go from an external node to the group */
          e = aes->clone ( (AbstractNodeShape*)aes->src, this );

          /* we must also create the edges that join the group of the node *
           * we do this only for the dest, so we avoid duplicates          */
          if ( ((AbstractNodeShape*)aes->src)->hasGroup() ) 
            ((AbstractNodeShape*)aes->src)->group->joinGroup ( aes, this, ret );
        }

      ret->append ( e );
    }

  return ret;
}


/*
 * Update the contour of an AbstractGroupShape.
 */
void
AbstractGroupShape::pack ( )
{
  this->prepareGeometryChange ( );

  this->contour = QPainterPath ( );

  if ( this->content.size() == 0 )
    return;

  if ( this->isExpanded() ) /* group mode (background drawing) */
    {
      if ( this->nbActive() > 0 ) /* if the group contains active elements, build a convex hull */
        {
          QPolygonF poly ( ConvexHull::compute ( this->content ) );

          this->setPos ( poly[0].x(), poly[0].y() );     /* set the first point of the polygon as the origin of the shape */
          poly.translate ( -poly[0].x(), -poly[0].y() );

          this->contour.addPolygon ( poly );
          this->contour.closeSubpath ( );
          this->contour.setFillRule ( Qt::OddEvenFill );

          /* text */
          if ( this->hasText() )
            {
              QPointF c ( poly.boundingRect().center ( ) );
              QRectF r ( this->text->boundingRect() );

              c.setX ( c.x() - (r.width() / 2.0 ) );
              c.setY ( c.y() - (r.height() / 2.0 ) );

              this->text->setPos ( c );
            }

          if ( !this->isVisible() )
            this->show ( );
        }
      else if ( this->isVisible() ) /* if the group contains no active elements, hide it */
        this->hide ( );
    }
  else  /* node mode */
    {
      if ( this->n_shape == GRP_NODE_SHAPE_ELLIPSE )
        this->contour.addEllipse ( 0.0, 0.0, getWidth(), getHeight() );
      else
        this->contour.addRect ( QRectF(0.0,0.0,getWidth(),getHeight()) );

      if ( this->hasText() )
        this->text->setPos ( this->txtPos.x(), this->txtPos.y() );
    }

  /* parent group */
  if ( this->hasGroup() )
    {
      this->group->pack ( );
      this->group->update ( );
    }
}


/*
 * Update the anchors of an AbstractGroupShape.
 */
void
AbstractGroupShape::updateAnchors ( )
{
  if ( this->isCollapsed() )
    AbstractNodeShape::updateAnchors ( );
}


/*
 * Does the node has an active group ? (in other words, is the node is overriden ?)
 * If the group is collapsed, it means that either the group itself or its parent group
 * is active and overrides the node.
 */
bool
AbstractGroupShape::hasActiveGroup ( ) const
{
  if ( this->isExpanded() )
    return true;

  if ( this->group )
    return this->group->isCollapsed ( );

  return false;
}


/*
 * Expand a group.
 * The group is deactivated and its content is shown.
 */
void
AbstractGroupShape::expand ( bool showAll )
{
  /*
  std::cout << "expanding of group \'" << this->id.toAscii().constData() << "\'\n";
  */

  this->mode = GRP_EXPANDED;
  ((Node *)this)->setActive ( false );
  this->setFlag ( ItemIsMovable, false );

  /* activate the nodes contained */
  if ( showAll )
    {
      foreach ( AbstractNodeShape *ans, this->content )
        {
          /* if a subgroup has already been expanded,     *
           * assume this is for a good reason and skip it */
          if ( IS_GROUP(ans) )
            if ( ((AbstractGroupShape *)ans)->isExpanded() )
              continue;

          ((Node *)ans)->setActive ( true );
          ans->show ( );
          ans->setPos ( this->x(), this->y() );
          ans->setFlag ( ItemIsMovable );
        }
    }

  /* hide edges that were attached to this group */
  foreach ( AbstractEdgeShape *aes, this->edges )
    aes->updateVisibility ( );

  /* keep in touch with the group this belongs to */
  if ( this->hasGroup() )
    this->group->subgroupExpanded();
}


/*
 * Collapse a group.
 * The group becomes a node and its content is hidden.
 */
void
AbstractGroupShape::collapse ( )
{
  /* if the parent group is collapsed, then we should not mess with anything */
  if ( this->hasGroup() )
    if ( this->group->isCollapsed() )
      return;

  /* switch to collapsed mode and activate the node */
  this->mode = GRP_COLLAPSED;
  ((Node *)this)->setActive ( true );
  this->setFlag ( ItemIsMovable );

  /* deactivate content */
  this->hideContent ( );

  /* show edges */
  foreach ( AbstractEdgeShape *aes, this->edges )
    aes->updateVisibility ( );
}


/*
 * Switch the mode of the group.
 */
void
AbstractGroupShape::switchGrpMode ( )
{
  if ( this->isCollapsed() )
    {
      this->expand ( );
      this->graphView->addUndoCommand ( new CmdExpand(this) );
    }
  else
    {
      this->collapse ( );
      this->graphView->addUndoCommand ( new CmdCollapse(this) );
    }

  this->graphView->synchronizeView ( );
}


/*
 * Callback for mouse press events.
 */
void
AbstractGroupShape::mousePressEvent ( QGraphicsSceneMouseEvent *event )
{
  QGraphicsItem::mousePressEvent ( event );
  event->accept();
}


/*
 * Callback for mouse release events.
 */
void
AbstractGroupShape::mouseReleaseEvent ( QGraphicsSceneMouseEvent *event )
{
  /* we check if an edge is not the real target of the click */
  AbstractGroupShape *target = this;
  QList<QGraphicsItem *> l ( this->scene()->items(event->scenePos()) );

  foreach ( QGraphicsItem *gi, l )
    {
      if ( IS_EDGE(gi) )
        {
          ((AbstractEdgeShape *) gi)->mouseReleaseEvent ( event );
          target = NULL;
          break;
        }
        else if ( IS_GROUP(gi) )
        {
          target = (AbstractGroupShape *) gi;
          break;
        }
    }

  if ( target == NULL )
    return;

  /* right clicks concern AbstractGroupShape */
  if ( event->button() == Qt::RightButton )
    target->switchGrpMode ( );

  QGraphicsItem::mouseReleaseEvent ( event );
}


/*
 * Count the number of active elements currently in the group.
 */
int
AbstractGroupShape::nbActive ( ) const
{
  int ret = 0;

  foreach ( AbstractNodeShape *ans, this->content )
    {
      if ( ((Node *)ans)->isActive() )
        ++ret;
      else if ( IS_GROUP(ans) )
        ret += ((AbstractGroupShape *) ans)->nbActive ( );
    }

  return ret;
}


/*
 * Create the edge that join two groups from an edge joining a node and a group.
 * This allows us to display two groups in collapsed mode by creating the
 * equivalent edges that exist between their content.
 */
void
AbstractGroupShape::joinGroup ( AbstractEdgeShape *modelEdge,
                                AbstractGroupShape *destGroup,
                                QList<AbstractEdgeShape *> *createdEdges )
{
  AbstractEdgeShape *e;

  if ( this->containsShape(destGroup) )
    return;

  e = modelEdge->clone ( this, destGroup );

  this->addChild ( destGroup );
  createdEdges->append ( e );

  /* propagate to parent group */
  if ( this->hasGroup() )
    this->group->joinGroup ( modelEdge, destGroup, createdEdges );
}


/*
 * Hide recursively the content of a group and its subgroups.
 */
void
AbstractGroupShape::hideContent ( )
{
  foreach ( AbstractNodeShape *ans, this->content )
    {
      ((Node *)ans)->setActive ( false );
      ans->hide ( );
      ans->setPos ( this->x(), this->y() );

      if ( IS_GROUP(ans) )
        {
          ((AbstractGroupShape *)ans)->mode = GRP_COLLAPSED;
          ((AbstractGroupShape *)ans)->hideContent ( );
        }
    }
}


/*
 * Callback used by a subgroup to tell its group that it just expanded.
 */
void
AbstractGroupShape::subgroupExpanded ( )
{
  if ( this->isExpanded() )
    return;

  /* this avoids inconsistent states where a group is *
   * collapsed and one of its subgroups is expanded   */
  this->mode = GRP_EXPANDED;
  ((Node *)this)->setActive ( false );
  this->setFlag ( ItemIsMovable, false );

  /* hide edges that were attached to this group */
  foreach ( AbstractEdgeShape *aes, this->edges )
    aes->updateVisibility ( );

  /* keep in touch with the group this belongs to */
  if ( this->hasGroup() )
    this->group->subgroupExpanded();
}


/*
 * Destructor
 */
AbstractGroupShape::~AbstractGroupShape ( )
{
}
