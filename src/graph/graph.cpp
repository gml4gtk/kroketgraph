/*
 * graph.cpp
 *
 * Implementation of the Graph object's methods.
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

#include "../parser/defs.h"
#include "layering-lazy.h"
#include "layering-floyd.h"
#include "ordering-wmedian.h"
#include "placing-genetic.h"
#include "graph.h"


/*
 * Constructor
 */
Graph::Graph ( )
{
  this->resetNodeCounter ( );
}

Graph::Graph ( QList<Node *> &nlist )
{
  Node *ncopy;
  Edge *e;

  this->resetNodeCounter ( );

  /* nodes creation */
  foreach ( Node *n, nlist )
    {
      ncopy = new Node ( this );
      ncopy->id = n->id;
      this->addNode ( ncopy );
    }

  /* edges creation */
  foreach ( Node *n, nlist )
    {
      foreach ( Node *child, n->children )
        {
          if ( nlist.contains(child) )
            {
              ncopy = this->getNode ( n->id );
              ncopy->addChild ( this->getNode(child->id) );
              e = new Edge ( ncopy, this->getNode(child->id) );
              this->addEdge ( e );
            }
        }
    }
}


/*
 * Initialization of a graph using an entity list.
 * Groups are not handled.
 */
void
Graph::initFromEntityList ( const EntityList *el )
{
  EntityList::iterator iter;
  Entity *e;
  Node *node;
  Edge *edge;
  Node *src, *dest;

  this->resetNodeCounter ( );

  /* nodes creation */
  iter = el->iterate ( ENTITY_NODE );

  while ( iter.hasNext() )
    {
      e = iter.next ( );

      node = new Node ( this );
      node->initFromEntity ( e );
      this->addNode ( node );
    }

  /* edges creation */
  iter = el->iterate ( ENTITY_EDGE );

  while ( iter.hasNext() )
    {
      e = iter.next ( );

      src = this->nodes.value ( Node::extractNodeName( e->getValueOfAttribute("src_port") ) );

      if ( src == NULL )
        continue;

      dest = this->nodes.value ( Node::extractNodeName( e->getValueOfAttribute("dest_port") ) );

      if ( dest == NULL )
        continue;

      if ( src != dest ) /* loops are ignored */
        {
          edge = new Edge ( );
          edge->setSrc ( src );
          edge->setDest ( dest );
          this->addEdge ( edge );

          src->addChild ( dest );
        }
    }
}


/*
 * Add a node to the graph.
 */
void
Graph::addNode ( Node *n )
{
  if ( this->nodes.contains(n->id) )
    std:: cerr << "warning: node id \'" << qPrintable(n->id) << "\' is used twice.\n";

  nodes.insert ( n->id, n );
}


/*
 * Reverse the edges that go from a node which is deeper than the destination.
 */
void
Graph::reverseUpwardEdges ( )
{
  foreach ( Edge *e, this->edges )
    if ( ( e->src->grid_y > e->dest->grid_y ) && ( e->isActive() ) )
      e->reverse ( );
}


/*
 * Virtualize the edges that traverse more than one layer.
 */
void
Graph::virtualizeLongEdges ( )
{
  foreach ( Edge *e, this->edges )
    if ( e->isActive() )
      e->virtualize ( this );
}


/*
 * Virtualize the edges that traverse more than one layer.
 */
void
Graph::unVirtualizeLongEdges ( )
{
  foreach ( Edge *e, this->edges )
    if ( ( e->virtualized ) && ( e->isActive() ) )
      e->unVirtualize ( this );
}


/*
 * Reverse the edges that go from a node which is deeper than the destination.
 */
void
Graph::unReverseUpwardEdges ( )
{
  foreach ( Edge *e, this->edges )
    if ( ( e->reversed ) && ( e->isActive() ) )
      e->reverse ( );
}


/*
 * Feed a list with the active nodes of a graph.
 */
void
Graph::feedListWithActiveNodes ( QList<Node *> &l ) const
{
  l.clear ( );

  for ( QHash<QString,Node *>::const_iterator iter=this->nodes.constBegin(); iter!=this->nodes.constEnd(); ++iter )
    if ( (*iter)->isActive() )
      l.append ( *iter );
}


