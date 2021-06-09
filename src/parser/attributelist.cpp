/*
 * attributelist.cpp
 *
 * Implementation of the AttributeList object's methods.
 *
 * Please note that there is no check for duplicate elements.
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

#include "attributelist.h"


/*
 * Constructor
 */
AttributeList::AttributeList ( )
{
  this->list[0] = NULL;
  this->list[1] = NULL;
  this->list[2] = NULL;
  this->list[3] = NULL;
  this->list[4] = NULL;
  this->list[5] = NULL;
  this->list[6] = NULL;
  this->list[7] = NULL;
}


/*
 * Add a new attribute.
 * Returns a pointer to the added attribute or NULL if an error occured.
 */
Attribute *
AttributeList::add ( const char *name,
                     const char *value )
{
  attr_chain *ac = new attr_chain;
  attr_chain **l = &this->list[name[0] & 0x07];

  ac->attr = new Attribute ( name, value );
  ac->next = *l;
  *l = ac;

  return ac->attr;
}


/*
 * Access to an attribute.
 * This function should be used parsimionously as it is slow.
 * Returns a pointer to the requested attribute or NULL if not found.
 */
Attribute *
AttributeList::get ( const char *req ) const
{
  attr_chain *ac = this->list[req[0] & 0x07];

  while ( ac != NULL )
    {
      if ( ac->attr->isNamed(req) )
        return ac->attr;

      ac = ac->next;
    }

  return NULL;
}


/*
 * Remove an attribute.
 * If the list contains any duplicate element, only one will be removed.
 */
void
AttributeList::remove ( const char *req )
{
  attr_chain **prev = &this->list[req[0] & 0x07];
  attr_chain *ac = *prev;

  while ( ac != NULL )
    {
      if ( ac->attr->isNamed(req) )
        {
          (*prev)->next = ac->next;

          delete ac->attr;
          delete ac;
          return;
        }

      ac = ac->next;
    }
}


/*
 * Clear the attributes list.
 */
void
AttributeList::clear ( )
{
  int i;
  attr_chain *tmp;
  attr_chain *ac;

  for ( i=0; i<8; ++i )
    {
      ac = this->list[0];

      while ( ac != NULL )
        {
          tmp = ac;
          ac = ac->next;

          delete tmp->attr;
          delete tmp;
        }

      this->list[i] = NULL;
    }
}


/*
 * Destructor
 */
AttributeList::~AttributeList ( )
{
  this->clear ( );
}
