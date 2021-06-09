/*
 * abstractanchor.h
 *
 * Declaration of the AbstractAnchor class.
 * It provides the basic behaviour of anchors.
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

#ifndef __ABSTRACTANCHOR_H__
#define __ABSTRACTANCHOR_H__


#include "abstractnodeshape.h"
#include "abstractedgeshape.h"


typedef enum { ANCHOR_SRC, ANCHOR_DEST } AnchorType ;


class Port;

class AbstractAnchor
{
 public:
  AbstractAnchor ( AbstractEdgeShape *, AnchorType );
  virtual ~AbstractAnchor ( );

  /*
   * Clone an anchor.
   */
  virtual AbstractAnchor *clone ( AbstractEdgeShape *, AnchorType ) = 0;

  /*
   * Initialize an anchor using an other anchor.
   */
  void initFromAnchor ( AbstractAnchor * );

  /*
   * Check the type of the anchor.
   */
  inline bool isSrc ( ) const { return (this->type == ANCHOR_SRC); }
  inline bool isDest ( ) const { return (this->type == ANCHOR_DEST); }

  /*
   * Assign a region name to an anchor.
   */
  inline void setRegion ( QString &r ) { this->region = r; }

  /*
   * Assign an attribute to an anchor.
   */
  inline void setAttr ( QString &a ) { this->attr = a; }

  /*
   * Assign a text to an anchor.
   */
  inline void setText ( QString &t ) { this->text = t; }
  inline void setText ( const char *t ) { this->text = QString::fromAscii(t); }

  /*
   * Assign a port to an anchor.
   */
  inline void setPort ( Port *p ) { this->port = p; }

  /*
   * Assign an edge to an anchor.
   */
  inline void setEdge ( AbstractEdgeShape *e ) { this->edge = e; }

  /*
   * Assign a position to an anchor.
   */
  inline void setPos ( QPointF &pos ) { this->pos = pos; edge->computePath(); }
  inline void setPos ( const QPointF pos ) { this->pos = pos; edge->computePath(); }
  inline void setPos ( const qreal x, const qreal y ) { this->pos = QPointF(x,y); edge->computePath(); }

  /*
   * Request an anchor's edge the screen X coordinate of the last control point towards the anchor.
   */
  inline qreal getControlX ( ) const { return this->edge->getControlX(this); }

  /*
   * Activate/Deactivate an anchor
   */
  void setActive ( bool );
  inline bool isActive ( ) { return active; }

  /*
   * Drawing related functions
   */
  virtual void draw ( QPainter * ) const = 0;
  virtual void update ( ) = 0;

  /*
   * Compare two anchors. Return a boolean according to their grid X coord.
   */
  static bool compareAnchors ( const AbstractAnchor *a1, const AbstractAnchor *a2 )
    {
      //if ( a1->attr == a2->attr )
        return ( a1->getControlX() < a2->getControlX() );
        //else
        //return ( a1->attr < a2->attr );
    }


  QString region;
  QString attr;
  QString text;

  AnchorType type;
  AbstractEdgeShape *edge;

  Port *port;

  QPointF pos;
  qreal angle;
  bool active;

} ;


#endif
