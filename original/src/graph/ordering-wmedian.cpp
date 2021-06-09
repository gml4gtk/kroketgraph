/*
 * ordering-wmedian.cpp
 *
 * Implementation of the OrderingWMedian class / weighted median ordering algorithm.
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
#include "ordering-wmedian.h"


/*
 * The weighted median ordering algorithm
 *
 * The median value of a node is defined as the median position
 * of its adjacent nodes. Each layer is sorted according to
 * the median values of the nodes it contains.
 */


#define NB_ITER 24


/*
 * The following structure is used to store a given configuration.
 */
class WMConfig
{
 public:
  /* constructor */
  WMConfig ( const QList<Node *> &l, const unsigned int score  )
  {
    QList<Node *>::const_iterator iter = l.constBegin ( );
    pos.clear ( );

    while ( iter != l.constEnd() )
      {
        pos.append ( new QPair<Node *, unsigned int> ( *iter, (*iter)->grid_x ) );
        ++iter;
      }

    this->score = score;
  }

  /* destructor */
  ~WMConfig ( )
  {
    qDeleteAll ( pos );
  }

  /* read and save the nodes positions */
  void sync ( unsigned int score )
  {
    QList<QPair<Node *, unsigned int> *>::iterator iter = pos.begin ( );
    QPair<Node *, unsigned int> *pair;

    while ( iter != pos.end() )
      {
        pair = *iter;
        pair->second = pair->first->grid_x;
        ++iter;
      }

    this->score = score;
  }

  /* write the saved positions back to the nodes */
  void writeBack ( )
  {
    QList<QPair<Node *, unsigned int> *>::const_iterator iter = pos.constBegin ( );
    QPair<Node *, unsigned int> *pair;

    while ( iter != pos.constEnd() )
      {
        pair = *iter;
        pair->first->grid_x = pair->second;
        ++iter;
      }
  }

 public:
  unsigned int score;
  QList<QPair<Node *, unsigned int> *> pos;

} ;


/*
 * Assign the weighted median coefficient of a parent.
 */
static inline void
assign_parental_wmedian_coef ( Node *n )
{
  const int nbChildren = n->children.size ( );

  if ( nbChildren == 0 )       /* no children, dummy value */
    n->coef = -1.0;
  else if ( nbChildren & 1 )   /* odd number of children, central value */
    n->coef = n->children[nbChildren >> 1]->grid_x;
  else if ( nbChildren == 2 )
    n->coef = (n->children[0]->grid_x + n->children[1]->grid_x) / 2.0;
  else
    {
      const unsigned int left = n->children[(nbChildren>>1)-1]->grid_x - n->children[0]->grid_x;
      const unsigned int right = n->children[nbChildren-1]->grid_x - n->children[nbChildren>>1]->grid_x;
      n->coef = n->children[(nbChildren>>1)-1]->grid_x * right + n->children[nbChildren>>1]->grid_x * left;
      n->coef = n->coef / (left+right);
    }
}


/*
 * Assign the weighted median coefficient of a child.
 */
static inline void
assign_filial_wmedian_coef ( Node *n )
{
  const int nbParents = n->parents.size ( );

  if ( nbParents == 0 )       /* no parents, dummy value */
    n->coef = -1.0;
  else if ( nbParents & 1 )   /* odd number of parents, central value */
    n->coef = n->parents[nbParents >> 1]->grid_x;
  else if ( nbParents == 2 )
    n->coef = (n->parents[0]->grid_x + n->parents[1]->grid_x) / 2.0;
  else
    {
      const unsigned int left = n->parents[(nbParents>>1)-1]->grid_x - n->parents[0]->grid_x;
      const unsigned int right = n->parents[nbParents-1]->grid_x - n->parents[nbParents>>1]->grid_x;
      n->coef = n->parents[(nbParents>>1)-1]->grid_x * right + n->parents[nbParents>>1]->grid_x * left;
      n->coef = n->coef / (left+right);
    }
}


/*
 * Compare two nodes' weighted median coefficient. This is used for sorting layer.
 */
static bool
compareMediCoef ( const Node *n1,
                  const Node *n2 )
{
  if ( n1->coef == -1.0 )
    {
      if ( n2->coef == -1.0 )
        return ( n1->grid_x < n2->grid_x );
      else
        return ( (double)n1->grid_x < n2->coef );
    }
  else
    {
      if ( n2->coef == -1.0 )
        return ( n1->coef < (double)n2->grid_x );
      else
        return ( n1->coef < n2->coef );
    }
}


/*
 * Sort the nodes in each layer (step 1 of sugiyama's algorithm)
 */
static inline void
sort_using_filial_wmedian_coef ( QList<QList<Node *> > &layers )
{
  int i, j;
  
  /* sort the nodes of the layer[i] according to the nodes of the layer[i-1] */
  for ( i=1; i<layers.size(); ++i )
    {
      QList<Node *> &l = layers[i];

      /* calculate filial weighted median coefs */
      foreach ( Node *n, l )
        assign_filial_wmedian_coef ( n );

      /* sort */
      qStableSort ( l.begin(), l.end(), compareMediCoef );

      /* update indexes */
      for ( j=0; j<l.size(); ++j )
        l[j]->grid_x = j+1;
    }
}


