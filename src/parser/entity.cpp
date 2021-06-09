/*
 * entity.cpp
 *
 * Implementation of the entity object's methods.
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

#include <map>
#include <cassert>
#include <iostream>

#include "entity.h"


/*
 * Constructor
 */
Entity::Entity ( const char *type )
{
  assert ( type != NULL );

  this->type.assign ( type );
  this->attributes = new AttributeList ( );
}


/*
 * Parse an attribute and add it to an existing entity.
 * attribute format is: name="value"
 */
void
Entity::parseAndAddAttributeFromString ( const std::string *str )
{
  std::string::size_type i;
  std::string n;
  std::string v;

  i = str->find ( "=" );
  n = str->substr ( 0, i );
  v = str->substr ( i+2, str->length()-i-3 ); /* -3 because we skip '=', amd two '"' */

  this->addAttribute ( n.c_str(), v.c_str() );
}


/*
 * Output an entity to an output stream.
 */
std::ostream &
operator << ( std::ostream &out, Entity &e )
{
  Attribute *a;
  AttributeList::iterator iter = e.attributes->iterate ( );

  out << "<" << e.type << "\n";

  while ( iter.hasNext() )
    {
      a = iter.next ( );
      out << "  " << *a << "\n";
    }

  out << "/>\n";

  return out;
}


/*
 * Destructor
 */
Entity::~Entity ( )
{
  delete this->attributes;
}
