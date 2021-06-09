/*
 * anchor-closeddotarrow.h
 *
 * Declaration of the AnchorClosedDotArrow class.
 * It provides the behaviour of an anchor with closed dot arrow shape.
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

#ifndef __ANCHOR_CLOSEDDOTARROW_H__
#define __ANCHOR_CLOSEDDOTARROW_H__


#include "abstractanchor.h"


#define ANCHOR_SHAPE_CLOSEDDOTARROW "closed_dot_arrow"


class AnchorClosedDotArrow : public AbstractAnchor
{
 public:
  AnchorClosedDotArrow ( AbstractEdgeShape *, AnchorType );
  virtual ~AnchorClosedDotArrow ( );

  /*
   * Clone an anchor.
   */
  virtual AbstractAnchor *clone ( AbstractEdgeShape *, AnchorType );

  /*
   * Update an anchor shape.
   */
  virtual void update ( );

  /*
   * Draw an anchor shape.
   */
  virtual void draw ( QPainter * ) const;


 protected:
  QPointF a1;
  QPointF a2;
  QPointF c;

  QRectF ellipse;
  QBrush brush;

} ;


#endif
