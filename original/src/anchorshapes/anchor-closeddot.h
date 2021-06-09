/*
 * anchor-closeddot.h
 *
 * Declaration of the AnchorClosedDot class.
 * It provides the behaviour of an anchor with closed dot shape.
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

#ifndef __ANCHOR_CLOSEDDOT_H__
#define __ANCHOR_CLOSEDDOT_H__


#include "abstractanchor.h"


#define ANCHOR_SHAPE_CLOSEDDOT "closed_dot"


class AnchorClosedDot : public AbstractAnchor
{
 public:
  AnchorClosedDot ( AbstractEdgeShape *, AnchorType );
  virtual ~AnchorClosedDot ( );

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
  QRectF ellipse;
  QBrush brush;

} ;


#endif
