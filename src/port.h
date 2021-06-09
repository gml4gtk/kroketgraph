/*
 * port.h
 *
 * Declaration of the Port class.
 * It provides the basic behaviour of ports.
 * A port manages attachments points or anchors for edges.
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

#ifndef __PORT_H__
#define __PORT_H__


#include <QtCore>
#include "abstractanchor.h"


class Port
{
 public:
  Port ( AbstractNodeShape *, AbstractAnchor * );
  ~Port ( );

  /*
   * Set the position of a port
   */
  void setPos ( const qreal, const qreal );

  /*
   * Update the position of the anchors held by a port.
   */
  void updateAnchors ( );

  /*
   * Add an anchor to a port.
   */
  void addAnchor ( AbstractAnchor * );

  /*
   * Callbacks for the actvation/deactivation of an anchor.
   */
  void anchorActivated ( AbstractAnchor * );
  void anchorDeactivated ( AbstractAnchor * );

  /*
   * Is a port active ?
   * A port is considered active when it contains at least one active anchor.
   */
  inline bool isActive ( ) const { return ( this->anchors.size() != 0 ); }

  /*
   * Compare ports in order to sort them.
   */
  static bool comparePorts ( const Port *p1, const Port *p2 )
  {
    if ( p1->attr == p2->attr )
      return ( p1->anchors.at(0)->getControlX() < p2->anchors.at(0)->getControlX() );
    return ( p1->attr < p2->attr );
  }


  AbstractNodeShape *node;
  QList<AbstractAnchor *> anchors;          /* active anchors */
  QList<AbstractAnchor *> inactiveAnchors;  /* inactive anchors */
  QPointF pos;                              /* position relative to the node */
  QString attr;
  QGraphicsSimpleTextItem *text;

} ;


#endif
