/*
 * placing-cuckoo.cpp
 *
 * Implementation of the PlacingCuckoo class / cuckoo placing algorithm.
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
#include <cmath>
#include "placing-cuckoo.h"


/*
 * The genetic placing algorithm computes various configurations
 * and evaluates them using the criteria of the distance between
 * adjacent nodes.
 * After a round of computation and evaluation, the best configurations
 * are cloned, randomized and a few other new configurations are created.
 */

inline double cauchy ( void )
{
  double p = drand48 ( );
  while ( p == 0.0 )
    p = drand48 ( );
  return 0.6*tan(M_PI*(p-0.5)); // 0.6 is scaling factor
}

/*
 * Value
 */
class Value
{
 public:
  Value ( Node *n ) : node(n), score(0) { }
  ~Value ( ) { }

  /* compute the score of a value */
  inline unsigned long int computeScore ( )
  {
    QList<Value *>::const_iterator iter;

    score = 0;

    for ( iter=children.constBegin(); iter!=children.constEnd(); ++iter )
      score += ( ( (*iter)->pos - pos ) * ( (*iter)->pos - pos ) );

    for ( iter=parents.constBegin(); iter!=parents.constEnd(); ++iter )
      score += ( ( (*iter)->pos - pos ) * ( (*iter)->pos - pos ) );

    return score;
  }

  /* save the position held by a value */
  inline void savePos ( )
  {
    node->grid_x = pos;
  }

  /* change the position held by a value */
  inline void setPos ( const unsigned int p )
  {
    prev_pos = pos;
    pos = p;
  }

  /* change the position held by a value */
  inline void storeOriPos ( )
  {
    prev_pos = pos;
  }

  /* randomize the position held by a value */
  inline void randomize ( )
  {
    double c = cauchy ( );
    pos = (unsigned int) ((double) pos) + c;

    if ( pos >= *upperB )
      pos = *upperB - 1;
    else if ( pos <= *lowerB )
      pos = *lowerB + 1;
  }

  /* reset the position help by a value */
  inline void reset ( )
  {
    pos = prev_pos;
  }

  /* validate the position help by a value */
  inline void validate ( ) const
  {
    if ( pos <= *lowerB )
      std::cout << "node " << qPrintable(node->id) << " conflicts with lowerB\n";

    if  ( pos >= *upperB )
      std::cout << "node " << qPrintable(node->id) << " conflicts with upperB\n";
  }

  /* is a value representing a virtual node ? */
  inline bool isVirtual ( ) const
  {
    return node->isVirtual;
  }


 public:
  Node *node;
  QList<Value *> children;
  QList<Value *> parents;

  unsigned int *upperB;
  unsigned int *lowerB;
  unsigned int pos;
  unsigned int prev_pos;

  unsigned long int score;
} ;


/*
 * Egg (set of values) / Potential placing solution
 */
class Egg
{
 public:
  Egg ( unsigned int *Z, unsigned int *M ) : ZERO(Z), MAX(M) { }
  ~Egg ( ) { qDeleteAll(values); values.clear(); }

  /* initialize an egg from a list of values */
  void fill ( QList<Value *> &l )
  {
    Value *g, *prev, *ori, *prev_ori;
    int i;
    QHash<Value *, Value *> tb;

    qDeleteAll ( values );
    values.clear ( );

    /* copy values */
    ori = l[0];
    g = new Value ( ori->node );
    g->lowerB = ZERO;
    g->pos = ori->pos;
    values.append ( g );
    tb.insert ( ori, g );
    prev = g;
    prev_ori = ori;

    for ( i=1; i<l.size(); ++i )
      {
        ori = l[i];
        g = new Value ( ori->node );
        values.append ( g );
        tb.insert ( ori, g );
        g->pos = ori->pos;

        if ( ori->lowerB != ZERO )
          g->lowerB = &prev->pos;
        else
          g->lowerB = ZERO;

        if ( prev_ori->upperB != MAX )
          prev->upperB = &g->pos;
        else
          prev->upperB = MAX;

        prev = g;
        prev_ori = ori;
      }

    prev->upperB = MAX;

    /* set children/parents */
    for ( i=0; i<l.size(); ++i )
      {
        ori = l[i];
        g = tb.value ( ori );

        foreach ( Value *child, ori->children )
          g->children.append ( tb.value(child) );

        foreach ( Value *parent, ori->parents )
          g->parents.append ( tb.value(parent) );
      }
  }

  /* randomize an egg */
  void randomize ( )
  {
    for ( QList<Value *>::iterator iter=values.begin(); iter!=values.end(); ++iter )
      (*iter)->randomize ( );
  }

  /* save the positions */
  void savePos ( )
  {
    for ( QList<Value *>::iterator iter=values.begin(); iter!=values.end(); ++iter )
      (*iter)->savePos ( );
  }

  /* store the positions as the original one */
  void storeOriPos ( )
  {
    for ( QList<Value *>::iterator iter=values.begin(); iter!=values.end(); ++iter )
      (*iter)->storeOriPos ( );
  }

  /* reset the positions */
  void reset ( )
  {
    for ( QList<Value *>::iterator iter=values.begin(); iter!=values.end(); ++iter )
      (*iter)->reset ( );
  }

  /* compute the score of an egg */
  unsigned long int computeScore ( )
  {
    score = 0;

    for ( QList<Value *>::iterator iter=values.begin(); iter!=values.end(); ++iter )
      score += (*iter)->computeScore ( );

    return score;
  }

