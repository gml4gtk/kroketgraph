/*
 * layering-lazy.cpp
 *
 * Implementation of the LayeringLazy class / lazy layering algorithm.
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
#include "layering-lazy.h"


/*
 * The lazy layering algorithm
 *
 * This layering algorithm moves nodes that are at the same level
 * as their parents to a deeper layer.
 *
 */


#define DEBUG_LAYERING_LEVEL 0


/*
 * Internal structure used to store a conflict.
 */
typedef struct st_conflict
{
  Node *n;                /* parent node which has children at the same level */
  QList<Node *> cnodes;   /* conflicting children */
  int nb;                 /* number of conflicting children */

} Conflict ;



/*
 * Helper function which tells if a node is the head of a conflict in a conflicts list.
 */
static inline bool
contains_conflict_where_parent_is ( const QList<Conflict *> conflicts,
                                    const Node *node )
{
  bool ret = false;

  foreach ( Conflict *c, conflicts )
    {
      if ( c->n == node )
        {
          ret = true;
          break;
        }
    }

  return ret;
}


/*
 * Move a node to the sublayer.
 * If the node is the head of a conflict, then its conflicting children are moved too.
 */
static void
move_node_in_sublayer ( const unsigned int l_idx,
                        QList<QList<Node *> > &layers,
                        Node *node,
                        QMap<int, Conflict *> &conflicts,
                        QList<Conflict *> candidates )
{
  qlist_remove ( layers[l_idx], node );
  node->grid_y = l_idx + 1;
  layers[l_idx+1].append ( node );

#if DEBUG_LAYERING_LEVEL >= 2
  std::cout << "moving node \'" << qPrintable(node->id) << "\' to layer " << (l_idx+1) << std::endl;
#endif

  foreach ( Conflict *c, conflicts ) /* search the node in the conflicts, so we can move its children */
    {
      if ( c->n == node )
        {
#if DEBUG_LAYERING_LEVEL >= 2
          std::cout << "found one conflict related to \'" << qPrintable(node->id) << "\'\n";
#endif

          foreach ( Node *child, c->cnodes )
            {
              if ( ( child->grid_y != (l_idx+1) ) &&
                   ( !contains_conflict_where_parent_is(candidates,child) ) )
                move_node_in_sublayer ( l_idx, layers, child, conflicts, candidates );
              else
                {
#if DEBUG_LAYERING_LEVEL >= 2
                  std::cout << "NOT moving subnode \'" << qPrintable(child->id) << "\' to layer " << (l_idx+1) << std::endl;
                  std::cout << "test layer said " << ( child->grid_y != (l_idx+1) ) << " and contains said " << ( !contains_conflict_where_parent_is(candidates,child) ) << std::endl;
#endif
                }
            }

          break;
        }
    }
}


/*
 * Return the first conflict of a not empty conflicts map.
 */
static inline Conflict *
get_first_conflict ( QMap<int, Conflict *> &conflicts )
{
  QMapIterator<int, Conflict *> iter ( conflicts );
  Conflict *ret = NULL;

  if ( iter.hasNext() )
    {
      iter.next ( );
      ret = iter.value ( );
    }

  return ret;
}


/*
 * Apply the lazy layering algorithm to a list of layers.
 */
