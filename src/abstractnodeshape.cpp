/*
 * abstractnodeshape.cpp
 *
 * Implementation of the AbstractNodeShape object's methods.
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

#include "graphview.h"
#include "abstractedgeshape.h"
#include "abstractgroupshape.h"
#include "abstractanchor.h"
#include "port.h"
#include "abstractnodeshape.h"


/*
 * Constructor
 */
AbstractNodeShape::AbstractNodeShape ( GraphView *gv )
  : AbstractShape ( ), Node ( gv->getGraph() ), graphView ( gv )
{
  this->group = NULL;

  this->setZValue ( 2 );
  this->setFlag ( ItemIsSelectable );

  this->ports.insert ( "default", new QList<Port *> );
  this->ports.insert ( "top", new QList<Port *> );
  this->ports.insert ( "bottom", new QList<Port *> );

  this->brush.setColor ( "white" );
  this->brush.setStyle ( Qt::SolidPattern );

  this->anim.setItem ( this );
  this->anim.setTimeLine ( &this->timeline );
  this->timeline.setCurveShape ( QTimeLine::EaseInOutCurve );

  this->setData ( DATA_TYPE, TYPE_NODE );
}


/*
 * Initialize an AbstractNodeShape from an entity.
 */
void
AbstractNodeShape::initFromEntity ( const Entity *e )
{
  AbstractShape::initFromEntity ( e );
  Node::initFromEntity ( e );

  this->pack ( );
}


/*
 * Set the position of an AbstractNodeShape (overloading QGraphicsItem).
 */
void
AbstractNodeShape::setPos ( qreal x,
                            qreal y )
{
  AbstractShape::setPos ( x, y );
  this->anim.setPosAt ( 1.0, QPointF(x,y) ); /* graphview's focusOn() relies on this */
  this->updateAnchors ( );
}

void
AbstractNodeShape::setPos ( const QPointF &pos )
{
  this->setPos ( pos.x(), pos.y() );
}


/*
 * Center the node on the provided coordinates.
 */
void
AbstractNodeShape::centerOn ( qreal x,
                              qreal y )
{
  this->setPos ( x-(getWidth()/2.0), y-(getHeight()/2.0) );
}

void
AbstractNodeShape::centerOn ( const QPointF &pos )
{
  this->centerOn ( pos.x(), pos.y() );
}


/*
 * Move the node to the given position using an animation.
 */
void
AbstractNodeShape::animateSetPos ( qreal x,
                                   qreal y )
{
  if ( this->timeline.state() == QTimeLine::Running )
    this->timeline.stop ( );

  this->anim.setPosAt ( 0.0, this->pos() );
  this->anim.setPosAt ( 1.0, QPointF(x,y) );
  this->timeline.start ( );
}

void
AbstractNodeShape::animateSetPos ( const QPointF &pos )
{
  this->animateSetPos ( pos.x(), pos.y() );
}

void
AbstractNodeShape::animateCenterOn ( qreal x,
                                     qreal y )
{
  this->animateSetPos ( x-(getWidth()/2.0), y-(getHeight()/2.0) );
}

void
AbstractNodeShape::animateCenterOn ( const QPointF &pos )
{
  this->animateCenterOn ( pos.x(), pos.y() );
}


/*
 * Compute the coordinates of the center of an AbstractNodeShape.
 */
QPointF
AbstractNodeShape::center ( )
{
  QPointF rc = this->boundingRect().center ( );
  return QPointF ( this->x()+rc.x(), this->y()+rc.y() );
}

QPointF
AbstractNodeShape::finalCenter ( )
{
  QPointF rc = this->boundingRect().center ( );
  QPointF pos = this->finalPos ( );
  return QPointF ( pos.x()+rc.x(), pos.y()+rc.y() );
}


/*
 * Register an anchor with the provided port name.
 */
void
AbstractNodeShape::addLoopingEdge ( AbstractEdgeShape *aes )
{
  this->loops.append ( aes );
  aes->setEnabled ( false );   /* the edge must not receive click or hover events */
  aes->setParentItem ( this ); /* the edge is drawn under the control of this node */

  Port *sp = new Port ( this, aes->srcAnchor );
  Port *dp = new Port ( this, aes->destAnchor );

  this->l_ports.insert ( aes, QPair<Port*,Port*>(sp,dp) );
}


