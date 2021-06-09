/*
 * anchor-arrow.cpp
 *
 * Implementation of the AnchorArrow object's methods.
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
#include "anchor-arrow.h"


#define ARROW_LENGTH 14


/*
 * Constructor
 */
AnchorArrow::AnchorArrow ( AbstractEdgeShape *e,
                           AnchorType t )
  : AbstractAnchor ( e, t )
{
}


/*
 * Clone an anchor.
 */
AbstractAnchor *
AnchorArrow::clone ( AbstractEdgeShape *e,
                     AnchorType t )
{
  AbstractAnchor *ret = new AnchorArrow ( e, t );
  ret->initFromAnchor ( this );
  return ret;
}


/*
 * Update an anchor shape.
 */
void
AnchorArrow::update ( )
{
  qreal arg = this->angle + M_PI - (M_PI/8);

  this->c = this->pos - this->edge->pos();

  this->a1.setX ( this->c.x() + ( ARROW_LENGTH * cos(arg)) );
  this->a1.setY ( this->c.y() + ( ARROW_LENGTH * sin(arg) ) );

  arg += M_PI/4;
  this->a2.setX ( this->c.x() + ( ARROW_LENGTH * cos(arg) ) );
  this->a2.setY ( this->c.y() + ( ARROW_LENGTH * sin(arg) ) );
}


/*
 * Draw an anchor shape.
 */
void
AnchorArrow::draw ( QPainter *painter ) const
{
  painter->drawLine ( c, a1 );
  painter->drawLine ( c, a2 );
}


/*
 * Destructor
 */
AnchorArrow::~AnchorArrow ( )
{
}
