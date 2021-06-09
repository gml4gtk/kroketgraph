/*
 * group-simple.h
 *
 * Declaration of the GroupSimple class.
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

#ifndef __GROUP_SIMPLE_H__
#define __GROUP_SIMPLE_H__


#include "abstractgroupshape.h"
#include "parser/entity.h"


class GroupSimple : public AbstractGroupShape
{
 public:
  GroupSimple ( GraphView * );
  virtual ~GroupSimple ( );

  /*
   * Initialize a group using an Entity object.
   */
  virtual void initFromEntity ( const Entity * );

  /*
   * Pack the group according to its content.
   */
  virtual void pack ( );

  /*
   * Drawing related functions.
   */
  virtual QRectF boundingRect ( ) const;
  virtual QPainterPath shape ( ) const;
  virtual void paint ( QPainter *, const QStyleOptionGraphicsItem *, QWidget *w=NULL );

} ;


#endif
