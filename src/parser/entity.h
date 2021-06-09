/*
 * entity.h
 *
 * Declaration of the Entity class which is mainly a container for attributes.
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

#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <string>
#include <iostream>
#include "attributelist.h"


class Entity
{

 public:
  Entity ( const char * );
  ~Entity ( );

  /*
   * Add an attribute to an existing entity.
   */
  inline void addAttribute ( const char *name, const char *val )
    { attributes->add ( name, val ); }

  void parseAndAddAttributeFromString ( const std::string * );

  /*
   * Get the attribute with the provided name of an entity.
   * Returns NULL if the attribute can not be found.
   */
  inline Attribute *getAttribute ( const char *name ) const
    { return this->attributes->get ( name ); }

  /*
   * Get the value of the attribute which has the provided name.
   * Returns NULL if the attribute can not be found.
   */
  inline const char *getValueOfAttribute ( const char *name ) const
    {
      Attribute *a = getAttribute ( name );
      return ( a==NULL ) ? NULL : a->getValue() ;
    }

  /*
   * Get an iterator over the attributes of an entity.
   */
  inline AttributeList::iterator iterateOverAttributes ( ) const
    { return this->attributes->iterate ( ); }


  friend std::ostream & operator << ( std::ostream &, Entity & );

 private:
  std::string type;

 public:
  AttributeList *attributes;

} ;


#endif
