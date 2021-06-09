/*
 * abstractshape.cpp
 *
 * Implementation of the AbstractShape object's methods.
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

#include "parser/defs.h"
#include "abstractshape.h"


/* pen styles */
typedef struct st_pen_style_entry
{
  const char *name;
  Qt::PenStyle style;
} PenStyleEntry ;

static PenStyleEntry penStyles [] = { { "nostroke", Qt::NoPen },
                                      { "solid", Qt::SolidLine },
                                      { "dashes", Qt::DashLine },
                                      { "dots", Qt::DotLine },
                                      { NULL, Qt::NoPen } };

/* brush styles */
typedef struct st_brush_style_entry
{
  const char *name;
  Qt::BrushStyle style;
} BrushStyleEntry ;

static BrushStyleEntry brushStyles [] = { { "nofill", Qt::NoBrush },
                                          { "solid", Qt::SolidPattern },
                                          { "dots", Qt::Dense5Pattern },
                                          { "cross", Qt::CrossPattern },
                                          { NULL, Qt::NoBrush } };

/* text styles */
#define TEXT_STYLE_NORMAL "normal"
#define TEXT_STYLE_BOLD   "bold"
#define TEXT_STYLE_ITALIC "italic"


/*
 * Constructor
 */
AbstractShape::AbstractShape ( )
  : QGraphicsItem()
{
  this->pen.setWidth ( 2 );
  this->penWidth = 2;

  this->text = NULL;

#if QT_VERSION >= 0x040600
  this->setFlag ( QGraphicsItem::ItemSendsGeometryChanges, true );
#endif
}


/*
 * Initialize a shape's pen using an Entity object.
 */
void
AbstractShape::initPenFromEntity ( const Entity *e )
{
  const char *attr_v;
  Attribute *a;

  /* load pen color */
  attr_v = e->getValueOfAttribute ( ATTR_PEN_COLOR );

  if ( attr_v != NULL )
    this->pen.setColor ( QColor(attr_v) );

  /* load pen style */
  attr_v = e->getValueOfAttribute ( ATTR_PEN_STYLE );

  if ( attr_v != NULL )
    {
      PenStyleEntry *pse = penStyles;

      while ( pse->name != NULL )
        {
          if ( strcmp(pse->name,attr_v) == 0 )
            {
              this->pen.setStyle ( pse->style );
              break;
            }

          ++pse;
        }
    }

  /* load pen width */
  a = e->getAttribute ( ATTR_PEN_WIDTH );

  if ( a != NULL )
    this->pen.setWidth ( a->getValueAsInt() );

  this->penWidth = this->pen.width ( );
}


/*
 * Initialize a shape's brush using an Entity object.
 */
void
AbstractShape::initBrushFromEntity ( const Entity *e )
{
  const char *attr_v;

  /* load brush color */
  attr_v = e->getValueOfAttribute ( ATTR_BRUSH_COLOR );

  if ( attr_v != NULL )
    this->brush.setColor ( QColor(attr_v) );

  /* load brush style */
  attr_v = e->getValueOfAttribute ( ATTR_BRUSH_STYLE );

  if ( attr_v != NULL )
    {
      BrushStyleEntry *bse = brushStyles;

      while ( bse->name != NULL )
        {
          if ( strcmp(bse->name,attr_v) == 0 )
            {
              this->brush.setStyle ( bse->style );
              break;
            }

          ++bse;
        }
    }
}


/*
 * Initialize a shape's text using an Entity object.
 */
void
AbstractShape::initTextFromEntity ( const Entity *e )
{
  const char *attr_v;
  Attribute *a;

  /* load font style */
  attr_v = e->getValueOfAttribute ( ATTR_FONT_STYLE );

  if ( attr_v != NULL )
    {
      if ( strcmp(attr_v,TEXT_STYLE_ITALIC) == 0 )
        this->font.setItalic ( true );
      else if ( strcmp(attr_v,TEXT_STYLE_BOLD) == 0 )
        this->font.setBold ( true );
      else
        {
          this->font.setBold ( false );
          this->font.setItalic ( false );
        }
    }

  /* load font size */
  a = e->getAttribute ( ATTR_FONT_SIZE );

  if ( a != NULL )
    this->font.setPointSize ( a->getValueAsInt() );

  /* load text */
  attr_v = e->getValueOfAttribute ( ATTR_TEXT );

  if ( attr_v != NULL )
    {
      QString str = QString::fromAscii ( attr_v );
      str = str.replace ( "\\n", "\n" );
      str = str.replace ( "\\t", "    " );

      if ( this->text != NULL )
        delete this->text;

      this->text = new QGraphicsSimpleTextItem ( str, this );

      /* load font color */
      attr_v = e->getValueOfAttribute ( ATTR_FONT_COLOR );

      if ( attr_v != NULL )
        this->text->setBrush ( QBrush(QColor(attr_v)) );

      this->text->setFont ( this->font );
    }
}


/*
 * Initialize a shape using an other shape.
 */
void
AbstractShape::initFromShape ( const AbstractShape *as )
{
  this->font = as->font;
  this->pen = as->pen;
  this->brush = as->brush;

  if ( as->text != NULL )
    {
      this->text = new QGraphicsSimpleTextItem ( as->text->text(), this );
      this->text->setFont ( this->font );
    }
}


/*
 * Paint the bounding rectangle of a shape.
 */
void
AbstractShape::paintBoundingRect ( QPainter *painter ) const
{
  painter->setBrush ( Qt::NoBrush );
  painter->setPen ( QPen(Qt::red,1) );
  painter->drawRect ( boundingRect() );
}


/*
 * Destructor
 */
AbstractShape::~AbstractShape ( )
{
  /*  this->text is freed by QGraphicsScene  */
}
