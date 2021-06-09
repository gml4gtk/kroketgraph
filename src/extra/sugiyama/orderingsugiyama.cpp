/*
 * ordering-sugiyama.cpp
 *
 * Implementation of the OrderingSugiyama class / sugiyama ordering algorithm.
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
#include "ordering-sugiyama.h"


/*
 * The sugiyama ordering algorithm
 *
 * This algorithm assigns each node a grid X coordinate.
 * In order to reduce edge crossings, we sort the nodes within
 * each layer using three different calculations of a
 * coefficient called barycenter coeffcient.
 *
 */


#define NB_ITER_STEP_1  4
#define NB_ITER_STEP_2  2


/*
 * Calculate the barycenter coefficient for a parent.
 */
static inline void
assign_parental_barycenter_coef ( Node *n )
{
  double coef = 0.00;

  if ( n->children.size() != 0 )
    {
      foreach ( Node *child, n->children )
        {
          // std::cout << "child \'" << qPrintable(child->id) << "\' of \'" << qPrintable(n->id) << "\' has grid_x=" << child->grid_x << "\n";
          coef += child->grid_x;
        }

      n->coef = (coef / n->children.size());
    }
  else
    n->coef = n->grid_x;

  //  std::cout << "node " << qPrintable(n->id) << " has parental coef=" << n->coef << "\n";
}


/*
 * Calculate the barycenter coefficient for a child.
 */
static inline void
assign_filial_barycenter_coef ( Node *n )
{
  double coef = 0.00;

  if ( n->parents.size() != 0 )
    {
      foreach ( Node *parent, n->parents )
        coef += parent->grid_x;

      n->coef = (coef / n->parents.size());
    }
  else
    n->coef = n->grid_x;

  //assign_parental_barycenter_coef ( n ); /* to be checked */

  // std::cout << "node " << qPrintable(n->id) << " has filial coef=" << n->coef << "\n";
}


/*
 * Calculate the barycenter coefficient for a child.
 */
static inline void
assign_average_barycenter_coef ( Node *n )
{
  double coef = 0.00;

  foreach ( Node *parent, n->parents )
    coef += parent->grid_x;

  foreach ( Node *child, n->children )
    coef += child->grid_x;

  if ( ( n->parents.size() + n->children.size() ) != 0 )
    n->coef = ( coef / (n->parents.size()+n->children.size()) );
  else
    n->coef = n->grid_x;
}


/*
 * Compare two nodes' coefficient. This is used for sorting layer.
 */
static bool
compareCoef ( const Node *n1,
              const Node *n2 )
{
  return ( n1->coef < n2->coef );
}


/*
 * Sort the nodes in each layer (step 1 of sugiyama's algorithm)
 */
static inline void
sort_layers ( QList<QList<Node *> > &layers )
{
  unsigned int niter;
  int i, j;

  for ( niter=0; niter<NB_ITER_STEP_1; ++niter )
    {
      // std::cout << "--- SORT LAYERS (iter=" << niter << ")\n";

      /* sort the nodes of the layer[i] according to the nodes of the layer[i-1] */
      for ( i=layers.size()-1; i!=0; --i )
        {
          QList<Node *> &l = layers[i];

          /* calculate filial barycenter coefs */
          foreach ( Node *n, l )
            assign_filial_barycenter_coef ( n );

          /* sort */
          qStableSort ( l.begin(), l.end(), compareCoef );

          /* update indexes */
          for ( j=0; j<l.size(); ++j )
            l[j]->grid_x = j+1;
        }

      /* sort the nodes of the layer[i] according to the nodes of the layer[i+1] */
      for ( i=0; i<layers.size()-1; ++i )
        {
          QList<Node *> &l = layers[i];

          /* calculate parental barycenter coefs */
          foreach ( Node *n, l )
            assign_parental_barycenter_coef ( n );

          /* sort */
          qStableSort ( l.begin(), l.end(), compareCoef );

          /* update indexes */
          for ( j=0; j<l.size(); ++j )
            l[j]->grid_x = j+1;
        }

      /* sort the nodes of the layer[i] according to the nodes of the layer[i-1] and layer[i+1] */
      for ( i=0; i<layers.size(); ++i )
        {
          QList<Node *> &l = layers[i];

          /* calculate parental barycenter coefs */
          foreach ( Node *n, l )
            assign_average_barycenter_coef ( n );

          /* sort */
          qStableSort ( l.begin(), l.end(), compareCoef );

          /* update indexes */
          for ( j=0; j<l.size(); ++j )
            l[j]->grid_x = j+1;
        }
    }
}


/*
 * Apply the sugiyama ordering algorithm to a list of layers.
 */