/*
 * Sort the nodes in each layer (step 1 of sugiyama's algorithm)
 */
static inline void
sort_using_parental_wmedian_coef ( QList<QList<Node *> > &layers )
{
  int i, j;

  /* sort the nodes of the layer[i] according to the nodes of the layer[i+1] */
  for ( i=(layers.size()-2); i>=0; --i )
    {
      QList<Node *> &l = layers[i];

      /* calculate parental barycenter coefs */
      foreach ( Node *n, l )
        assign_parental_wmedian_coef ( n );

      /* sort */
      qStableSort ( l.begin(), l.end(), compareMediCoef );

      /* update indexes */
      for ( j=0; j<l.size(); ++j )
        l[j]->grid_x = j+1;
    }
}


/*
 * Count the number of edge crossings between two nodes.
 * count_nb_crossings(a,b,s) != count_nb_crossings(b,a,s)
 * The function consider n1 is on the left side of n2.
 */
static inline unsigned int
count_nb_crossings ( const Node *n1,
                     const Node *n2,
                     QList<Node *> *upperLayer,
                     QList<Node *> *lowerLayer )
{
  unsigned int ret = 0;
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
static unsigned int
count_nb_crossings ( QList<QList<Node *> > &layers )
{
  int i, j, k;
  unsigned int ret = 0;

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
 * Exchange the position of nodes within a layer if it improves the layout.
 */
static void
transpose ( QList<QList<Node *> > &layers )
{
  int i, j;
  Node *n1, *n2;
  bool improved = true;

  while ( improved )
    {
      improved = false;

      /* handle the first layer separately */
      QList<Node *> &l0 = layers[0];

      for ( j=0; j<l0.size()-1; ++j )
        {
          n1 = l0[j];
          n2 = l0[j+1];

          const int c_ori = count_nb_crossings ( n1, n2, NULL, &layers[1] );
          const int c_exc = count_nb_crossings ( n2, n1, NULL, &layers[1] );

          if ( c_ori > c_exc )
            {
              improved = true;
              l0.swap ( j, j+1 );
              n2->grid_x = j+1;
              n1->grid_x = j+2;
            }
        }

      /* handle layers that have an upper and a lower layer */
      for ( i=1; i<layers.size()-1; ++i )
        {
          QList<Node *> &l = layers[i];

          for ( j=0; j<l.size()-1; ++j )
            {
              n1 = l[j];
              n2 = l[j+1];

              const int c_ori = count_nb_crossings ( n1, n2, &layers[i-1], &layers[i+1] );
              const int c_exc = count_nb_crossings ( n2, n1, &layers[i-1], &layers[i+1] );

              if ( c_ori > c_exc )
                {
                  improved = true;
                  l.swap ( j, j+1 );
                  n2->grid_x = j+1;
                  n1->grid_x = j+2;
                }
            }
        }

      /* handle the last layer separately */
      QList<Node *> &lN = layers[i];

      for ( j=0; j<lN.size()-1; ++j )
        {
          n1 = lN[j];
          n2 = lN[j+1];

          const int c_ori = count_nb_crossings ( n1, n2, &layers[i-1], NULL );
          const int c_exc = count_nb_crossings ( n2, n1, &layers[i-1], NULL );

          if ( c_ori > c_exc )
            {
              improved = true;
              lN.swap ( j, j+1 );
              n2->grid_x = j+1;
              n1->grid_x = j+2;
            }
        }
    }
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
 * Order a list of nodes using the median algorithm.
 */
void
OrderingWMedian::applyToNodes ( QList<Node *> &nodes )
{
  QList<QList<Node *> > layers;
  QList<unsigned int> posList;
  QList<Node *> entries;
  Node *entry;
  int i;

  if ( nodes.size() == 0 )
    return;

  entry = nodes[0];

  /* build layers and entry points list */
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
    }

  if ( entries.size() == 0 )
    entries.append ( entry );

  /* compute initial order by going through each tree */
  foreach ( Node *n, entries )
    assignInitialGridX ( n, posList );

  for ( i=0; i<layers.size(); ++i )
    qSort ( layers[i].begin(), layers[i].end(), compareGridX );

  if ( layers.size() == 1 )
    return;

  /* apply the weighted median sorting algorithm to the layers */
  unsigned int nbc = count_nb_crossings ( layers );
  WMConfig conf ( nodes, nbc );

  for ( i=0; i<NB_ITER; ++i )
    {
      //std::cout << "[wm " << i << " W] " << nbc << " edge crossings\n";

      sort_using_filial_wmedian_coef ( layers );
      transpose ( layers );
      sort_using_parental_wmedian_coef ( layers );
      transpose (  layers );

      nbc = count_nb_crossings ( layers );

      if ( nbc == 0 )
        break; /* no more crossing edges, bail out */

      if ( nbc < conf.score )
        conf.sync ( nbc );
    }

  if ( nbc != 0 )
    conf.writeBack ( );

  //std::cout << "[wm] " << conf.score << " edge crossings\n";
}
