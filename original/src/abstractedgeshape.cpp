/*
 * abstractedgeshape.cpp
 *
 * Implementation of the AbstractEdgeShape object's methods.
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

#include <cmath>
#include "graphview.h"
#include "abstractanchor.h"
#include "anchorshapes/anchor-line.h"
#include "anchorshapes/anchor-arrow.h"
#include "anchorshapes/anchor-opendot.h"
#include "anchorshapes/anchor-opendotarrow.h"
#include "anchorshapes/anchor-closeddot.h"
#include "anchorshapes/anchor-closeddotarrow.h"
#include "parser/defs.h"
#include "abstractedgeshape.h"


/*
 * Constructor
 */
AbstractEdgeShape::AbstractEdgeShape ( AbstractNodeShape *src,
                                       AbstractNodeShape *dest )
  : AbstractShape ( ), Edge ( src, dest )
{
  this->setZValue ( 3 );
  this->setFlag ( ItemIsSelectable );

  if ( !this->isLoop() )
    {
      src->edges.append ( this );
      dest->edges.append ( this );
      this->setAcceptsHoverEvents ( true );
      this->setHandlesChildEvents ( true );
    }

  this->hovered = false;

  this->setData ( DATA_TYPE, TYPE_EDGE );
}


/*
 * Initialize an edge shape using an Entity object.
 */
void
AbstractEdgeShape::initFromEntity ( const Entity *e )
{
  const char *attr_v;

  AbstractShape::initFromEntity ( e );
  Edge::initFromEntity ( e );

  /* src anchor */
  attr_v = e->getValueOfAttribute ( ATTR_EDGE_SRC_STYLE );

  if ( attr_v == NULL )
    this->srcAnchor = new AnchorLine ( this, ANCHOR_SRC );
  else if ( strcmp(attr_v,ANCHOR_SHAPE_LINE) == 0 )
    this->srcAnchor = new AnchorLine ( this, ANCHOR_SRC );
  else if ( strcmp(attr_v,ANCHOR_SHAPE_ARROW) == 0 )
    this->srcAnchor = new AnchorArrow ( this, ANCHOR_SRC );
  else if ( strcmp(attr_v,ANCHOR_SHAPE_CLOSEDDOT) == 0 )
    this->srcAnchor = new AnchorClosedDot ( this, ANCHOR_SRC );
  else if ( strcmp(attr_v,ANCHOR_SHAPE_CLOSEDDOTARROW) == 0 )
    this->srcAnchor = new AnchorClosedDotArrow ( this, ANCHOR_SRC );
  else if ( strcmp(attr_v,ANCHOR_SHAPE_OPENDOT) == 0 )
    this->srcAnchor = new AnchorOpenDot ( this, ANCHOR_SRC );
  else if ( strcmp(attr_v,ANCHOR_SHAPE_OPENDOTARROW) == 0 )
    this->srcAnchor = new AnchorOpenDotArrow ( this, ANCHOR_SRC );
  else
    {
      this->srcAnchor = new AnchorLine ( this, ANCHOR_SRC );
      std::cerr << "warning: anchor shape \'" << attr_v << "\' is invalid.\n";
    }

  if ( (attr_v = e->getValueOfAttribute ( ATTR_EDGE_SRC_TEXT ) ) != NULL )
    this->srcAnchor->setText ( attr_v );

  ((AbstractNodeShape *) this->src)->registerAnchor ( this->srcAnchor, e->getValueOfAttribute(ATTR_EDGE_SRC_PORT) );
    

  /* dest anchor */
  attr_v = e->getValueOfAttribute ( ATTR_EDGE_DEST_STYLE );

  if ( attr_v == NULL )
    this->destAnchor = new AnchorArrow ( this, ANCHOR_DEST );
  else if ( strcmp(attr_v,ANCHOR_SHAPE_ARROW) == 0 )
    this->destAnchor = new AnchorArrow ( this, ANCHOR_DEST );
  else if ( strcmp(attr_v,ANCHOR_SHAPE_LINE) == 0 )
    this->destAnchor = new AnchorLine ( this, ANCHOR_DEST );
  else if ( strcmp(attr_v,ANCHOR_SHAPE_CLOSEDDOT) == 0 )
    this->destAnchor = new AnchorClosedDot ( this, ANCHOR_DEST );
  else if ( strcmp(attr_v,ANCHOR_SHAPE_CLOSEDDOTARROW) == 0 )
    this->destAnchor = new AnchorClosedDotArrow ( this, ANCHOR_DEST );
  else if ( strcmp(attr_v,ANCHOR_SHAPE_OPENDOT) == 0 )
    this->destAnchor = new AnchorOpenDot ( this, ANCHOR_DEST );
  else if ( strcmp(attr_v,ANCHOR_SHAPE_OPENDOTARROW) == 0 )
    this->destAnchor = new AnchorOpenDotArrow ( this, ANCHOR_DEST );
  else
    {
      this->destAnchor = new AnchorArrow ( this, ANCHOR_DEST );
      std::cerr << "warning: anchor shape \'" << attr_v << "\' is invalid.\n";
    }

  if ( (attr_v = e->getValueOfAttribute ( ATTR_EDGE_DEST_TEXT ) ) != NULL )
    this->destAnchor->setText ( attr_v );

  ((AbstractNodeShape *) this->dest)->registerAnchor ( this->destAnchor, e->getValueOfAttribute(ATTR_EDGE_DEST_PORT) );

  /* hover pen */
  this->initHoverPen ( );
}


