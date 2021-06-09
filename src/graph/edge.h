/*
 * edge.h
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


#ifndef __EDGE_H__
#define __EDGE_H__


#include <QtCore>

#include "node.h"


class Graph;

class Edge
{
 public:
  Edge ( Node *src=NULL, Node *dest=NULL );
  virtual ~Edge ( );

  void initFromEntity ( const Entity * );

  inline void setSrc ( Node *n ) { src = n; storeKId(); };
  inline void setDest ( Node *n ) { dest = n; storeKId(); }

  void reverse ( );
  void virtualize ( Graph * );
  void unVirtualize ( Graph * );

  inline bool isUpward ( ) const { return ( src->grid_y > dest->grid_y ); }
  inline bool isDownward ( ) const { return ( src->grid_y < dest->grid_y ); }

  inline bool isActive ( ) const { return ( src->active && dest->active ); }
  inline bool isLoop ( )  const { return ( src == dest ); }

  /*
   * The KId is used for debugging purposes mainly
   */
  static inline QString getKId ( Node *src, Node *dest )
    {
      QString ret = src->id;
      ret.append ( "-" );
      ret.append ( dest->id );
      return ret;
    }

  inline void storeKId ( )
    {
      if ( ( src != NULL ) && ( dest != NULL ) )
        this->k_id = getKId ( src, dest );
    }


 public:
  QString id;                    /* text ID    */
  QString k_id;                  /* text ID built with the src and dest nodes IDs */

  Node *src;                     /* source Node */
  Node *dest;                    /* destination Node */

  bool reversed;                 /* Has the edge been reversed ? */

  bool virtualized;              /* Has the edge been virtualized ? */
  QList<Edge *> virtualEdges;
  QMap<int, quint32> virtualPoints;

} ;


#endif
