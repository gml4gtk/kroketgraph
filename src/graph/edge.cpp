/*
 * edge.cpp
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
#include "edge.h"
#include "graph.h"


static unsigned int counter = 0;


/*
 * Constructor
 */
Edge::Edge ( Node *src,
             Node *dest )
{
  this->src = src;
  this->dest = dest;

  this->id.setNum ( counter++ );
  this->id.append ( "__edge" );

  this->reversed = false;
  this->virtualized = false;
  this->storeKId ( );
}


/*
 * Initialization of an edge using an Entity object.
 */
void
Edge::initFromEntity ( const Entity *e )
{
  const char *attr_v;

  /* load ID */
  attr_v = e->getValueOfAttribute ( ATTR_EDGE_ID );

  if ( attr_v != NULL )
    this->id = attr_v;


  /* src and dest nodes are initialized externally */
}


/*
 * Reverse an edge
 */
void
Edge::reverse ( )
{
  Node *n;

  this->reversed = !this->reversed;

  this->src->removeChild ( this->dest );
  this->dest->addChild ( this->src );

  n = this->src;
  this->src = this->dest;
  this->dest = n;
}


/*
 * Virtualize an edge
 * (add virtual nodes between src and dest nodes)
 */
void
Edge::virtualize ( Graph *g )
{
  Edge *last = this;
  Edge *e;
  Node *virt;

  while ( (last->dest->grid_y - last->src->grid_y) > 1 )
    {
      virt = new Node ( g, true );
      virt->grid_y = last->src->grid_y + 1;
      g->addNode ( virt );

      e = new Edge ( virt, last->dest );

      last->src->removeChild ( last->dest );

      last->src->addChild ( virt );
      virt->addChild ( last->dest );

      last = e;
      this->virtualEdges.append ( e );
    }

  if ( this->virtualEdges.size() > 0 )
    this->virtualized = true;
  else
    {
      this->virtualized = false;
      this->virtualPoints.clear ( );
    }
}


/*
 * Un-Virtualize an edge
 * (remove virtual nodes between src and dest nodes)
 */
void
Edge::unVirtualize ( Graph *g )
{
  Node *first = NULL;
  Node *last;
  Edge *e;

  this->virtualized = false;
  this->virtualPoints.clear ( );

  last = this->dest;
  this->src->removeChild ( last );

  while ( this->virtualEdges.size() != 0 )
    {
      e = this->virtualEdges.takeFirst ( );

      if ( this->reversed ) /* get the virtual control points ordered according to the edge's direction */
        this->virtualPoints.insert ( -e->src->grid_y, e->src->grid_x );
      else
        this->virtualPoints.insert ( e->src->grid_y, e->src->grid_x );

      last = e->dest;           /* e->dest might be the real node we are trying to reach */
      last->removeParent ( e->src );

      if ( first == NULL )
        first = e->src;

      g->removeNode ( e->src ); /* e->src is the virtual node we created */
      delete e->src;

      delete e;
    }

  this->src->unlinkChild ( first );
  last->addParent ( this->src );
  this->dest = last;
}


/*
 * Destructor
 */
Edge::~Edge ( )
{
}