/*
 * Sets the visibility/activation state of an edge according to its ports.
 * An edge is visible when :
 * 1) it links two active nodes
 * 2) it links an active node to an unactive node which is _not_ overriden by its group.
 */
void
AbstractEdgeShape::updateVisibility ( )
{
  if ( ((Edge *)this)->isActive() )
    {
      this->show ( );
      this->srcAnchor->setActive ( true );
      this->destAnchor->setActive ( true );
      /*
        std::cout << "updating visibility of edge \'" << this->id.toAscii().constData() << "\' (active)\n";
       */
      return;
    }

  if ( ( this->src->isActive() ) && ( ! ((AbstractNodeShape*)this->dest)->hasActiveGroup() ) )
    {
      this->show ( );
      this->srcAnchor->setActive ( true );
      this->destAnchor->setActive ( false );
      /*
        std::cout << "updating visibility of edge \'" << this->id.toAscii().constData() << "\' (src is active)\n";
      */
      return;
    }

  if ( ( this->dest->isActive() ) && ( ! ((AbstractNodeShape*)this->src)->hasActiveGroup() ) )
    {
      this->show ( );
      this->srcAnchor->setActive ( false );
      this->destAnchor->setActive ( true );
      /*
        std::cout << "updating visibility of edge \'" << this->id.toAscii().constData() << "\' (dest is active)\n";
      */
      return;
    }

  this->hide ( );
  this->srcAnchor->setActive ( false );
  this->destAnchor->setActive ( false );
  /*
    std::cerr << "updating visibility of edge \'" << this->id.toAscii().constData() << "\' (inactive)\n";
  */
}


/*
 * Compute the angle of the line linking two points.
 */
static inline qreal
angle ( QPointF p1,
        QPointF p2 )
{
  qreal vx, vy, modul, arg;

  vx = p2.x() - p1.x();
  vy = p2.y() - p1.y();

  modul = sqrt ( (vx*vx) + (vy*vy) );

  if ( modul != 0 )
    {
      vx /= modul;
      vy /= modul;
    }

  arg = acos ( vx );

  if ( vy < 0 )
    arg = -arg;

  return arg;
}


/*
 * Compute the path of the edge.
 */