void
OrderingSugiyama::applyToLayers ( QList<QList<Node *> > &layers )
{
  unsigned int niter;
  int i, j;
  bool moved;

  for ( niter=0; niter<NB_ITER_STEP_2; ++niter )
    {
      moved = false;

      sort_layers ( layers );

      /* swap the nodes which have the same parental barycenter coefficient */
      for ( i=0; i<layers.size()-1; ++i )
        {
          QList<Node *> &l = layers[i];

          /* calculate parental barycenter coefs */
          foreach ( Node *n, l )
            assign_parental_barycenter_coef ( n );

          /* swap */
          for ( j=0; j<l.size()-1; ++j )
            {
              if ( l[j]->coef == l[j+1]->coef )
                {
                  l.swap ( j, j+1 );
                  l[j]->grid_x = j+1;
                  l[j+1]->grid_x = j+2;
                  moved = true;
                }
            }

          if ( moved )
            sort_layers ( layers );
        }

      moved = false;

      /* swap the nodes which have the same filial barycenter coefficient */
      for ( i=layers.size()-1; i!=0; --i )
        {
          QList<Node *> &l = layers[i];

          /* calculate filial barycenter coefs */
          foreach ( Node *n, l )
            assign_filial_barycenter_coef ( n );

          /* swap */
          for ( j=0; j<l.size()-1; ++j )
            {
              if ( l[j]->coef == l[j+1]->coef )
                {
                  l.swap ( j, j+1 );
                  l[j]->grid_x = j+1;
                  l[j+1]->grid_x = j+2;
                  moved = true;
                }
            }

          if ( moved )
            sort_layers ( layers );
        }
    }
}

/*
 * Count the number of edge crossings between two nodes.
 * count_nb_crossings(a,b,s) != count_nb_crossings(b,a,s)
 * The function consider n1 is on the left side of n2.
 */
static inline int
count_nb_crossings ( const Node *n1,
                     const Node *n2,
                     QList<Node *> *upperLayer,
                     QList<Node *> *lowerLayer )
{
  int ret = 0;
  Node *p;

  if ( upperLayer != NULL )
    {
      foreach ( p, n1->parents )
        {
          const int idx = p->grid_x - 1;

          for ( int i=0; i<idx; ++i )
            if ( n2->parents.contains((*upperLayer)[i]) )
              ++ret;
        }
    }

  if ( lowerLayer != NULL )
    {
      foreach ( p, n1->children )
        {
          const int idx = p->grid_x - 1;

          for ( int i=0; i<idx; ++i )
            if ( n2->children.contains((*lowerLayer)[i]) )
              ++ret;
        }
    }

  return ret;
}


/*
 * Count the number of edge crossings for all layers.
 */
static int
count_nb_crossings ( QList<QList<Node *> > &layers )
{
  int i, j, k;
  int ret = 0;

  for ( i=0; i<layers.size()-1; ++i )
    {
      QList<Node *> &l = layers[i];
      QList<Node *> *l1 = &layers[i+1];

      for ( j=0; j<l.size()-1; ++j )
        for ( k=j+1; k<l.size(); ++k )
          ret += count_nb_crossings ( l[j], l[k], NULL, l1 );
    }

  return ret;
}


/*
 * Assign an X coordinate to each node in order discovery
 */
static void
assignInitialGridX ( Node *n,
                     QList<unsigned int> &posList )
{
  unsigned int x;

  if ( n->grid_x != 0 )
    return;

  x = posList[n->grid_y];
  n->grid_x = x;
  posList[n->grid_y] = x + 1;

  foreach ( Node *child, n->children )
    assignInitialGridX ( child, posList );
}


/*
 * Compare to nodes grid_x in order to sort a layer.
 */
static bool
compareGridX ( const Node *n1,
               const Node *n2 )
{
  return ( n1->grid_x < n2->grid_x );
}


/*
 * Order a list of nodes using the sugiyama algorithm.
 */
void
OrderingSugiyama::applyToNodes ( QList<Node *> &nodes )
{
  QList<QList<Node *> > layers;
  QList<unsigned int> posList;
  QList<Node *> entries;
  Node *entry;
  int i;

  if ( nodes.size() == 0 )
    return;

  entry = nodes[0];

  /* build layers list */
  layers.append ( QList<Node *> ( ) );
  posList.append ( 1 );

  foreach ( Node *n, nodes )
    {
      while ( (int)n->grid_y >= layers.size() )
        {
          layers.append ( QList<Node *> ( ) );
          posList.append ( 1 );
        }

      layers[n->grid_y].append ( n );
      n->grid_x = 0;

      if ( entry->n_id > n->n_id ) /* we take the first node that was added to the graph */
        entry = n;                 /* it will be used as the default entry point */

      if ( !n->hasParents() ) /* save the entries of the graph */
        entries.append ( n );

      if ( entries.size() == 0 )
        entries.append ( entry );
    }

  /* compute initial order by going through each tree */
  foreach ( Node *n, entries )
    assignInitialGridX ( n, posList );

  for ( i=0; i<layers.size(); ++i )
    qSort ( layers[i].begin(), layers[i].end(), compareGridX );

  if ( layers.size() == 1 )
    return;

  /* apply sugiyama's algorithm to the layers */
  applyToLayers ( layers );

  std::cout << "[su] " << count_nb_crossings(layers) << " edge crossings\n";
}
