/*
 * attribute.cpp
 *
 * This file implements the methods of the Attribute object.
 * The purpose of this object is mainly to provide conversion
 * functions from a string to any other useful type.
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

#include <cassert>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>

#include "masked_isspace.h"
#include "attribute.h"


/*
 * Constructor
 */
Attribute::Attribute ( const char *name,
                       const char *value )
{
  assert ( name != NULL );

  this->name.assign ( name );

  if ( value != NULL )
    this->value.assign ( value );
  else
    this->value.assign ( "" );
}


/*
 * Compare the name of an attribute with the provided string.
 * Returns true if names match, false either.
 */
bool
Attribute::isNamed ( const char *str ) const
{
  return ( this->name.compare(str) == 0 );
}


/*
 * Set/Overwrite the value of an attribute.
 */
void
Attribute::setValue ( const char *str )
{
  this->value.assign ( str );
}


/*
 * Set/Overwrite the value of an attribute from an integer.
 */
void
Attribute::setValue ( const int v )
{
  std::ostringstream oss;

  oss << v;
  this->value.assign ( oss.str() );
}


/*
 * Set/Overwrite the value of an attribute from a double.
 */
void
Attribute::setValue ( const double v )
{
  std::ostringstream oss;

  oss << v;
  this->value.assign ( oss.str() );
}


/*
 * Set/Overwrite the value of an attribute from a douple.
 */
void
Attribute::setValue ( Douple *d )
{
  std::ostringstream oss;
  QListIterator<double> iter ( *d );

  oss << "(";

  if ( iter.hasNext() )
    {
      while ( true )
        {
          oss << iter.next();

          if ( !iter.hasNext() )
            break;

          oss << ", ";
        }
    }

  oss << ")";

  this->value.assign ( oss.str() );
}


/*
 * Set/Overwrite the value of an attribute from a vector of string.
 */
void
Attribute::setValue ( QStringList *v )
{
  std::ostringstream oss;
  QStringListIterator iter ( *v );

  oss << "[";

  if ( iter.hasNext() )
    {
      while ( true )
        {
          oss << qPrintable(iter.next());

          if ( !iter.hasNext() )
            break;

          oss << ", ";
        }
    }

  oss << "]";

  this->value.assign ( oss.str() );
}


/*
 * Set/Overwrite the value of an attribute from a vector of douple.
 */
void
Attribute::setValue ( QList<Douple *> *v )
{
  std::ostringstream oss;
  QListIterator<Douple *> iter ( *v );

  oss << "[";

  if ( iter.hasNext() )
    {
      while ( true )
        {
          oss << "(";

          QListIterator<double> iter_double ( *(iter.next()) );

          if ( iter_double.hasNext() )
            {
              while ( true )
                {
                  oss << iter_double.next();

                  if ( !iter_double.hasNext() )
                    break;

                  oss << ", ";
                }
            }

          oss << ")";

          if ( !iter.hasNext() )
            break;

          oss << ", ";
        }
    }

  oss << "]";

  this->value.assign ( oss.str() );
}


/*
 * Get the attribute's value.
 */
const char *
Attribute::getValue ( ) const
{
  return this->value.c_str();
}


/*
 * Convert the attribute's value into an integer.
 */
int
Attribute::getValueAsInt ( ) const
{
  int v;
  std::istringstream iss ( this->value );

  if ( (iss >> std::dec >> v).fail() )
    return 0;

  return v;
}


/*
 * Convert the attribute's value into a double.
 */
double
Attribute::getValueAsDouble ( ) const
{
  double v;

  std::istringstream iss ( this->value );

  if ( (iss >> std::dec >> v).fail() )
    return 0.0;

  return v;
}


/*
 * Search for the next separator.
 * This function returns the number of bytes read/skipped.
 */
std::string::size_type
find_next_separator ( const std::string &str,
                      const int start )
{
  const char *c = str.c_str ( );
  const unsigned int str_len = str.length ( );
  unsigned int i;
  bool parentheses = false;

  for ( i=start; i<str_len; ++i )
    {
      if ( c[i] == '(' )
        parentheses = true;
      else if ( parentheses )
        {
          if ( c[i] == ')' )
            parentheses = false;
        }
      else if ( c[i] == ',' )
        break;
    }

  if ( i == str_len )
    return std::string::npos;

  return i;
}


