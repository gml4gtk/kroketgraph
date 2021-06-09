/*
 * placing-genetic.cpp
 *
 * Implementation of the PlacingGenetic class / genetic placing algorithm.
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
#include "placing-genetic.h"


/*
 * The genetic placing algorithm computes various configurations
 * and evaluates them using the criteria of the distance between
 * adjacent nodes.
 * After a round of computation and evaluation, the best configurations
 * are cloned, randomized and a few other new configurations are created.
 */


/*
 * Gene
 */
class Gene
{
 public:
  Gene ( Node *n ) : node(n), score(0) { }
  ~Gene ( ) { }

  /* compute the score of a gene */
  inline unsigned long int computeScore ( )
  {
    QList<Gene *>::const_iterator iter;

    score = 0;

    for ( iter=children.constBegin(); iter!=children.constEnd(); ++iter )
      score += ( ( (*iter)->pos - pos ) * ( (*iter)->pos - pos ) );

    for ( iter=parents.constBegin(); iter!=parents.constEnd(); ++iter )
      score += ( ( (*iter)->pos - pos ) * ( (*iter)->pos - pos ) );

    return score;
  }

  /* save the position held by a gene */
  inline void savePos ( )
  {
    node->grid_x = pos;
  }

  /* change the position held by a gene */
  inline void setPos ( const unsigned int p )
  {
    prev_pos = pos;
    pos = p;
  }

  /* randomize the position held by a gene */
  inline void randomize ( )
  {
    int delta = *upperB - *lowerB;

    --delta;
    prev_pos = pos;

    if ( delta != 0 )
      setPos ( *lowerB + 1 + (rand() % delta) );
  }

  /* reset the position help by a gene */
  inline void reset ( )
  {
    pos = prev_pos;
  }

  /* compute a new position using the median heuristic */
  inline void median ( )
  {
    QList<Gene *>::const_iterator iter;
    int delta = 0;
    int res;

    prev_pos = pos;

    if ( ( parents.size() | children.size() ) == 0 )
      return;

    for ( iter=children.constBegin(); iter!=children.constEnd(); ++iter )
      delta += (*iter)->pos - pos;

    for ( iter=parents.constBegin(); iter!=parents.constEnd(); ++iter )
      delta += (*iter)->pos - pos;

    delta /= (parents.size()+children.size());
    res = pos + delta;

    if ( res <= (int)*lowerB )
      setPos ( *lowerB + 1 );
    else if ( res >= (int)*upperB )
      setPos ( *upperB - 1 );
    else
      setPos ( res );
  }

  /* validate the position help by a gene */
  inline void validate ( ) const
  {
    if ( pos <= *lowerB )
      std::cout << "node " << qPrintable(node->id) << " conflicts with lowerB\n";

    if  ( pos >= *upperB )
      std::cout << "node " << qPrintable(node->id) << " conflicts with upperB\n";
  }

  /* is a gene representing a virtual node ? */
  inline bool isVirtual ( ) const
  {
    return node->isVirtual;
  }

  /* straighten an edge of virtual nodes */
  void straightenDownward ( const unsigned int fix,
                            const unsigned int min,
                            const unsigned int max )
  {
    if ( !isVirtual() )
      return;

    this->straightened = false;

    if ( fix <= *lowerB )
      {
        this->straightened = true;
        // std::cout << "[" << qPrintable(node->id) << "] fix <= lowerB (" << fix << " <= " << *lowerB << ")\n";

        setPos ( 1 + *lowerB );

        parents[0]->straightenUpward ( fix );
        children[0]->straightenDownward ( pos, *lowerB, *upperB );
      }
    else if ( fix >= *upperB )
      {
        this->straightened = true;
        // std::cout << "[" << qPrintable(node->id) << "] fix >= upperB (" << fix << " >= " << *upperB << ")\n";
        setPos ( *upperB - 1 );
        parents[0]->straightenUpward ( fix );
        children[0]->straightenDownward ( pos, *lowerB, *upperB );
      }
    else
      {
        if ( children[0]->isVirtual() )
          {
            // std::cout << "[" << qPrintable(node->id) << "] going down\n";
            children[0]->straightenDownward ( fix,
                                              (min > *lowerB) ? min : *lowerB,
                                              (max < *upperB) ? max : *upperB );
          }
        else
          {
            // std::cout << "[" << qPrintable(node->id) << "] going up\n";
            this->straightenUpward ( fix );
          }
      }
  }

  void straightenUpward ( const unsigned int fix )
  {
    if ( ( !isVirtual() ) || ( this->straightened ) )
      return;

    // std::cout << "[" << qPrintable(node->id) << "] moving from " << pos << " to " << fix << "\n";
    setPos ( fix );
    parents[0]->straightenUpward ( fix );
  }


