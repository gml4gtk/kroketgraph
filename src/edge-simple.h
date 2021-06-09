/*
 * edge-simple.h
 *
 * Declaration of the EdgeSimple class.
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

#ifndef __EDGE_SIMPLE_H__
#define __EDGE_SIMPLE_H__


#include "abstractedgeshape.h"
#include "parser/entity.h"


class EdgeSimple : public AbstractEdgeShape
{
 public:
  EdgeSimple ( AbstractNodeShape *, AbstractNodeShape * );
  virtual ~EdgeSimple ( );

  /*
   * Initialize an edge using an Entity object.
   */
  virtual void initFromEntity ( const Entity * );

  /*
   * Clone an edge shape.
   */
  virtual AbstractEdgeShape *clone ( AbstractNodeShape *, AbstractNodeShape * );

  /*
   * Drawing related functions.
   */
  virtual QRectF boundingRect ( ) const;
  virtual QPainterPath shape ( ) const;
  virtual void paint ( QPainter *, const QStyleOptionGraphicsItem *, QWidget *w=NULL );

} ;


#endif
