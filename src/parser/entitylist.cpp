/*
 * entitylist.cpp
 *
 * Implementation of the EntityList object's methods.
 *
 * Handles a set of entities, which are stored according to their type.
 * There is no check for duplicate elements.
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

#include <sys/types.h>
#include <sys/stat.h>
#ifndef MSWIN
#include <sys/mman.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cerrno>
#include "entitylist.h"
#include "masked_isspace.h"


/*
 * Constructor
 */
EntityList::EntityList ( )
{
  this->sets = NULL;
}


/*
 * Read the next un-interrupted sequence of non-space characters
 * and copy it into the provided string.
 * This function returns the number of bytes read/skipped.
 */
inline static int
get_word ( const char *start,
           const char *limit,
           std::string *str )
{
  const char *ptr;
  const char *word;
  char prev;
  bool quoting;

  for ( ptr=start; ptr!=limit; ++ptr )
    if ( !_isspace(*ptr) )
      break;

  word = ptr;
  quoting = false;
  prev = ' ';

  while ( ptr!=limit )
    {
      if ( ( *ptr == '\"' ) && ( prev != '\\' ) )
        {
          quoting = !quoting;
          prev = *ptr;
          ++ptr;
          continue;
        }

      if ( _isspace(*ptr) && !quoting )
        break;

      prev = *ptr;
      ++ptr;
    }

  str->assign ( word, ptr-word );

  return ptr-start;
}


/*
 * Read entities from a file.
 */
void
EntityList::parseFromFile ( const char *file_name )
{
  assert ( file_name != NULL );

  this->clear ( );

  /* open the input file and calculate its size */
  int fhandle = open ( file_name, O_RDONLY );

  if ( fhandle == -1 )
    {
      std::cerr << __FILE__ << "(" << __LINE__ << "): \'" << file_name << "\': " << strerror(errno) << std::endl;
      throw new ParserException ( strerror(errno) );
    }

  size_t fsize = (size_t) lseek ( fhandle, 0, SEEK_END );
  lseek ( fhandle, 0, SEEK_SET );

  /* mmap file content */
#ifndef MSWIN
  char *fmap = (char *) mmap ( 0, fsize, PROT_READ, MAP_PRIVATE, fhandle, 0 );

  if ( fmap == MAP_FAILED )
    {
      std::cerr << __FILE__ << "(" << __LINE__ << "): \'" << file_name << "\': " << strerror(errno) << std::endl;
      close ( fhandle );
      throw new ParserException ( strerror(errno) );
    }
#else
  char *fmap = (char *) malloc ( sizeof(char)*fsize );
  read ( fhandle, fmap, fsize );
#endif


  /* parsing */
  const char *limit = fmap + fsize;
  std::string *str = new std::string ( );
  Entity *e;

  for ( const char *ptr=fmap; ptr<limit; ++ptr )
    {
      if ( *ptr == '<' ) /* entity start character */
        {
          ++ptr;

          ptr += get_word ( ptr, limit, str );

          e = this->addEntityWithType ( str->c_str() );

          while ( true )
            {
              ptr += get_word ( ptr, limit, str );

              if ( str->length() == 0 )
                break;

              if ( ( (*str)[0] == '/' ) && /* entity end */
                   ( (*str)[1] == '>' ) )
                break;

              e->parseAndAddAttributeFromString ( str );
            }
        }
    }

  delete str;

  /* close the input file and unmap */
  close ( fhandle );
#ifndef MSWIN
  munmap ( fmap, fsize );
#else
  free ( fmap );
#endif
}


/*
 * Unparse entities to a file.
 */
void
EntityList::unparseToFile ( const char *file_name )
{
  assert ( file_name != NULL );

  /* open the output file */
  std::ofstream fout ( file_name, std::ios::out|std::ios::trunc );

  if ( fout.bad() )
    {
      std::cerr << __FILE__ << "(" << __LINE__ << "): unparse_to_file(): " << strerror(errno) << std::endl;
      throw new ParserException ( strerror(errno) );
    }


  /* unparsing */
  Entity *e;
  EntityList::iterator iter = this->iterate ( );

  while ( iter.hasNext() )
    {
      e = iter.next ( ); 
      fout << *e;
    }

  /* close the output file */
  fout.close ( );
}


/*
 * Get the set that has the requested name.
 * Returns NULL if the set couldn't be found.
 */
static inline entity_set *
get_set_with_type ( entity_set *eset,
                    const char *type )
{
  while ( eset != NULL )
    {
      if ( eset->type->compare(type) == 0 )
        return eset;

      eset = eset->next;
    }

  return NULL;
}


/*
 * Add a new entity.
 * Returns a pointer to the added entity or NULL if an error occured.
 */
Entity *
EntityList::addEntityWithType ( const char *type )
{
  entity_set *eset;
  entity_chain *ec;

  eset = get_set_with_type ( this->sets, type );
  ec = new entity_chain;
  ec->entity = new Entity ( type );

  if ( eset == NULL )
    {
      eset = new entity_set;
      eset->type = new std::string ( type );

      eset->next = this->sets;
      this->sets = eset;

      ec->next = NULL;
    }
  else
    ec->next = eset->chain;

  eset->chain = ec;

  return ec->entity;
}


/*
 * Build an iterator over entities.
 * A specific type can be provided.
 */
EntityList::iterator
EntityList::iterate ( const char *type ) const
{
  entity_set *eset;

  if ( type == NULL )
    return iterator(this->sets); /* iterator over all entities */

  eset = get_set_with_type ( this->sets, type );

  if ( eset == NULL )
    return iterator();           /* iterator over nothing */

  return iterator(eset->chain);  /* iterator over a specific type of entities */
}


/*
 * Clear the entity list.
 */
void
EntityList::clear ( )
{
  entity_set *eset;
  entity_chain *ec;

  while ( this->sets != NULL )
    {
      eset = this->sets;
      this->sets = eset->next;

      while ( eset->chain != NULL )
        {
          ec = eset->chain;
          eset->chain = ec->next;

          delete ec->entity;
          delete ec;
        }

      delete eset->type;
      delete eset;
    }
}


/*
 * Destructor
 */
EntityList::~EntityList ( )
{
  this->clear ( );
}
