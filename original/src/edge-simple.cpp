/*
 * edge-simple.cpp
 *
 * Implementation of the EdgeSimple object's methods.
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

#include "abstractanchor.h"
#include "edge-simple.h"


/*
 * Constructor
 */
EdgeSimple::EdgeSimple ( AbstractNodeShape *src,
                         AbstractNodeShape *dest )
  : AbstractEdgeShape ( src, dest )
{
  this->pen.setWidth ( 1 );
}


/*
 * Initialize an edge using an Entity object.
 */
void
EdgeSimple::initFromEntity ( const Entity *e )
{
  AbstractEdgeShape::initFromEntity ( e );
}


/*
 * Clone an edge shape.
 */
AbstractEdgeShape *
EdgeSimple::clone ( AbstractNodeShape *src_ns,
                    AbstractNodeShape *dest_ns )
{
  AbstractEdgeShape *ret;

  ret = new EdgeSimple ( src_ns, dest_ns );

  ret->srcAnchor = this->srcAnchor->clone ( ret, ANCHOR_SRC );
  src_ns->registerAnchor ( ret->srcAnchor );

  ret->destAnchor = this->destAnchor->clone ( ret, ANCHOR_DEST );
  dest_ns->registerAnchor ( ret->destAnchor );

  ret->initFromShape ( this );
  ret->initHoverPen ( );

  return ret;
}


/*
 * Build the minimal rectangle which contains an edge simple.
 */
QRectF
EdgeSimple::boundingRect ( ) const
{
  return  this->pathD.boundingRect().adjusted ( -2, -2, 2, 2 ) | childrenBoundingRect();
}


/*
 * Build a path following the shape of an item.
 */
QPainterPath
EdgeSimple::shape ( ) const
{
  return this->pathD;
}


/*
 * Paint an item.
 */
void
EdgeSimple::paint ( QPainter *painter,
                    const QStyleOptionGraphicsItem *option,
                    QWidget *w )
{
  painter->setBrush ( this->brush );

  if ( this->hovered )
    {
      painter->setPen ( this->hoverPen );
      painter->strokePath ( this->path, this->hoverPen );

      this->srcAnchor->draw ( painter );
      this->destAnchor->draw ( painter );
    }

  painter->setPen ( this->pen );
  painter->strokePath ( this->path, this->pen );

  this->srcAnchor->draw ( painter );
  this->destAnchor->draw ( painter );

  // paintBoundingRect ( painter );

  Q_UNUSED ( w );
  Q_UNUSED ( option );
}


/*
 * Destructor
 */
EdgeSimple::~EdgeSimple ( )
{
}
