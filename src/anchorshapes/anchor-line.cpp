/*
 * anchor-line.cpp
 *
 * Implementation of the AnchorLine object's methods.
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

#include "anchor-line.h"


/*
 * Constructor
 */
AnchorLine::AnchorLine ( AbstractEdgeShape *e,
                         AnchorType t )
  : AbstractAnchor ( e, t )
{
}


/*
 * Clone an anchor.
 */
AbstractAnchor *
AnchorLine::clone ( AbstractEdgeShape *e,
                    AnchorType t )
{
  AbstractAnchor *ret = new AnchorLine ( e, t );
  ret->initFromAnchor ( this );
  return ret;
}


/*
 * Update an anchor shape.
 */
void
AnchorLine::update ( )
{
}


/*
 * Draw an anchor shape.
 */
void
AnchorLine::draw ( QPainter *painter ) const
{
  Q_UNUSED ( painter );
}


/*
 * Destructor
 */
AnchorLine::~AnchorLine ( )
{
}