  /* validate an egg */
  void validate ( ) const
  {
    for ( QList<Value *>::const_iterator iter=values.begin(); iter!=values.end(); ++iter )
      (*iter)->validate ( );
  }

  /* align all the positions along the left border */
  void alignLeft ( )
  {
    unsigned int min_pos = *MAX;

    for ( QList<Value *>::iterator iter=values.begin(); iter!=values.end(); ++iter )
      if ( (*iter)->pos < min_pos )
        min_pos = (*iter)->pos;

    --min_pos; /* the left border has index=1 */

    for ( QList<Value *>::iterator iter=values.begin(); iter!=values.end(); ++iter )
      (*iter)->setPos ( (*iter)->pos - min_pos );
  }

 public:
  QList<Value *> values;

  unsigned long int score;

  unsigned int *ZERO;
  unsigned int *MAX;
} ;



#define PRECISION      1
#define NB_ITER        10000

#ifdef PLACING_TEST
extern unsigned long int placing_score;
#endif


extern bool compareLayerIndex ( const Node *, const Node * );


/*
 * Place nodes using a simplified cuckoo algorithm.
 */
void
PlacingCuckoo::applyToNodes ( QList<Node *> &nodes )
{
  unsigned int max_width = 0;
  QList<Value *> ref;
  Node *n;
  Value *g = NULL;
  Value *previous;
  Egg *e;

  unsigned long int min_score;
  int i, j;

  unsigned int ZERO = 0;
  unsigned int MAX;

  if ( nodes.size() == 0 )
    return;

  /* order the nodes */
  QList<QList<Node *> *> *layers = new QList<QList<Node *> *> ( );
  QList<Node *> *l;
  layers->append ( new QList<Node *> ( ) );

  foreach ( Node *n, nodes )
    {
      while ( (int)n->grid_y >= layers->size() )
        layers->append ( new QList<Node *> ( ) );

      l = layers->at ( n->grid_y );
      l->append ( n );

      if ( l->size() > (int)max_width )
        max_width = l->size ( );
    }

  max_width = max_width * PRECISION;
  MAX = max_width+1;

  for ( i=0; i<layers->size(); ++i )
    {
      l = layers->at ( i );
      qStableSort ( l->begin(), l->end(), compareLayerIndex );
    }


  /* build the reference values */
  QHash<Node *, Value *> *hb = new QHash<Node *, Value *> ( );
  previous = NULL;

  for ( i=0; i<layers->size(); ++i )
    {
      l = layers->at ( i );
      previous = NULL;

      n = l->at ( 0 );
      g = new Value ( n );
      g->lowerB = &ZERO;
      g->pos = ZERO;
      previous = g;
      hb->insert ( n, g );
      ref.append ( g );

      for ( j=1; j<l->size()-1; ++j )
        {
          n = l->at ( j );
          g = new Value ( n );
          g->lowerB = &previous->pos;
          g->pos = PRECISION * n->grid_x;
          previous->upperB = &g->pos;
          previous = g;
          hb->insert ( n, g );
          ref.append ( g );
        }

      if ( l->size() != 1 )
        {
          n = l->at ( j );
          g = new Value ( n );
          g->lowerB = &previous->pos;
          g->pos = PRECISION * n->grid_x;
          previous->upperB = &g->pos;
          previous = g;
          hb->insert ( n, g );
          ref.append ( g );
        }

      previous->upperB = &MAX;
    }

  foreach ( Node *n, nodes )
    {
      g = hb->value ( n );

      foreach ( Node *child, n->children )
        {
          if ( hb->value(child) == NULL )
            {
              std::cout << "[" << qPrintable(n->id) << "] error : NULL entry for child \'" << qPrintable(child->id) << "\' (" << child << ")\n";
              if ( child->isActive() )
                std::cout << "child is active !\n";
              else
                std::cout << "child is INactive !\n";
            }

          /* if the error occurs, then it might be because we have duplicate nodes IDs */

          g->children.append ( hb->value(child) );
        }

      foreach ( Node *parent, n->parents )
        {
          if ( hb->value(parent) == NULL )
            {
              std::cout << "[" << qPrintable(n->id) << "] error : NULL entry for parent \'" << qPrintable(parent->id) << "\' (" << parent << ")\n";
              if ( parent->isActive() )
                std::cout << "parent is active !\n";
              else
                std::cout << "parent is INactive !\n";
            }

          g->parents.append ( hb->value(parent) );
        }
    }

  
  qDeleteAll ( *layers );
  delete layers;
  delete hb;

  /* create two eggs */
#ifndef PLACING_TEST
  srand ( time(NULL) );
#endif

  e = new Egg ( &ZERO, &MAX );
  e->fill ( ref );

  e->computeScore ( );
  min_score = e->score;

  /* evaluate, produce offspring, mutate the best, add fresh flesh */
  for ( i=0; i<NB_ITER; ++i )
    {
      e->randomize ( );
      e->computeScore ( );

      if ( e->score > min_score )
        e->reset ( );
      else
        {
          min_score = e->score;
          e->storeOriPos ( );
        }

#ifdef PLACING_TEST
      if ( (i % 128) == 0 )
        std::cout << "[" << i << "] best score -> " << min_score << "\n";
#endif
    }

  // e->validate ( );

  //e->alignLeft ( );
  e->savePos ( );

#ifdef PLACING_TEST
  e->computeScore ( );
  placing_score = e->score;
  // std::cout << "[placing] reached score -> " << min_score << " (" << ((double)min_score/(min_score+improvement)*100.0) << "%)\n";
#endif

  /* free */
  qDeleteAll ( ref );
  delete e;
}