void
LayeringLazy::applyToLayers ( QList<QList<Node *> > &layers )
{
  bool moved;
  int i;
  int valid = 0;

  do
    {
      moved = false;

      for ( i=valid; i<layers.size(); ++i )
        {
          QMap<int, Conflict *> conflicts;
          Conflict *conflict;
          QList<Conflict *> candidates;
          int nb_conflicts;

#if DEBUG_LAYERING_LEVEL >= 1
          std::cout << "\n --- Layer " << i << " (" << layers[i].size() << " nodes) ---\n";
#endif

          /* for each node of the layer, look for children which are in the same layer */
          foreach ( Node *n, layers[i] )
            {
              conflict = new Conflict;
              conflict->n = n;
              nb_conflicts = 0;

              foreach ( Node *child, n->children )
                {
                  if ( child->grid_y == (unsigned int) i )
                    {
                      conflict->cnodes.append ( child );
                      --nb_conflicts; /* we decrement so the keys in the conflicts map have descendant order */
#if DEBUG_LAYERING_LEVEL >= 2
                      std::cout << "conflict found between \'" << qPrintable(n->id) << "\' and \'" << qPrintable(child->id) << "\'\n";
#endif
                    }
                }

              if ( nb_conflicts == 0 )
                delete conflict;
              else
                {
                  conflict->nb = nb_conflicts;
                  conflicts.insertMulti ( nb_conflicts, conflict );
                }
            }

          /* if there is any conflict, we must move some nodes down */
          if ( conflicts.size() != 0 )
            {
              if ( i != 0 )
                {
                  QList<Node *> valids;

                  /* only the nodes with a parent in the upper-layer interest us */
                  foreach ( Node *parent, layers[i-1] )
                    valids << parent->children;

                  foreach ( Conflict *c, conflicts.values(get_first_conflict(conflicts)->nb) ) /* best conflicts which are valid */
                    {
                      if ( valids.contains(c->n) )
                        candidates.append ( c );
                    }
                }
              else
                {
                  candidates.append ( get_first_conflict(conflicts) );
#if DEBUG_LAYERING_LEVEL >= 1
                  std::cout << "no best choice (layer 0)" << std::endl;
#endif
                }

              /* default choice if we could not find a satisfying candidate */
              if ( candidates.size() == 0 )
                {
#if DEBUG_LAYERING_LEVEL >= 1
                  std::cout << "no best choice (" << conflicts.size() << " conflicts)" << std::endl;
                  std::cout << "will resolve conflict of node \'" << qPrintable(get_first_conflict(conflicts)->n->id) << "\'\n";
#endif
                  candidates.append ( get_first_conflict(conflicts) );
                }
#if DEBUG_LAYERING_LEVEL >= 1
              else
                std::cout << "found " << candidates.size() << " interesting conflicts" << std::endl;
#endif

              /* allocate a new layer if needed */
              if ( (i+1) >= layers.size() )
                layers.append ( QList<Node *> ( ) );

              /* move the nodes down */
              if ( candidates.size() != 0 )
                {
                  Conflict *c = candidates.at ( 0 );
                  foreach ( Node *n, c->cnodes )
                    move_node_in_sublayer ( i, layers, n, conflicts, candidates );
                }

              /* free allocated Conflict structures */
              QMapIterator<int, Conflict *> iter ( conflicts );

              while ( iter.hasNext() )
                {
                  iter.next ( );
                  delete iter.value();
                }

              /* restart from top */
              moved = true;
              break;
            }
          else
            {
              valid = i+1;
#if DEBUG_LAYERING_LEVEL >= 1
              std::cout << "no more conflicts in layer " << i << std::endl;
#endif
            }
        }
    }
  while ( moved ) ;
}


/*
 * Remove a layer from the layers list.
 */
static void
removeLayer ( QList<QList<Node *> > &layers,
              const int idx )
{
  int i;
  QList<Node *> l;

  layers.removeAt ( idx );

  for ( i=idx; i<layers.size(); ++i )
    {
      l = layers.at ( i );

      foreach ( Node *n, l )
        --n->grid_y;
    }
}


/*
 * Layer a list of nodes using the lazy layering algorithm.
 */
void
LayeringLazy::applyToNodes ( QList<Node *> &nodes )
{
  QList<QList<Node *> > layers;
  int i;

  /* build layers list */
  layers.append ( QList<Node *> ( ) );

  foreach ( Node *n, nodes )
    {
      while ( (int)n->grid_y >= layers.size() )
        layers.append ( QList<Node *> ( ) );

      layers[n->grid_y].append ( n );
    }

  /* layering */
  applyToLayers ( layers );

  /* correct invalid layers */
  for ( i=0; i<layers.size(); ++i )
    {
      if ( layers.at(i).size() == 0 )
        {
          removeLayer ( layers, i );
          --i;
        }
    }
}
