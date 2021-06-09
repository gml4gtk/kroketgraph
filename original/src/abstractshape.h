/*
 * abstractshape.h
 *
 * Declaration of the AbstractShape class.
 * This class is abstract.
 * It provides the basic behaviour of items displayed.
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

#ifndef __ABSTRACTSHAPE_H__
#define __ABSTRACTSHAPE_H__


#include <QtGui>

#include "parser/entity.h"


#define DATA_TYPE  0xd0d0   /* key for the storage of the type of the shape (node, group, edge, ...) */

#define TYPE_NODE  0xd0d0
#define TYPE_EDGE  0xd0d1
#define TYPE_GROUP 0xd0d2

#define IS_NODE(s)  ( s->data(DATA_TYPE).toInt() == TYPE_NODE )
#define IS_EDGE(s)  ( s->data(DATA_TYPE).toInt() == TYPE_EDGE )
#define IS_GROUP(s) ( s->data(DATA_TYPE).toInt() == TYPE_GROUP )


class AbstractShape : public QGraphicsItem
{
 public:
  AbstractShape ( );
  virtual ~AbstractShape ( );

  /*
   * Initialize a shape using an Entity object.
   */
  virtual inline void initFromEntity ( const Entity *e )
    {
      initPenFromEntity ( e );
      initBrushFromEntity ( e );
      initTextFromEntity ( e );
    }

  void initPenFromEntity ( const Entity * );
  void initBrushFromEntity ( const Entity * );
  void initTextFromEntity ( const Entity * );

  /*
   * Initialize a shape using an other shape.
   */
  void initFromShape ( const AbstractShape * );

  /*
   * Does a shape has a text attached ?
   */
  inline bool hasText ( ) const
    { return (text!=NULL); }

  inline QGraphicsSimpleTextItem * textItem ( ) const
    { return text; }

  /*
   * Modify the width of the pen.
   */
  //inline void adaptPenWidth ( const qreal ratio, const qreal max ) { this->pen.setWidth(((int)( max>(ratio*penWidth) ? ratio*penWidth : max ))); }
  inline void adaptPenWidth ( const qreal max ) { this->pen.setWidth(((int)( max>penWidth ? max:penWidth ))); }
  inline void resetPenWidth ( ) { this->pen.setWidth(penWidth); }

  /*
   * Drawing related functions
   */
  virtual QRectF boundingRect ( ) const = 0;
  virtual QPainterPath shape ( ) const = 0;
  virtual void paint ( QPainter *, const QStyleOptionGraphicsItem *, QWidget * ) = 0;

  void paintBoundingRect ( QPainter * ) const;


  int penWidth;
  QPen pen;

  QBrush brush;

 protected:
  QFont font;
  QGraphicsSimpleTextItem *text;

} ;


#endif
