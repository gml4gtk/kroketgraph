/*
 * application.h
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

#ifndef __APPLICATION_H__
#define __APPLICATION_H__


#include "ui_kroket.h"
#include "graphviewtab.h"


class HelpDialog;
class HelpInputDialog;
class HelpAboutDialog;

class AppKroket : public QMainWindow
{
  Q_OBJECT

 public:
  AppKroket ( QWidget *parent = 0 );
  ~AppKroket ( );

  QStatusBar *statusDisplay ( ) { return this->ui.statusbar; }

  GraphView *currentGraphView ( )
  {
    if ( this->ui.tabWidget->currentWidget() == NULL )
      return NULL;

    return ((GraphViewTab *) this->ui.tabWidget->currentWidget())->graphView();
  }

  void openFile ( QString );

  void updateUndoActions ( );  /* update undo actions buttons and menu entries */
  void updateActions ( );      /* update actions buttons and menu entries */

 public Q_SLOTS:
  void open ( );

  void undo ( );
  void redo ( );
  void zoomIn ( );
  void zoomOut ( );
  void zoomFit ( );

  void expandAll ( bool autoSync=true );
  void collapseAll ( bool autoSync=true );

  void reset ( );

  void exportImage ( );
  void print ( );

  void help ( );
  void help_input ( );
  void help_about ( );

  void graphviewChanged ( int index );
  void graphviewClosing ( int index );


 private:
  Ui::MainWindow ui;
  HelpDialog *hd;
  HelpInputDialog *hid;
  HelpAboutDialog *had;

};


#endif
