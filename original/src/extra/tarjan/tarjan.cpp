/*
 * tarjan.cpp
 *
 * Implementation of the Tarjan algorithm.
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


#include "graph/tarjan.h"


/*
 * Depth First Search with numbering in order of recursive calls completion.
 */
quint32
depthFirstSearchNumbering ( Node *start,
                            QList<Node *> &visited,
                            quint32 id )
{
  if ( !visited.contains(start) )
    {
      visited.append ( start );

      foreach ( Node *child, start->children )
        id = depthFirstSearchNumbering ( child, visited, id );

      start->tag = id++;
    }

  return id;
}


/*
 * Depth First Search which builds lists of Strongly Connected Components.
 * The list contains pointers to the nodes of the original graph.
 */
void
depthFirstSearchSCCMaker ( Node *start,
                           QList<Node *> &sortedQueue,
                           QList<Node *> &visited,
                           const Graph *originalGraph,
                           QList<Node *> *SCC )
{
  if ( !visited.contains(start) )
    {
      visited.append ( start );

      foreach ( Node *child, start->children )
        depthFirstSearchSCCMaker ( child, sortedQueue, visited, originalGraph, SCC );

      SCC->append ( originalGraph->getNode(start->id) );
      qlist_remove ( sortedQueue, start );
    }
}


/*
 * Return true if n1 if greater than n2 (descendant order)
 */
static bool
compareNodesTags ( const Node *n1,
                   const Node *n2 )
{
  return ( n1->tag > n2->tag );
}


/*
 * Build a list of the SCCs of a graph.
 */
QList<QList<Node *> * > *
Tarjan::buildSccList ( const Graph *originalGraph )
{
  Graph *graph;
  QList<Node *> visited;
  QList<Node *> sortedQueue;
  QList<QList<Node *> * > *sccList = new QList<QList<Node *> * > ( );
  quint32 id = 1;

  /* Make a copy of the graph */
  originalGraph->feedListWithActiveNodes ( visited );
  graph = new Graph ( visited );
  visited.clear ( );
  
  /* DFS with numbering in order of recursive calls completion */
  foreach ( Node *n, graph->nodes )
    id = depthFirstSearchNumbering ( n, visited, id );

  /* order the list of visited nodes according to the nodes's tags */
  qSort ( visited.begin(), visited.end(), compareNodesTags );
  sortedQueue = visited;

  /* graph reversal */
  foreach ( Edge *e, graph->edges )
    e->reverse ( );

  /* DFS starting from the node with the higher tag */
  visited.clear ( );

  while ( !sortedQueue.empty() )
    {
      QList<Node *> *s = new QList<Node *> ( );
      depthFirstSearchSCCMaker ( sortedQueue.front(), sortedQueue, visited, originalGraph, s );
      sccList->append ( s );
    }

  /* graph deletion */
  delete graph;

  return sccList;
}


/*
 * Assign the SCC ID to each node of a graph.
 */
void
Tarjan::assignSccIDs ( Graph *g )
{
  int id;
  QList<QList<Node *> * > *sccList = buildSccList ( g );
  QList<Node *> *l;

  /* assignment */
  for ( id=1; id<sccList->size(); ++id )
    {
      l = sccList->at ( id );

      foreach ( Node *n, *l )
        n->scc_id = id;
    }

  /* clean up */
  qDeleteAll ( *sccList );
  delete ( sccList );
}
