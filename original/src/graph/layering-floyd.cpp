/*
 * layering-floyd.cpp
 *
 * Implementation of the LayeringFloyd class / floyd layering algorithm.
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

#include <iostream>
#include "layering-floyd.h"


/*
 * The floyd layering algorithm
 *
 * Using the Floyd shortest path algorithm, this layering
 * algorithm assign each node a grid Y coordinate based
 * on the distance between a node and the entry points
 * of the graph.
 *
 */


#define LIMIT INT_MAX


/*
 * Layout a list of nodes using the floyd algorithm.
 */
void
LayeringFloyd::applyToNodes ( QList<Node *> &nodes )
{
  quint32 **m;
  quint32 *row;
  quint32 i, j, k;
  Node *n, *child, *entry;
  QList<Node *> entries;

  const quint32 N = nodes.size ( );

  if ( N == 0 )
    return;

  entry = nodes[0];

  /* allocate matrix */
  m = new quint32 * [ N ];

  for ( i=0; i!=N; ++i )
    {
      m[i] = new quint32 [ N ];
      row = m[i];

      for ( j=0; j!=N; ++j )
        row[j] = LIMIT;

      nodes[i]->tag = i;
    }

  /* initialize matrix with adjacencies */
  QListIterator<Node *> iter ( nodes );

  while ( iter.hasNext() )
    {
      n = iter.next ( );
      n->grid_y = 0;
      row = m [ n->tag ];

      if ( entry->n_id > n->n_id ) /* we take the first node that was added to the graph */
        entry = n;                 /* it will be used as the default entry point */

      if ( !n->hasParents() ) /* save the entries of the graph */
        entries.append ( n );

      foreach ( child, n->children )
        row[child->tag] = 1;

      row[n->tag] = 0;
    }

  if ( entries.size() == 0 )
    entries.append ( entry );

  /* compute matrix (floyd algorithm) */
  quint32 tmp;

  for ( i=0; i!=N; ++i )
    {
      for ( j=0; j!=N; ++j )
        {
          row = m[j];

          for ( k=0; k!=N; ++k )
            {
              tmp = row[i] + m[i][k];

              if ( row[k] > tmp )
                row[k] = tmp;
            }
        }
    }


  /* affect a layer id to each node according to its distance from the entry points */
  iter.toFront ( );

  QListIterator<Node *> entries_iter ( entries );
  quint32 distance;

  while ( iter.hasNext() )
    {
      n = iter.next ( );

      distance = 0;
      entries_iter.toFront ( );

      while ( entries_iter.hasNext() )
        {
          entry = entries_iter.next ( );

          if ( m[entry->tag][n->tag] != LIMIT )
            {
              if ( m[entry->tag][n->tag] > distance )
                distance = m[entry->tag][n->tag];
            }
        }

      n->grid_y = distance;
    }

  /* we move the entry points just above their nearest child */
  entries_iter.toFront ( );

  while ( entries_iter.hasNext() )
    {
      entry = entries_iter.next ( );

      iter = QListIterator<Node *> ( entry->children );
      n = NULL;
      k = LIMIT;

      while ( iter.hasNext() )
        {
          child = iter.next ( );

          if ( child->grid_y == 0 )
            continue;

          if ( child->grid_y < k )
            {
              k = child->grid_y;
              n = child;
            }
        }

      if ( n != NULL )
        entry->grid_y = k - 1;
    }

  /* free memory */
  for ( i=0; i<N; ++i )
    delete m[i];

  delete m;
}
