/*
 * attributelist.h
 *
 * Declaration of the AttributeList class.
 * This is a data structure for attributes storage.
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

#ifndef __ATTRIBUTELIST_H__
#define __ATTRIBUTELIST_H__

#include <iostream>
#include <string>

#include "attribute.h"


/*
 * Internal structure (chained list)
 */
typedef struct _attr_chain attr_chain;

struct _attr_chain
{
  Attribute *attr;
  attr_chain *next;
} ;


/*
 * AttributeList
 */
class AttributeList
{

 public:
  AttributeList ( );
  ~AttributeList ( );

  Attribute *add ( const char *, const char * );
  Attribute *get ( const char * ) const;
  void remove ( const char * );
  void clear ( );

 private:
  attr_chain *list[8];


 public:
  class iterator
    {
    public:
      iterator ( attr_chain **l )
        {
          list = l;
          i = -1;

          do { ptr = list[++i]; }
          while ( ( ptr==NULL ) && ( i!=7 ) ) ;
        }

      ~iterator ( )
        { }

      inline Attribute *next ( )
        {
          Attribute *a;
          a = ptr->attr;
          ptr = ptr->next;

          while ( i != 7 )
            {
              if ( ptr != NULL )
                break;

              ptr = list[++i];
            }

          return a;
        }

      inline bool hasNext ( ) const
        { return (ptr != NULL); }

    private:
      attr_chain *ptr;
      attr_chain **list;
      int i;
    } ;

  inline AttributeList::iterator iterate ( )
    { return iterator(list); };

} ;

#endif
