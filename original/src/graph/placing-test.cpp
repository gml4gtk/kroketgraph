/*
 * placing-test.cpp
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

#include <sys/time.h>
#include "graph.h"
#include "layering-floyd.h"
#include "layering-lazy.h"
#include "ordering-wmedian.h"
#include "placing-genetic.h"
#include "placing-cuckoo.h"

unsigned long int placing_score;

int
main ( int argc,
       char *argv[] )
{
  int i;
  struct timeval start, end;

  std::cout << "\n";

  for ( i=1; i<argc; ++i )
    {
      Graph *g = new Graph ( );
      EntityList *el = new EntityList ( );

      el->parseFromFile ( argv[i] );
      g->initFromEntityList ( el );
      delete el;

      QList<Node *> nodes_list;
      g->feedListWithActiveNodes ( nodes_list );

      LayeringFloyd::applyToNodes ( nodes_list );
      LayeringLazy::applyToNodes ( nodes_list );
      g->reverseUpwardEdges ( );
      g->virtualizeLongEdges ( );
      g->feedListWithActiveNodes ( nodes_list ); /* update the nodes list because of the virtualization */
      OrderingWMedian::applyToNodes ( nodes_list );

      srand ( 7 );
      gettimeofday ( &start, NULL );
      PlacingGenetic::applyToNodes ( nodes_list );
      std::cout << "final score -> " << placing_score << "\n";
      PlacingCuckoo::applyToNodes ( nodes_list );
      gettimeofday ( &end, NULL );

      std::cout << "final score -> " << placing_score << "\n";
      std::cout << "elapsed time -> " << (((end.tv_sec-start.tv_sec)*1000000.0+(end.tv_usec-start.tv_usec))/1000.0) << "ms\n\n";

      delete g;
    }

  return 0;
}
