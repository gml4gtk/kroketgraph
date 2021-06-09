/*
 * abstractedgeshape.h
 *
 * Declaration of the AbstractEdgeShape class.
 * This class is abstract.
 * It provides the basic behaviour of edges displayed.
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

#ifndef __ABSTRACTEDGESHAPE_H__
#define __ABSTRACTEDGESHAPE_H__

#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsSimpleTextItem>
#include <QtWidgets/QGraphicsItem>

#include "abstractshape.h"
#include "abstractnodeshape.h"
#include "graph/edge.h"
#include "parser/entity.h"

#define LOOP_WIDTH 24


class AbstractEdgeShape : public AbstractShape, public Edge
{
 public:
  AbstractEdgeShape ( AbstractNodeShape *, AbstractNodeShape * );
  virtual ~AbstractEdgeShape ( );

  /*
   * Initialize an edge shape using an Entity object.
   */
  virtual void initFromEntity ( const Entity * );

  /*
   * Clone an edge shape.
   */
  virtual AbstractEdgeShape *clone ( AbstractNodeShape *, AbstractNodeShape * ) = 0;

  /*
   * Initialize the pen used to draw the 'hovered' hint.
   */
  virtual void initHoverPen ( );

  /*
   * Sets the visibility/activation state of an edge according to its ports.
   */
  virtual void updateVisibility ( );

  /*
   * Compute the path of the edge.
   */
  virtual void computePath ( );

  /*
   * Compute the position of the text.
   */
  virtual void computeTextPos ( );

  /*
   * Get the screen X coordinate of the last control point towards the provided anchor.
   */
  virtual qreal getControlX ( const AbstractAnchor * );

  /*
   * Drawing related functions.
   */
  virtual QRectF boundingRect ( ) const = 0;
  virtual QPainterPath shape ( ) const = 0;
  virtual void paint ( QPainter *, const QStyleOptionGraphicsItem *, QWidget * ) = 0;

  /*
   * Callback for mouse clicks
   */
  virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * );

  /*
   * Callback for the mouse hovering
   */
  virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * );
  virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * );
  inline bool isHovered ( ) const { return this->hovered; }


 public:
  AbstractAnchor *srcAnchor;      /* source anchor */
  AbstractAnchor *destAnchor;     /* destination anchor */

  QPainterPath path;  /* path of the edge */
  QPainterPath pathD;

  bool hovered;       /* an hovered edge means that the mouse cursor is above it */
  QPen hoverPen;      /* pen used when drawing the 'hovered' hint */

} ;


#endif