void
AbstractEdgeShape::computePath ( )
{
  QPointF p;
  qreal arg;
  GraphView *gv = ((AbstractNodeShape *) this->src)->graphView;
  this->prepareGeometryChange ( );
  this->path = QPainterPath ( );

  if ( this->isLoop() )          /* loops */
    {
      this->setPos ( 0, 0 );
      this->path.moveTo ( this->srcAnchor->pos );
      this->path.arcTo ( this->srcAnchor->pos.x(), this->srcAnchor->pos.y(),
                         LOOP_WIDTH, this->destAnchor->pos.y()-this->srcAnchor->pos.y(),
                         90, -180 );
      this->path.lineTo ( this->destAnchor->pos );
      this->pathD = this->path;
      this->srcAnchor->angle = M_PI;
      this->destAnchor->angle = M_PI;
    }
  else                           /* node to node edges */
    {
      if ( !this->src->isActive() )
        {
          p = this->destAnchor->pos;

          if ( this->destAnchor->region == "bottom" ) /* it would be better if the node handled this in updateAnchor() */
            this->srcAnchor->pos = QPointF ( p.x(), p.y()+50 );
          else
            this->srcAnchor->pos = QPointF ( p.x(), p.y()-50 );

          this->setPos ( this->srcAnchor->pos );

          arg = angle ( this->srcAnchor->pos, this->destAnchor->pos );
          this->srcAnchor->angle = -arg;
          this->destAnchor->angle = arg;
        }
      else if ( !this->dest->isActive() )
        {
          p = this->srcAnchor->pos;

          if ( this->srcAnchor->region == "top" ) /* it would be better if the node handled this in updateAnchor() */
            this->destAnchor->pos = QPointF ( p.x(), p.y()-50 );
          else
            this->destAnchor->pos = QPointF ( p.x(), p.y()+50 );

          this->setPos ( this->srcAnchor->pos );

          arg = angle ( this->srcAnchor->pos, this->destAnchor->pos );
          this->srcAnchor->angle = -arg;
          this->destAnchor->angle = arg;
        }
      else
        {
          QMap<int,quint32>::const_iterator iter = this->virtualPoints.constBegin();
          this->setPos ( this->srcAnchor->pos );
          p = this->srcAnchor->pos;

          if ( iter != this->virtualPoints.constEnd() ) /* get the first virtual point in order to compute the angle */
            {
              p = QPointF ( gv->gridCellHCenter[iter.value()], gv->gridCellVCenter[abs(iter.key())] );
              this->path.lineTo ( p - this->srcAnchor->pos );
              ++iter;

              arg = angle ( p, this->srcAnchor->pos );
              this->srcAnchor->angle = arg;

              while ( iter != this->virtualPoints.constEnd() )
                {
                  p = QPointF ( gv->gridCellHCenter[iter.value()], gv->gridCellVCenter[abs(iter.key())] );
                  this->path.lineTo ( p - this->srcAnchor->pos );
                  ++iter;
                }
            }
          else
            {
              arg = angle ( this->destAnchor->pos, this->srcAnchor->pos );
              this->srcAnchor->angle = arg;
            }

          arg = angle ( p, this->destAnchor->pos );
          this->destAnchor->angle = arg;
        }

      this->path.lineTo ( this->destAnchor->pos - this->srcAnchor->pos );

      /* we create a secondary path to workaround some strange behavior of Qt.                                    *
       * this secondary path has a T at its end, and fixes the shape() and boundingRect() functions.              *
       * those two functions are used for the hover and click events.                                             *
       * if we don't add this T shape, then edges (especially edges with line-line anchors'style) are unclickable */
      this->pathD = this->path;
      arg += M_PI_2;
      p = this->destAnchor->pos - this->srcAnchor->pos;
      qreal ax = 6*cos(arg);
      qreal ay = 6*sin(arg);
      this->pathD.lineTo ( p.x()+ax, p.y()+ay );
      this->pathD.lineTo ( p.x()-ax, p.y()-ay );
    }

  /* update the anchors and the text */
  this->srcAnchor->update ( );
  this->destAnchor->update ( );
  this->computeTextPos ( );
}


/*
 * Compute the position of the text.
 */