/*
 * Register an anchor with the provided port name.
 */
void
AbstractNodeShape::registerAnchor ( AbstractAnchor *anchor,
                                    const char *name )
{
  /* loops are ignored because their anchors are not handled */
  if ( anchor->edge->isLoop() )
    return;

  /* extract the anchor's region and attribute */
  QString aRegion = Node::extractAnchorRegion ( name );
  QString aAttr = Node::extractAnchorAttr ( name );

  if ( !this->ports.contains(aRegion) )
    {
      std:: cerr << "warning: anchor region name \'" << qPrintable(aRegion) << "\' is invalid.\n";
      aRegion = "default";
    }

  anchor->setRegion ( aRegion );
  anchor->setAttr ( aAttr );

  this->registerAnchor ( anchor );
}

void
AbstractNodeShape::registerAnchor ( AbstractAnchor *anchor )
{
  Port *p;
  QList<Port *> *l = this->ports.value ( anchor->region );

  if ( anchor->attr == NODE_ANCHOR_ATTR_NULL )
    {
      /* if the anchor has no extra attribute, then it has a dedicated port */
      p = new Port ( this, anchor );
      l->append ( p );
    }
  else
    {
      /* we look for the port that matches the extra attribute */
      p = NULL;

      foreach ( Port *exp, *l )
        {
          if ( exp->attr == anchor->attr )
            {
              p = exp;
              break;
            }
        }

      if ( p == NULL ) /* no matching port, create a new one */
        {
          p = new Port ( this, anchor );
          l->append ( p );
        }
      else             /* matching port found, add the anchor to it */
        p->addAnchor ( anchor );
    }
}


/*
 * Update the anchors of an AbstractNodeShape.
 */
void
AbstractNodeShape::updateAnchors ( )
{
  int i;
  Port *p;
  qreal offset, pad;
  QList<Port *> *def = this->ports.value ( "default" );
  QList<Port *> top ( *this->ports.value("top") );     /* list is copied (very fast) */
  QList<Port *> bot ( *this->ports.value("bottom") );  /* list is copied (very fast) */

  /* process the ports list (remove inactive ports, handle 'default' ports) */
  foreach ( p, top )
    if ( !p->isActive() )
      qlist_remove ( top, p );

  foreach ( p, bot )
    if ( !p->isActive() )
      qlist_remove ( bot, p );

  foreach ( p, *def )
    {
      if ( !p->isActive() )
        continue;

      AbstractAnchor *a = p->anchors.at ( 0 );

      if ( ((Edge *)a->edge)->isActive() )
        {
          if ( this == a->edge->src )
            {
              if ( this->y() > ((AbstractNodeShape *)a->edge->dest)->y() )
                top.append ( p );
              else
                bot.append ( p );
            }
          else
            {
              if ( this->y() > ((AbstractNodeShape *)a->edge->src)->y() )
                top.append ( p );
              else
                bot.append ( p );
            }
        }
      else
        {
          if ( ( a->edge->src->isActive() ) && ( this == a->edge->src ) )
            bot.append ( p );
          else if ( ( a->edge->dest->isActive() ) && ( this == a->edge->dest ) )
            top.append ( p );
        }
    }

  /* top ports sorting and position assignment */
  qSort ( top.begin(), top.end(), Port::comparePorts );

  pad = (qreal) getWidth() / (qreal) top.size();
  offset = (getWidth()/2) - ((pad*(top.size()-1)) / 2.0 );

  for ( i=0; i<top.size(); ++i )
    {
      top[i]->setPos ( offset+(pad*i), 0 );
      top[i]->updateAnchors ( );
    }


  /* bottom ports sorting and position assignment */
  qSort ( bot.begin(), bot.end(), Port::comparePorts );

  pad = (qreal) getWidth() / (qreal) bot.size();
  offset = (getWidth()/2) - ((pad*(bot.size()-1)) / 2.0 );

  for ( i=0; i<bot.size(); ++i )
    {
      bot[i]->setPos ( offset+(pad*i), getHeight() );
      bot[i]->updateAnchors ( );
    }


  /* process loops */
  offset = 0;
  QPair<Port *, Port *> pair;
  foreach ( AbstractEdgeShape *aes, this->loops )
    {
      aes->srcAnchor->setPos ( offset+getWidth(), getHeight()-4 );
      aes->destAnchor->setPos ( offset+getWidth(), 4 );

      pair = this->l_ports.value ( aes );
      pair.first->setPos ( offset+getWidth(), getHeight()-4 );
      pair.second->setPos ( offset+getWidth(), 4 );

      /* Note that the updateAnchors function is not called so the anchors are not moved. *
       * Anchors are indeed handled with the coordinate system of the scene, while        *
       * loops are children items of the node.                                            */

      offset += (LOOP_WIDTH+2);
    }
}


