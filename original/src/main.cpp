/*
 * main.cpp
 *
 * The main() function is here.
 * It checks the command line
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

#include <QApplication>
#include <getopt.h>

#include "application.h"


#define KROKET_BIN "kroket"


/*
 * options array (used by getopt)
 */
static const char * options = "vhEe:Cc:f:t:w:W:";

static struct option long_options[] = {
  {"version",      0, NULL, 'v'},
  {"help",         0, NULL, 'h'},
  {"expand-all",   0, NULL, 'E' },
  {"expand",       1, NULL, 'e' },
  {"collapse-all", 0, NULL, 'C' },
  {"collapse",     1, NULL, 'c' },
  {"focus",        1, NULL, 'f'},
  {"trace",        1, NULL, 't'},
  {"with",         1, NULL, 'w'},
  {"without",      1, NULL, 'W' },
  {NULL,           0, NULL,  0 }
};


/*
 * usage
 */
void
usage ( void )
{
  fprintf ( stderr, "\nusage: %s [options] file\n\n", KROKET_BIN );

  fprintf ( stderr, "General Options:\n\t"                                            \
                    "-h, --help\t\tshow this help\n\t"                                \
                    "-v, --version\t\tshow version number\n\n"                        \
                    "Program Behaviour:\n\t"                                          \
                     "-E, --expand-all\texpand all the groups\n\t"                    \
                     "-e ID, --expand=ID\texpand the specified group\n\t"             \
                     "-C, --collapse-all\tcollapse all the groups (default)\n\t"      \
                     "-c ID, --collapse=ID\tcollapse the specified group\n\t"         \
                     "-f ID, --focus=ID\tfocus on the specified node\n\t"             \
                     "-t ID, --trace=ID\ttrace from the specified node\n\t"           \
                     "-w TAG, --with=TAG\tshow only nodes with the specified tag\n\t" \
                     "-W TAG, --without=TAG\tshow only nodes without the specified tag\n\n" );
}


int
main ( int argc,
       char *argv[] )
{
  char c;
  int ret;
  QString nodeFocus;
  QString nodeTrace;
  QStringList with;
  QStringList without;

  bool expandAll = false;
  QStringList expand;

  bool collapseAll = false;
  QStringList collapse;


  QApplication app ( argc, argv );

  /* args parsing */
  while ( ( c = getopt_long(argc, argv, options, long_options, NULL) ) != -1 )
    {
      switch ( c )
        {
        case 'h':
          {
            usage ( );
            return 0;
          }

        case 'v':
          {
            fprintf ( stderr, "\n%s version %d.%d.%d\n\n", KROKET_BIN, KROKET_VER_MAJ, KROKET_VER_MIN, KROKET_VER_PAT );
            fprintf ( stderr, "Copyright (c) 2010 Nicolas BENOIT\n"     \
                      "This is free software; see the source for copying conditions.  There is NO\n" \
                      "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n" );
            return 0;
          }

        case 'f':
          {
            nodeFocus = QString::fromAscii ( optarg );
            break;
          }

        case 't':
          {
            nodeTrace = QString::fromAscii ( optarg );
            break;
          }

        case 'E':
          {
            expandAll = true;
            collapseAll = false;
            break;
          }

        case 'e':
          {
            expand.append ( QString::fromAscii(optarg) );
            break;
          }

        case 'C':
          {
            collapseAll = true;
            expandAll = false;
            break;
          }

        case 'c':
          {
            collapse.append ( QString::fromAscii(optarg) );
            break;
          }

        case 'w':
          {
            with.append ( QString::fromAscii(optarg) );
            break;
          }

        case 'W':
          {
            without.append ( QString::fromAscii(optarg) );
            break;
          }

        default:
          break;
        }
    }

  /* build and show UI */
  AppKroket *kroket = new AppKroket ( );
  kroket->show ( );

  while ( optind < argc )
    {
      kroket->openFile ( argv[optind] );

      /* expanding / collapsing */
      if ( expandAll )
        kroket->currentGraphView()->expandAll ( false );
      else if ( collapseAll )
        kroket->currentGraphView()->collapseAll ( false );

      foreach ( QString str, expand )
        kroket->currentGraphView()->expand ( str, false );

      foreach ( QString str, collapse )
        kroket->currentGraphView()->collapse ( str, false );

      /* tracing */
      if ( nodeTrace.size() != 0 )
        kroket->currentGraphView()->traceFrom ( nodeTrace, false );

      /* filtering */
      foreach ( QString str, with )
        kroket->currentGraphView()->filterWithTag ( str, false );

      foreach ( QString str, without )
        kroket->currentGraphView()->filterWithoutTag ( str, false );

      /* synchronize view */
      kroket->currentGraphView()->synchronizeView ( );

      /* focus */
      if ( nodeFocus.size() != 0 )
        kroket->currentGraphView()->focusOn ( nodeFocus );

      ++optind;
    }

  ret = app.exec ( );

  delete kroket;

  return ret;
}
