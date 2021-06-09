/*
 * group-simple.cpp
 *
 * Implementation of the GroupSimple object's methods.
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

#include "edge-simple.h"
#include "group-simple.h"


/*
 * Constructor
 */
GroupSimple::GroupSimple ( GraphView *gv )
  : AbstractGroupShape ( gv )
{
}


/*
 * Initialize a group using an Entity object.
 */
void
GroupSimple::initFromEntity ( const Entity *e )
{
  AbstractGroupShape::initFromEntity ( e );

  this->pen.setColor ( "#09dd35" );
  this->pen.setWidth ( 1 );
  this->brush.setStyle ( Qt::SolidPattern );
  this->brush.setColor ( QColor(250, 255, 253, 120) );
}


/*
 * Pack the group according to its content.
 */
void
GroupSimple::pack ( )
{
  AbstractGroupShape::pack ( );

  /* this simple group uses directly the abstract contour */
}


/*
 * Build the minimal rectangle which contains an edge simple.
 */
QRectF
GroupSimple::boundingRect ( ) const
{
  /* enlarge the bounding rectangle a little */
  return this->contour.boundingRect().adjusted ( -1, -1, 1, 1 );
}


/*
 * Build a path following the shape of an item.
 */
QPainterPath
GroupSimple::shape ( ) const
{
  return this->contour;
}


/*
 * Paint an item.
 */
void
GroupSimple::paint ( QPainter *painter,
                    const QStyleOptionGraphicsItem *option,
                    QWidget *w )
{
  painter->setBrush ( this->brush );
  painter->setPen ( this->pen );

  painter->drawPath ( this->contour );

  // paintBoundingRect ( painter );
  Q_UNUSED ( w );
  Q_UNUSED ( option );
}


/*
 * Destructor
 */
GroupSimple::~GroupSimple ( )
{
}
