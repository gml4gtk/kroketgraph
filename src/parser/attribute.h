/*
 * attribute.h
 *
 * Declaration of the Attribute class.
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

#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__

#include <string>
#include <QtCore>


/* A Douple is a list (tuple) of double */
typedef struct QList<double> Douple;


class Attribute
{

 public:
  Attribute ( const char *, const char *value=NULL );
  ~Attribute ( );

  bool isNamed ( const char * ) const;

  void setValue ( const char * );
  void setValue ( const int );
  void setValue ( const double );
  void setValue ( Douple * );
  void setValue ( QStringList * );
  void setValue ( QList<Douple *> * );

  const char *getValue ( ) const;
  int getValueAsInt ( ) const;
  double getValueAsDouble ( ) const;
  Douple *getValueAsDouple ( ) const;
  QStringList *getValueAsListOfString ( ) const;
  QList<Douple *> *getValueAsVectorOfDouple ( ) const;

  friend std::ostream & operator << ( std::ostream &, Attribute & );

 private:
  std::string name;
  std::string value;

} ;


#endif
