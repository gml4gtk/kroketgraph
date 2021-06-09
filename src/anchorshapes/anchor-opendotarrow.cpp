/*
 * anchor-opendotarrow.cpp
 *
 * Implementation of the AnchorOpenDotArrow object's methods.
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
#include "anchor-opendotarrow.h"


#define ARROW_LENGTH 14

#define DOT_RADIUS   4
#define DOT_DIAM     8


/*
 * Constructor
 */
AnchorOpenDotArrow::AnchorOpenDotArrow ( AbstractEdgeShape *e,
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
AnchorOpenDotArrow::clone ( AbstractEdgeShape *e,
                              AnchorType t )
{
  AbstractAnchor *ret = new AnchorOpenDotArrow ( e, t );
  ret->initFromAnchor ( this );
  return ret;
}


/*
 * Update an anchor shape.
 */
void
AnchorOpenDotArrow::update ( )
{
  QPointF p = this->pos - this->edge->pos();
  qreal arg = this->angle;

  /* arrow */
  this->c.setX ( p.x() - ( DOT_RADIUS * cos(arg)) );
  this->c.setY ( p.y() - ( DOT_RADIUS * sin(arg) ) );

  arg = arg + M_PI - (M_PI/8);

  this->a1.setX ( this->c.x() + ( ARROW_LENGTH * cos(arg)) );
  this->a1.setY ( this->c.y() + ( ARROW_LENGTH * sin(arg) ) );

  arg += M_PI/4;
  this->a2.setX ( this->c.x() + ( ARROW_LENGTH * cos(arg) ) );
  this->a2.setY ( this->c.y() + ( ARROW_LENGTH * sin(arg) ) );

  /* open dot */
  this->ellipse = QRectF ( p.x()-DOT_RADIUS, p.y()-DOT_RADIUS, DOT_DIAM, DOT_DIAM );
}


/*
 * Draw an anchor shape.
 */
void
AnchorOpenDotArrow::draw ( QPainter *painter ) const
{
  /* arrow */
  painter->drawLine ( c, a1 );
  painter->drawLine ( c, a2 );

  /* open dot */
  painter->setBrush ( this->brush );
  painter->drawEllipse ( this->ellipse );
}


/*
 * Destructor
 */
AnchorOpenDotArrow::~AnchorOpenDotArrow ( )
{
}
