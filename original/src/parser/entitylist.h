/*
 * entitylist.h
 *
 * Declaration of the EntityList class.
 * This is a data structure for entities storage.
 * It has a built-in parser for i/o operations.
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

#ifndef __ENTITYLIST_H__
#define __ENTITYLIST_H__

#include <iostream>
#include <string>
#include <exception>

#include "entity.h"


/*
 * Internal structure (chained list of chained lists)
 */
typedef struct _entity_chain entity_chain;

struct _entity_chain
{
  Entity *entity;
  entity_chain *next;
} ;

typedef struct _entity_set entity_set;

struct _entity_set
{
  std::string *type;
  entity_chain *chain;
  entity_set *next;
} ;


/*
 * Parser Exception
 */
class ParserException
{

 public:
  ParserException ( const char *msg )
    {
      this->msg = new std::string ( msg );
    }

  ~ParserException ( )
    {
      delete msg;
    }

  const char *what ( ) const { return msg->c_str(); }

  friend std::ostream &
  operator << ( std::ostream &stream, const ParserException &pe )
    {
      stream << "ParserException: " << pe.msg << "\n";
      return stream;
    }

 private:
  std::string *msg;

} ;


/*
 * EntityList
 */
class EntityList
{

 public:
  EntityList ( );
  ~EntityList ( );

  void parseFromFile ( const char * );
  void unparseToFile ( const char * );

  Entity *addEntityWithType ( const char * );
  void clear ( );

 private:
  entity_set *sets;
  unsigned int entity_count;


 public:
  class iterator
    {
    public:
      iterator ( ) : ptr(NULL),s(NULL) { }
      iterator ( entity_chain *ec ) : ptr(ec),s(NULL) { }
      iterator ( entity_set *eset ) : ptr(eset->chain),s(eset) { }
      ~iterator ( ) { }

      inline Entity *next ( )
        {
          Entity *e = ptr->entity;
          ptr = ptr->next;

          if ( ( s != NULL ) && ( ptr == NULL ) )
            {
              s = s->next;

              if ( s != NULL )
                ptr = s->chain;
            }

          return e;
        }

      inline bool hasNext ( ) const { return (ptr != NULL); }

    private:
      entity_chain *ptr;
      entity_set *s;

    } ;

  EntityList::iterator iterate ( const char *type=NULL ) const;

} ;

#endif