/*
 * Read an element from a string representing a sequence (tuple or list).
 * This function returns the number of bytes read/skipped.
 */
inline static int
get_next_sequence_elem ( const std::string &str,
                         const int start,
                         std::string *elem )
{
  std::string::size_type separator;
  std::string raw_elem;

  /* we get the full element */
  separator = find_next_separator ( str, start );

  if ( separator == std::string::npos )
    separator = str.length() - 1; /* if no separator, then we read until the end of the string */

  raw_elem = str.substr ( start, separator-start );
  const unsigned int raw_elem_len = raw_elem.length ( );

  if ( raw_elem_len == 0 )
    {
      elem->assign ( "" );
      return 0;
    }

  /* we remove useless whitespaces */
  int i, j;

  for ( i=0; i<(int)raw_elem_len; ++i )
    if ( !_isspace(raw_elem[i]) )
      break;

  for ( j=raw_elem_len-1; j>i; --j )
    if ( !_isspace(raw_elem[j]) )
      break;

  elem->assign ( raw_elem.substr(i,j-i+1) );

  return (separator-start+1); /* +1 so we skip the separator */
}


/*
 * Returns true if the provided string represents a list, false either.
 */
inline static bool
is_list ( const std::string &str )
{
  return ( ( str[0] == '[' ) && ( str[str.length()-1] == ']' ) );
}


/*
 * Returns true if the provided string represents a tuple, false either.
 */
inline static bool
is_tuple ( const std::string &str )
{
  return ( ( str[0] == '(' ) && ( str[str.length()-1] == ')' ) );
}


/*
 * Returns a douple built from the provided string.
 */
inline static Douple *
build_douple_from_string ( const std::string &str )
{
  Douple *d = new Douple ( );
          
  if ( is_tuple(str) )
    {
      int i=1; /* skip the '(' char */
      int len;
      double v;

      std::string *elem = new std::string ( );

      while ( true )
        {
          len = get_next_sequence_elem ( str, i, elem );

          if ( len == 0 )
            break;

          i += len;

          std::istringstream iss ( *elem );
          if ( (iss >> std::dec >> v).fail() )
            v = 0.0;

          d->append ( v );
        }

      delete elem;
    }

  return d;
}


/*
 * Convert the attribute's value into a douple.
 */
Douple *
Attribute::getValueAsDouple ( ) const
{
  return build_douple_from_string ( this->value );
}


/*
 * Convert the attribute's value into a vector of string.
 */
QStringList *
Attribute::getValueAsListOfString ( ) const
{
  QStringList *l = new QStringList ( );

  if ( is_list(this->value) )
    {
      int i=1; /* skip the '[' char */
      int len;

      std::string *elem = new std::string ( );

      while ( true )
        {
          len = get_next_sequence_elem ( this->value, i, elem );

          if ( len == 0 )
            break;

          i += len;

          l->append ( QString::fromStdString(*elem) );
        }

      delete elem;
    }

  return l;
}


/*
 * Convert the attribute's value into a vector of douple.
 */
QList<Douple *> *
Attribute::getValueAsVectorOfDouple ( ) const
{
  QList<Douple *> *v = new QList<Douple *> ( );

  if ( is_list(this->value) )
    {
      int i=1; /* skip the '[' char */
      int len;

      std::string *elem = new std::string ( );
      Douple *d;

      while ( true )
        {
          len = get_next_sequence_elem ( this->value, i, elem );

          if ( len == 0 )
            break;

          i += len;

          d = build_douple_from_string ( *elem );
          v->append ( d );
        }

      delete elem;
    }

  return v;
}


/*
 * Output an attribute to an output stream.
 */
std::ostream &
operator << ( std::ostream &out,
              Attribute &a )
{
  out << a.name << "=\"" << a.value << "\"";
  return out;
}


/*
 * Destructor
 */
Attribute::~Attribute ( )
{
}
