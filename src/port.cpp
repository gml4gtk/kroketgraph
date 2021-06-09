/*
 * port.cpp
 *
 * Implementation of the Port object's methods.
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


/*
 * Constructor
 */
Port::Port ( AbstractNodeShape *owner,
             AbstractAnchor *aa )
  : node(owner)
{
  this->addAnchor ( aa );

  if ( aa->text.size() > 0 )
    {
      QString str = aa->text.replace ( "\\n", "\n" );
      str = str.replace ( "\\t", "    " );
      this->text = new QGraphicsSimpleTextItem ( str, owner );
      this->text->setZValue ( 4 );
    }
  else
    this->text = NULL;

  this->attr = aa->attr;
}


/*
 * Set the position of a port
 */
void
Port::setPos ( const qreal x, 
               const qreal y )
{
  this->pos = QPointF ( x, y );

  if ( this->text!= NULL )
    this->text->setPos ( this->pos + QPointF(4,0) );
}


/*
 * Add an anchor to a port.
 */
void
Port::addAnchor ( AbstractAnchor *aa )
{
  if ( aa->isActive() )
    this->anchors.append ( aa );
  else
    this->inactiveAnchors.append ( aa );

  aa->setPort ( this );
}


/*
 * Update the position of the anchors held by a port.
 * The inactive edges are distributed along the area occupied by the port.
 * The active edges are set to the center of the area occupied by the port.
 */
void
Port::updateAnchors ( )
{
  qreal pad;
  const QPointF p = node->pos() + this->pos;

  qSort ( this->anchors.begin(), this->anchors.end(), AbstractAnchor::compareAnchors );

  pad = - ( 5 * (this->anchors.size()-1) );

  foreach ( AbstractAnchor *aa, this->anchors )
    {
      if ( ((Edge *)aa->edge)->isActive() )
        aa->setPos ( p );
      else
        {
          aa->setPos ( p.x()+pad, p.y() );
          pad += 10;
        }
    }
}


/*
 * Callbacks for the actvation/deactivation of an anchor.
 */
void
Port::anchorActivated ( AbstractAnchor *a )
{
  if ( !this->anchors.contains(a) )
    this->anchors.append ( a );

  if ( this->inactiveAnchors.contains(a) )
    qlist_remove ( this->inactiveAnchors, a );
}

void
Port::anchorDeactivated ( AbstractAnchor *a )
{
  if ( this->anchors.contains(a) )
    qlist_remove ( this->anchors, a );

  if ( !this->inactiveAnchors.contains(a) )
    this->inactiveAnchors.append ( a );
}


/*
 * Destructor
 */
Port::~Port ( )
{
}
