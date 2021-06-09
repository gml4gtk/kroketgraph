/*
 * node-circle.cpp
 *
 * Implementation of the NodeCircle object's methods.
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

#include "node-circle.h"


/*
 * Constructor
 */
NodeCircle::NodeCircle ( GraphView *gv )
  : AbstractNodeShape ( gv )
{
  this->setFlag ( ItemIsMovable );
}


/*
 * Pack the circle shape according to the content (text) of a node.
 */
void
NodeCircle::pack ( )
{
  if ( this->hasText() )
    {
      int tx, ty;
      QRectF r;

      r = this->text->boundingRect ( );

      if ( r.height() > r.width() )
        this->radius = (int) (r.height() + 20.0 );
      else
        this->radius = (int) (r.width() + 20.0 );

      tx = ( this->radius - (int)r.width() ) / 2;
      ty = ( this->radius - (int)r.height() ) / 2;

      this->text->setPos ( tx, ty );
    }
  else
    this->radius = 12;
}


/*
 * Build the minimal rectangle which contains a node circle.
 */
QRectF
NodeCircle::boundingRect ( ) const
{
  const int pw = this->pen.width();

  return QRectF ( -pw-1, -pw-1, this->radius+pw+1, this->radius+pw+1 ) | this->childrenBoundingRect();
}


/*
 * Build a path following the shape of an item.
 */
QPainterPath
NodeCircle::shape ( ) const
{
  QPainterPath path;
  path.addEllipse ( 0, 0, this->radius, this->radius );
  return path;
}


/*
 * Paint an item.
 */
void
NodeCircle::paint ( QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *w )
{
  painter->setBrush ( this->brush );
  painter->setPen ( this->pen );

  painter->drawEllipse ( 0, 0, this->radius, this->radius );

  //paintBoundingRect ( painter );

  //if ( !this->expanded )
  //  painter->drawLine ( 0, this->radius+3, this->radius, this->radius+3 );

  Q_UNUSED ( w );
  Q_UNUSED ( option );
}


/*
 * Destructor
 */
NodeCircle::~NodeCircle ( )
{
}
