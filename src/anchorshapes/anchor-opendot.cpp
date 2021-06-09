/*
 * anchor-opendot.cpp
 *
 * Implementation of the AnchorOpenDot object's methods.
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

#include "anchor-opendot.h"


#define DOT_RADIUS 4
#define DOT_DIAM   8


/*
 * Constructor
 */
AnchorOpenDot::AnchorOpenDot ( AbstractEdgeShape *e,
                               AnchorType t )
  : AbstractAnchor ( e, t )
{
  this->brush.setColor ( QColor("white") );
  this->brush.setStyle ( Qt::SolidPattern );
}


/*
 * Clone an anchor.
 */
AbstractAnchor *
AnchorOpenDot::clone ( AbstractEdgeShape *e,
                       AnchorType t )
{
  AbstractAnchor *ret = new AnchorOpenDot ( e, t );
  ret->initFromAnchor ( this );
  return ret;
}


/*
 * Update an anchor shape.
 */
void
AnchorOpenDot::update ( )
{
  QPointF p = this->pos - this->edge->pos();
  this->ellipse = QRectF ( p.x()-DOT_RADIUS, p.y()-DOT_RADIUS, DOT_DIAM, DOT_DIAM );
}


/*
 * Draw an anchor shape.
 */
void
AnchorOpenDot::draw ( QPainter *painter ) const
{
  painter->setBrush ( this->brush );
  painter->drawEllipse ( this->ellipse );
}


/*
 * Destructor
 */
AnchorOpenDot::~AnchorOpenDot ( )
{
}
