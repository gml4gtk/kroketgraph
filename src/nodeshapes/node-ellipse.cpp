/*
 * node-ellipse.cpp
 *
 * Implementation of the NodeEllipse object's methods.
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

#include "node-ellipse.h"


/*
 * Constructor
 */
NodeEllipse::NodeEllipse ( GraphView *gv )
  : AbstractNodeShape ( gv )
{
  this->setFlag ( ItemIsMovable );
}


/*
 * Pack the ellipse shape according to the content (text) of a node.
 */
void
NodeEllipse::pack ( )
{
  if ( this->hasText() )
    {
      int tx, ty;
      QRectF r;

      r = this->text->boundingRect ( );
      this->width = (int) (r.width() * 1.5 );
      this->height = (int) (r.height() * 1.5 );

      if ( this->height > this->width )
        this->width <<= 1;

      tx = ( this->width - (int)r.width() ) / 2;
      ty = ( this->height - (int)r.height() ) / 2;
      this->text->setPos ( tx, ty );
    }
  else
    {
      this->width = 24;
      this->height = 16;
    }
}


/*
 * Build the minimal rectangle which contains a node ellipse.
 */
QRectF
NodeEllipse::boundingRect ( ) const
{
  const int pw = this->pen.width();

  return QRectF ( -pw-1, -pw-1, this->width+pw+1, this->height+pw+1 ) | this->childrenBoundingRect();
}


/*
 * Build a path following the shape of an item.
 */
QPainterPath
NodeEllipse::shape ( ) const
{
  QPainterPath path;
  path.addEllipse ( 0, 0, this->width, this->height );
  return path;
}


/*
 * Paint an item.
 */
void
NodeEllipse::paint ( QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *w )
{
  painter->setBrush ( this->brush );
  painter->setPen ( this->pen );

  painter->drawEllipse ( 0, 0, this->width, this->height );

  // paintBoundingRect ( painter );

  //if ( !this->expanded )
  //  painter->drawLine ( 0, this->height+3, this->width, this->height+3 );

  Q_UNUSED ( w );
  Q_UNUSED ( option );
}


/*
 * Destructor
 */
NodeEllipse::~NodeEllipse ( )
{
}
