/*
 * abstractanchor.cpp
 *
 * Implementation of the AbstractAnchor object's methods.
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


#include "port.h"
#include "abstractanchor.h"


/*
 * Constructor
 */
AbstractAnchor::AbstractAnchor ( AbstractEdgeShape *e,
                                 AnchorType t )
  : type(t), edge(e)
{
  active = true;
}


/*
 * Initialize an anchor using an other anchor.
 */
void
AbstractAnchor::initFromAnchor ( AbstractAnchor *a )
{
  this->setRegion ( a->region );
  this->setAttr ( a->attr );
  this->setText ( a->text );
}


/*
 * Activate/Deactivate an anchor
 */
void
AbstractAnchor::setActive ( bool active )
{
  this->active = active;

  if ( active )
    this->port->anchorActivated ( this );
  else
    this->port->anchorDeactivated ( this );
}


/*
 * Destructor
 */
AbstractAnchor::~AbstractAnchor ( )
{
}