/*
 * Visibility
 */
void
AbstractNodeShape::setVisible ( bool visible )
{
  QGraphicsItem::setVisible ( visible );

  /* modify attached edges state */
  foreach ( AbstractEdgeShape *aes, this->edges )
    {
      aes->updateVisibility ( );

      if ( aes->isVisible() )
        {
          aes->computePath ( );
          aes->update ( );
        }
    }
}

void
AbstractNodeShape::show ( )
{
  this->setVisible ( true );
}

void
AbstractNodeShape::hide ( )
{
  this->setVisible ( false );
}


/*
 * Hide and deactivate an AbstractNodeShape and all its adjacent nodes below.
 */
void
AbstractNodeShape::hideAndDeactivate ( )
{
  AbstractNodeShape *ans;

  ((Node *)this)->setActive ( false );
  this->hide ( );

  foreach ( Node *n, ((Node *)this)->children )
    {
      ans = (AbstractNodeShape *) n;

      if ( ans->y() > this->y() )
        ans->hideAndDeactivate ( );
    }

  foreach ( Node *n, ((Node *)this)->parents )
    {
      ans = (AbstractNodeShape *) n;

      if ( ans->y() > this->y() )
        ans->hideAndDeactivate ( );
    }

  if ( this->hasGroup() )
    this->group->pack ( );
}


/*
 * Does the node has an active group ? (in other words, is the node is overriden ?)
 * If the group is collapsed, it means that either the group itself or its parent group
 * is active and overrides the node.
 */
bool
AbstractNodeShape::hasActiveGroup ( ) const
{
  if ( this->group )
    return this->group->isCollapsed ( );

  return false;
}


/*
 * Callback for item state changes
 */
QVariant
AbstractNodeShape::itemChange ( GraphicsItemChange change,
                                const QVariant &value )
{
  switch ( change )
    {
    case ItemPositionHasChanged:
      {
        this->updateAnchors ( );

        foreach ( Node *node, ((Node*)this)->children )
          ((AbstractNodeShape *) node)->updateAnchors ( );

        foreach ( Node *node, parents )
          ((AbstractNodeShape *) node)->updateAnchors ( );

        if ( this->hasGroup() )
          {
            this->group->pack ( );
            this->group->update ( );
          }

        break;
      }

    default:
      break;
    };

  return QGraphicsItem::itemChange(change, value);
}


/*
 * Callback for mouse double click events
 */
void
AbstractNodeShape::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent *event )
{
  if ( event->buttons() & Qt::LeftButton )
    this->graphView->traceFrom ( this );

  Q_UNUSED ( event );
}


/*
 * Callback for mouse clicks
 */
void
AbstractNodeShape::mouseReleaseEvent ( QGraphicsSceneMouseEvent *event )
{
  if ( event->buttons() & Qt::LeftButton )
    {
      /* we check if an edge is not the real target of the click */
      QList<QGraphicsItem *> l ( this->scene()->items(event->scenePos()) );

      foreach ( QGraphicsItem *gi, l )
        {
          if ( IS_EDGE(gi) )
            {
              ((AbstractEdgeShape *) gi)->mouseReleaseEvent ( event );
              return;
            }
        }
    }

  QGraphicsItem::mouseReleaseEvent ( event );
}


/*
 * Destructor
 */
AbstractNodeShape::~AbstractNodeShape ( )
{
  foreach ( QList<Port *> *l, this->ports )
    qDeleteAll ( *l );

  QPair<Port *, Port *> p;
  foreach ( p, this->l_ports )
    {
      delete p.first;
      delete p.second;
    }

  qDeleteAll ( this->ports );
}