 public:
  Node *node;
  QList<Gene *> children;
  QList<Gene *> parents;

  unsigned int *upperB;
  unsigned int *lowerB;
  unsigned int pos;
  unsigned int prev_pos;

  unsigned long int score;

  bool straightened;
} ;


/*
 * Chromosome (set of genes)
 */
class Chromosome
{
 public:
  Chromosome ( unsigned int *Z, unsigned int *M ) : ZERO(Z), MAX(M) { }
  ~Chromosome ( ) { qDeleteAll(genes); genes.clear(); }

  /* initialize a chromosome from a list of genes */
  void fill ( QList<Gene *> &l )
  {
    Gene *g, *prev, *ori, *prev_ori;
    int i;
    QHash<Gene *, Gene *> tb;

    qDeleteAll ( genes );
    genes.clear ( );

    /* copy genes */
    ori = l[0];
    g = new Gene ( ori->node );
    g->lowerB = ZERO;
    g->pos = ori->pos;
    genes.append ( g );
    tb.insert ( ori, g );
    prev = g;
    prev_ori = ori;

    for ( i=1; i<l.size(); ++i )
      {
        ori = l[i];
        g = new Gene ( ori->node );
        genes.append ( g );
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

        foreach ( Gene *child, ori->children )
          g->children.append ( tb.value(child) );

        foreach ( Gene *parent, ori->parents )
          g->parents.append ( tb.value(parent) );
      }
  }

  /* randomize a chromosome */
  void randomize ( )
  {
    for ( QList<Gene *>::iterator iter=genes.begin(); iter!=genes.end(); ++iter )
      (*iter)->randomize ( );
  }

  /* mutate randomly a chromosome */
  void mutate ( )
  {
    for ( QList<Gene *>::iterator iter=genes.begin(); iter!=genes.end(); ++iter )
      {
        if ( (rand() & 3) == 0 ) /* 25% mutation rate */
          (*iter)->randomize ( );
      }
  }

  /* apply the median heuristic to a chromosome */
  void median ( )
  {
    for ( QList<Gene *>::iterator iter=genes.begin(); iter!=genes.end(); ++iter )
      (*iter)->median ( );
  }

  /* save the positions */
  void savePos ( )
  {
    for ( QList<Gene *>::iterator iter=genes.begin(); iter!=genes.end(); ++iter )
      (*iter)->savePos ( );
  }

  /* reset the positions */
  void reset ( )
  {
    for ( QList<Gene *>::iterator iter=genes.begin(); iter!=genes.end(); ++iter )
      (*iter)->reset ( );
  }

  /* compute the score of a chromosome */
  unsigned long int computeScore ( )
  {
    score = 0;

    for ( QList<Gene *>::iterator iter=genes.begin(); iter!=genes.end(); ++iter )
      score += (*iter)->computeScore ( );

    return score;
  }

  /* validate a chromosome */
  void validate ( ) const
  {
    for ( QList<Gene *>::const_iterator iter=genes.begin(); iter!=genes.end(); ++iter )
      (*iter)->validate ( );
  }

  /* align all the positions along the left border */
  void alignLeft ( )
  {
    unsigned int min_pos = *MAX;

    for ( QList<Gene *>::iterator iter=genes.begin(); iter!=genes.end(); ++iter )
      if ( (*iter)->pos < min_pos )
        min_pos = (*iter)->pos;

    --min_pos; /* the left border has index=1 */

    for ( QList<Gene *>::iterator iter=genes.begin(); iter!=genes.end(); ++iter )
      (*iter)->setPos ( (*iter)->pos - min_pos );
  }

  /* straighten the virtual edges */
  void straightenVirtualEdges ( )
  {
    QList<Gene *> vheads;
    Gene *g;

    /* find the first node of virtualized edges */
    for ( QList<Gene *>::iterator genesIter=genes.begin(); genesIter!=genes.end(); ++genesIter )
      {
        g = *genesIter;

        if ( g->isVirtual() )
          continue;

        for ( QList<Gene *>::iterator childrenIter=g->children.begin(); childrenIter!=g->children.end(); ++childrenIter )
          if ( (*childrenIter)->isVirtual() )
            vheads.append ( *childrenIter );
      }

    /* attempt to straighten each virtual edge */
    for ( QList<Gene *>::iterator iter=vheads.begin(); iter!=vheads.end(); ++iter )
      {
        g = *iter;
        // std::cout << "\n-- straightening from parent " << qPrintable(g->parents[0]->node->id) << "\n";
        g->straightenDownward ( g->parents[0]->pos, *g->lowerB, *g->upperB );
      }
  }


 public:
  QList<Gene *> genes;

