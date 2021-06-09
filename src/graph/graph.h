/*
 * graph.h
 *
 * Declaration of the Graph class.
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


#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <QtCore>

#include "../parser/entitylist.h"
#include "node.h"
#include "edge.h"


typedef struct st_candidate
{
  unsigned int score;
  Edge *e;
  
} Candidate ;


class Graph
{
 public:
  Graph ( );
  Graph ( QList<Node *> & );
  virtual ~Graph ( );

  void initFromEntityList ( const EntityList * );

  inline quint32 getNextNodeId ( ) { return this->n_id_counter++; }

  void addNode ( Node * );
  inline Node *getNode ( QString &s ) const { return nodes.value(s); }
  inline QHashIterator<QString,Node *> iterateOverNodes ( ) { return QHashIterator<QString,Node *> ( nodes ); }
  inline void removeNode ( Node *n ) { nodes.remove(n->id); }

  inline void addEdge ( Edge *e ) { edges.append(e); }
  inline QListIterator<Edge *> iterateOverEdges ( ) { return QListIterator<Edge *> ( edges ); }
  inline void removeEdge ( Edge *e ) { qlist_remove(edges,e); }

  void feedListWithActiveNodes ( QList<Node *> & ) const; /* feeds a list with all the active nodes of a graph */

  void assignSccIDs ( );          /* assign to each (active) node its Stronly Connected Component ID */
  void assignSubgraphIDs ( );     /* assign to each (active) node its subgraph ID */
  QPair<quint32, quint32> assignGridCoordinates ( ); /* assign to each (active) node its grid coordinates */

  void clear ( );

  void reverseUpwardEdges ( );    /* reverse the edges that are upward oriented */
  void virtualizeLongEdges ( );   /* add virtual nodes so that for each edge : src.grid_y == dest.grid_y+1 */
  void unVirtualizeLongEdges ( ); /* remove the previously created virtual nodes */
  void unReverseUpwardEdges ( );  /* un-reverse the previously reversed edges */

 private:  
  void resetNodeCounter ( ) { this->n_id_counter = 0; } /* reset the node counter */


 public:
  QHash<QString,Node *> nodes;  /* nodes of the graph */
  QList<Edge *> edges;          /* edges of the graph */

 private:
  quint32 n_id_counter;

} ;


#endif