/*
 * Assign to each (active) node its subgraph ID
 * recursiveGrouping() finds the subgraph a set of nodes belong to.
 * recursiveTagging() assigns a subgraph ID to every node it processes.
 */
static quint32
recursiveGrouping ( Node *n )
{
  /* this function determines if a set of nodes belong to a subgraph */

  quint32 id = 0;

  if ( n->subgraph_id != 0 )
    return n->subgraph_id;

  /* the node is tagged if visited */
  if ( n->tag != 0 )
    return n->subgraph_id;

  n->tag = 1;

  /* explore children */
  foreach ( Node *child, n->children )
    {
      id = recursiveGrouping ( child );

      if ( id != 0 )
        break;
    }

  /* explore parents */
  if ( id != 0 )
    {
      foreach ( Node *parent, n->parents )
        {
          id = recursiveGrouping ( parent );

          if ( id != 0 )
            break;
        }
    }

  return id;
}

static void
recursiveTagging ( Node *n,
                   quint32 id )
{
  /* this function assigns a subgraph_id */

  if ( n->subgraph_id == id )
    return;

  n->subgraph_id = id;

  foreach ( Node *child, n->children )
    recursiveTagging ( child, id );

  foreach ( Node *parent, n->parents )
    recursiveTagging ( parent, id );
}

void
Graph::assignSubgraphIDs ( )
{
  Node *n;
  Node *entry;
  QList<Node *> entries;
  QList<Node *> nodesList;
  this->feedListWithActiveNodes ( nodesList );

  /* initialization */
  QListIterator<Node *> iter ( nodesList );
  entry = nodesList[0];

  while ( iter.hasNext() )
    {
      n = iter.next ( );
      n->subgraph_id = 0;
      n->tag = 0;

      if ( entry->n_id > n->n_id ) /* we take the first node that was added to the graph */
        entry = n;                 /* it will be used as the default entry point */

      if ( !n->hasParents() ) /* save the entries of the graph */
        entries.append ( n );
    }

  if ( entries.size() == 0 )
    entries.append ( entry );

  /* assignment */
  quint32 id;
  quint32 nextid = 1;

  foreach ( n, entries )
    {
      id = recursiveGrouping ( n );

      if ( id == 0 )
        recursiveTagging ( n, nextid++ );
      else
        recursiveTagging ( n, id );
    }
}


/*
 * Assign to each (active) node its grid coordinates.
 * The maximal X grid coord and the maximal Y grid coord are returned.
 */
QPair<quint32, quint32>
Graph::assignGridCoordinates ( )
{
  QPair<quint32, quint32> gridMax ( 0, 0 );
  QList<Node *> nodes_list;
  this->feedListWithActiveNodes ( nodes_list );

  LayeringFloyd::applyToNodes ( nodes_list );
  LayeringLazy::applyToNodes ( nodes_list );

  this->reverseUpwardEdges ( );
  this->virtualizeLongEdges ( );

  /*  * sanity check * 
  foreach ( Node *n, nodes_list )
    {
      foreach ( Node *child, n->children )
        {
          if ( 1+n->grid_y != child->grid_y )
            {
              std::cout << "inconsistent state with nodes \'" << qPrintable(n->id) << "\' and \'" << qPrintable(child->id) << "\'\n";
              std::cout << "n has layer=" << n->grid_y << " and child has layer=" << child->grid_y << "\n";
              exit ( -1 );
            }
        }
    }
  */

  this->feedListWithActiveNodes ( nodes_list ); /* update the nodes list because of the virtualization */

  OrderingWMedian::applyToNodes ( nodes_list );
  PlacingGenetic::applyToNodes ( nodes_list );

  foreach ( Node *n, nodes_list )
    {
      if ( n->grid_x > gridMax.first )
        gridMax.first = n->grid_x;

      if ( n->grid_y > gridMax.second )
        gridMax.second = n->grid_y;
    }

  this->unVirtualizeLongEdges ( );
  this->unReverseUpwardEdges ( );

  return gridMax;
}


/*
 * Clear a graph.
 */
void
Graph::clear ( )
{
  foreach ( Node *n, this->nodes )
    delete n;

  this->nodes.clear ( );

  foreach ( Edge *e, this->edges )
    delete e;

  this->edges.clear ( );
}


/*
 * Destructor
 */
Graph::~Graph ( )
{
  this->clear ( );
}