  unsigned long int score;

  unsigned int *ZERO;
  unsigned int *MAX;
} ;



#define PRECISION      1
#define NB_ITER        12
#define NB_CHROMOSOMES 128
#define NB_KEPT        NB_CHROMOSOMES / 4
#define NB_FRESH       NB_CHROMOSOMES / 4

#ifdef PLACING_TEST
extern unsigned long int placing_score;
#endif


/*
 * Compare two nodes' layer index. This is used for sorting layer.
 */
bool compareLayerIndex ( const Node *n1,
                         const Node *n2 )
{
  return n1->grid_x < n2->grid_x;
}


/*
 * Compare two chromosomes' score. This is used for sorting.
 */
bool compareScore ( const Chromosome *c1,
                    const Chromosome *c2 )
{
  return c1->score < c2->score;
}


/*
 * Place nodes using a genetic algorithm.
 */
void
PlacingGenetic::applyToNodes ( QList<Node *> &nodes )
{
  unsigned int max_width = 0;
  QList<Gene *> ref;
  QList<Chromosome *> pool;
  Node *n;
  Gene *g = NULL;
  Gene *previous;
  Chromosome *c;

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


  /* build the reference genes */
  QHash<Node *, Gene *> *hb = new QHash<Node *, Gene *> ( );
  previous = NULL;

  for ( i=0; i<layers->size(); ++i )
    {
      l = layers->at ( i );
      previous = NULL;

      n = l->at ( 0 );
      g = new Gene ( n );
      g->lowerB = &ZERO;
      g->pos = ZERO;
      previous = g;
      hb->insert ( n, g );
      ref.append ( g );

      for ( j=1; j<l->size()-1; ++j )
        {
          n = l->at ( j );
          g = new Gene ( n );
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
          g = new Gene ( n );
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

  /* create initial population */
#ifndef PLACING_TEST
  srand ( time(NULL) );
#endif

  for ( i=0; i<NB_CHROMOSOMES; ++i )
    {
      c = new Chromosome ( &ZERO, &MAX );
      c->fill ( ref );

      c->randomize ( );
      c->randomize ( );
      c->randomize ( );
      c->median ( );
      c->median ( );
      c->straightenVirtualEdges ( );
      c->straightenVirtualEdges ( );

      pool.append ( c );
    }

  int improvement = 0;
  min_score = pool[0]->computeScore ( );

  /* evaluate, produce offspring, mutate the best, add fresh flesh */
  for ( i=0; i<NB_ITER; ++i )
    {
      for ( j=0; j<NB_CHROMOSOMES; ++j )
        pool[j]->computeScore ( );

      qSort ( pool.begin(), pool.end(), compareScore );

      improvement += min_score - pool[0]->score;
      min_score = pool[0]->score;

      for ( j=NB_KEPT; j<NB_CHROMOSOMES-NB_FRESH; ++j )
        {
          c = pool[j];
          c->fill ( pool[j%NB_KEPT]->genes );
          c->randomize ( );
          c->median ( );
          c->straightenVirtualEdges ( );
          c->straightenVirtualEdges ( );
        }

      for ( j=0; j<NB_KEPT; ++j )
        pool[j]->median ( );

      for ( j=NB_CHROMOSOMES-NB_FRESH; j<NB_CHROMOSOMES; ++j )
        {
          pool[j]->fill ( ref );
          pool[j]->randomize ( );
          pool[j]->median ( );
          pool[j]->median ( );
          pool[j]->straightenVirtualEdges ( );
          pool[j]->straightenVirtualEdges ( );
        }

      pool[0]->computeScore ( );

      if ( pool[0]->score > min_score )
        pool[0]->reset ( );
      else
        min_score = pool[0]->score;

#ifdef PLACING_TEST
      double avg=0.0;
      for ( int j=0; j<pool.size(); ++j )
        avg += (double)pool[j]->score;
      std::cout << "[" << i << "] best score -> " << min_score << " ; average -> " << avg/(double)pool.size() << "\n";
#endif
    }

  // pool[0]->validate ( );

  pool[0]->straightenVirtualEdges ( );
  pool[0]->straightenVirtualEdges ( );
  // pool[0]->computeScore ( );
  // std::cout << "[straightening improvement]  " << min_score << " -> " << pool[0]->score << "\n";

  pool[0]->alignLeft ( );
  pool[0]->savePos ( );

#ifdef PLACING_TEST
  pool[0]->computeScore ( );
  placing_score = pool[0]->score;
  // std::cout << "[placing] reached score -> " << min_score << " (" << ((double)min_score/(min_score+improvement)*100.0) << "%)\n";
#endif

  /* free */
  qDeleteAll ( pool );
  qDeleteAll ( ref );
}
