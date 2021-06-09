/*
 * Parser test file
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

#include <unistd.h>

#include "entitylist.h"


int
main ( int argc,
       char *argv[] )
{
  EntityList *l;

  if ( argc > 1 )
    {
      if ( strcmp("-s",argv[1]) == 0 )
        {
          /* test speed */
          l = new EntityList ( );

          char e_name[128];
          char a_names[128][128];

          for ( int i=0; i<64; ++i )
            sprintf ( a_names[i], "attr%d", i );

          for ( unsigned int j=0; j<32768; ++j )
            {
              Entity *e;
              e = l->addEntityWithType ( "entity" );

              sprintf ( e_name, "value%d", j );
              e->addAttribute ( "name", e_name );

              for ( int i=0; i<48; ++i )
                {
                  try { e->addAttribute ( a_names[i], "value" ); }
                  catch ( std::bad_alloc &e ) { std::cout << "error at entity " << j << std::endl; break; }
                }
            }

          l->unparseToFile ( "speedtest.gd" );

          l->parseFromFile ( "speedtest.gd" );

          delete l;

          unlink ( "speedtest.gd" );
        }
      else if ( strcmp("-c",argv[1]) == 0 )
        {
          /* test consistency */
          l = new EntityList ( );

          char a_names[128][128];

          for ( int i=0; i<64; ++i )
            sprintf ( a_names[i], "attr%d", i );

          Entity *e = l->addEntityWithType ( "entity" );

          e->addAttribute ( "attr", a_names[0] );
          e->addAttribute ( "bttr", a_names[1] );
          e->addAttribute ( "cttr", a_names[2] );
          e->addAttribute ( "dttr", a_names[3] );
          e->addAttribute ( "lttr", a_names[4] );
          e->addAttribute ( "atsd", a_names[5] );
          e->addAttribute ( "afsd", a_names[6] );

          l->unparseToFile ( "ctest.gd" );

          l->parseFromFile ( "ctest.gd" );

          EntityList::iterator iter = l->iterate ( );

          if ( iter.hasNext() )
            {
              e = iter.next ( );
              const char *v;

              v = e->getValueOfAttribute ( "attr" );
              std::cout << v << std::endl;

              v = e->getValueOfAttribute ( "dttr" );
              std::cout << v << std::endl;

              v = e->getValueOfAttribute ( "afsd" );
              std::cout << v << std::endl;

              e->attributes->remove ( "afsd" );
              v = e->getValueOfAttribute ( "afsd" );
              std::cout << (v?v:"NULL") << std::endl;
            }

          delete l;

          unlink ( "ctest.gd" );
        }
    }
  else
    {
      l = new EntityList ( );
      l->parseFromFile ( "sample.gd" );
      l->unparseToFile ( "sample_out.gd" );
      delete l;
    }

  return 0;
}