void
AbstractEdgeShape::computeTextPos ( )
{
  if ( !this->hasText() )
    return;

  QPointF ptext ( this->path.pointAtPercent(0.5) );
  QRectF r ( this->text->boundingRect() );
  qreal arg = angle ( this->path.pointAtPercent(0.45), this->path.pointAtPercent(0.55) );

  this->text->resetTransform ( );
  this->text->setPos ( ptext.x()-(r.height()*sin(arg))+((r.width()/2.0)*cos(arg)), // center according to text bounding rect
                       ptext.y()+(r.height()*cos(arg))+((r.width()/2.0)*sin(arg)) );

  this->text->rotate ( ( (arg+M_PI) / (M_PI*2.0) ) * 360.0 );
}


/*
 * Get the screen X coordinate of the last control point towards the provided anchor.
 */
qreal
AbstractEdgeShape::getControlX ( const AbstractAnchor *a )
{
  qreal ret;
  GraphView *gv = ((AbstractNodeShape *) this->src)->graphView;
  QList<quint32> vx ( this->virtualPoints.values() );

  if ( vx.size() > 0 )
    {
      if ( a == this->srcAnchor ) 
        ret = gv->gridCellHCenter[vx.first()];
      else
        ret = gv->gridCellHCenter[vx.last()];
    }
  else
    {
      if ( a == this->srcAnchor ) 
        ret = this->destAnchor->pos.x();
      else
        ret = this->srcAnchor->pos.x();
    }

  return ret;
}


/*
 * Callback for mouse clicks.
 */
void
AbstractEdgeShape::mouseReleaseEvent ( QGraphicsSceneMouseEvent *event )
{
  QGraphicsItem::mouseReleaseEvent ( event );

  if ( event->button() != Qt::LeftButton )
    return;

  if ( ((Edge *)this)->isActive() )
    {
      if ( ((AbstractNodeShape*)this->dest)->y() > ((AbstractNodeShape*)this->src)->y() )
        {
          ((AbstractNodeShape*)this->dest)->hideAndDeactivate ( );
          ((AbstractNodeShape*)this->src)->setPos ( event->scenePos() );
        }
      else
        {
          ((AbstractNodeShape*)this->src)->hideAndDeactivate ( );
          ((AbstractNodeShape*)this->dest)->setPos ( event->scenePos() );
        }
    }
  else
    {
      if ( !((Node *)this->src)->isActive() )
        {
          ((Node *)this->src)->setActive ( true );
          ((AbstractNodeShape*)this->src)->show ( );
          ((AbstractNodeShape*)this->src)->setPos ( event->scenePos() );
        }
      else if ( !((Node *)this->dest)->isActive() )
        {
          ((Node *)this->dest)->setActive ( true );
          ((AbstractNodeShape*)this->dest)->show ( );
          ((AbstractNodeShape*)this->dest)->setPos ( event->scenePos() );
        }
    }

  this->computePath ( );
  this->update ( );

  ((AbstractNodeShape*)this->dest)->graphView->synchronizeView ( );
}


/*
 * Callback for the mouse hovering.
 */
void
AbstractEdgeShape::hoverEnterEvent ( QGraphicsSceneHoverEvent *event )
{
  this->hovered = true;
  this->update ( );
  this->srcAnchor->update ( );
  this->destAnchor->update ( );

  Q_UNUSED ( event );
}

void
AbstractEdgeShape::hoverLeaveEvent ( QGraphicsSceneHoverEvent *event )
{
  this->hovered = false;
  this->update ( );
  this->srcAnchor->update ( );
  this->destAnchor->update ( );

  Q_UNUSED ( event );
}


/*
 * Initializes the pen used to draw the hover.
 */
void
AbstractEdgeShape::initHoverPen ( )
{
  this->hoverPen = this->pen;
  this->hoverPen.setColor ( "yellow" );
  this->hoverPen.setWidth ( this->pen.width() + 4 );
}


/*
 * Destructor
 */
AbstractEdgeShape::~AbstractEdgeShape ( )
{
  delete this->srcAnchor;
  delete this->destAnchor;
}
