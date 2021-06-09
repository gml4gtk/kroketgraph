/*
 * node-rectangle.h
 *
 * Declaration of the NodeRectangle class.
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

#ifndef __NODE_RECTANGLE_H__
#define __NODE_RECTANGLE_H__


#include "abstractnodeshape.h"
#include "parser/entity.h"


#define NODE_SHAPE_RECTANGLE "rectangle"


class NodeRectangle : public AbstractNodeShape
{
 public:
  NodeRectangle ( GraphView * );
  virtual ~NodeRectangle ( );

  /*
   * Pack the shape according to its content (text).
   */
  virtual void pack ( );

  /*
   * Drawing related functions
   */
  virtual QSizeF size ( ) const { return QSizeF ( width, height ); }
  virtual QRectF boundingRect ( ) const;
  virtual qreal getWidth ( ) const { return width; };
  virtual qreal getHeight ( ) const { return height; }
  virtual QPainterPath shape ( ) const;
  virtual void paint ( QPainter *, const QStyleOptionGraphicsItem *, QWidget *w=NULL );


 protected:
  int width;
  int height;

} ;


#endif
