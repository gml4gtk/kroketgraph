/*
 * graphview.cpp
 *
 * Implementation of the GraphView object's methods.
 *
 * GraphView widget handles the display of a graph.
 *
 * It uses the parser/ and graph/ packages to build a
 * interactive representation of the requested graph.
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

#include "graphviewtab.h"


/*
 * Constructor
 */
GraphViewTab::GraphViewTab ( AppKroket *app, QWidget *parent )
  : QWidget ( parent )
{
  QVBoxLayout *mainLayout = new QVBoxLayout ( );
  this->setLayout ( mainLayout );

  this->app = app;

  /* graph view */
  this->gv = new GraphView ( );
  this->gv->setApp ( app );
  mainLayout->addWidget ( (QWidget *) this->gv );

  /* search bar */
  QWidget *searchbar = new QWidget ( );
  QHBoxLayout *searchbarlayout = new QHBoxLayout ( );
  searchbar->setLayout ( searchbarlayout );
  mainLayout->addWidget ( (QWidget *) searchbar );

  search = new QLineEdit ( );
  searchbarlayout->addWidget ( (QWidget *) this->search );

  btnFocus = new QPushButton (  QIcon(QString::fromUtf8(":/resources/gfx/find.png")), QString::fromUtf8("Focus On") );
  QObject::connect ( btnFocus, SIGNAL(clicked()), this, SLOT(focus()) );
  searchbarlayout->addWidget ( (QWidget *) btnFocus );

  btnTrace = new QPushButton ( QIcon(QString::fromUtf8(":/resources/gfx/home.png")), QString::fromUtf8("Trace From") );
  QObject::connect ( btnTrace, SIGNAL(clicked()), this, SLOT(trace()) );
  searchbarlayout->addWidget ( (QWidget *) btnTrace );

  btnFilter = new QPushButton ( QIcon(QString::fromUtf8(":/resources/gfx/filter.png")), QString::fromUtf8("Filter Tags") );
  QObject::connect ( btnFilter, SIGNAL(clicked()), this, SLOT(filter()) );
  searchbarlayout->addWidget ( (QWidget *) btnFilter );
}


/*
 * Callback for the Focus signal.
 */
void
GraphViewTab::focus ( )
{
  QString nname = this->search->text ( );

  if ( nname.size() > 0 )
    {
      if ( !this->gv->focusOn ( nname ) )
        {
          QString msg;
          msg.sprintf ( "error: \'%s\' could not be found.", qPrintable(nname) );
          this->app->statusDisplay()->showMessage ( msg, 2000 );
        }
    }
}


/*
 * Callback for the Trace signal.
 */
void
GraphViewTab::trace ( )
{
  QString nname = this->search->text ( );

  if ( nname.size() > 0 )
    {
      if ( !this->gv->traceFrom ( nname ) )
        {
          QString msg;
          msg.sprintf ( "error: \'%s\' could not be found.", qPrintable(nname) );
          this->app->statusDisplay()->showMessage ( msg, 2000 );
        }
    }
}


/*
 * Callback for the Filter signal.
 */
void
GraphViewTab::filter ( )
{
  QString tag = this->search->text ( );

  if ( tag.size() > 0 )
    {
      if ( tag[0] == '!' )
        {
          tag = tag.mid ( 1 );
          this->gv->filterWithoutTag ( tag );
        }
      else
        this->gv->filterWithTag ( tag );
    }
}


/*
 * Destructor
 */
GraphViewTab::~GraphViewTab ( )
{
  delete this->gv;
  delete this->search;
  delete this->btnFocus;
  delete this->btnTrace;
  delete this->btnFilter;
}
