/*
 * application.cpp
 *
 * This code handles the application's GUI.
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

#include <QtGui>
#include "application.h"
#include "help.h"


/*
 * Constructor
 */
AppKroket::AppKroket ( QWidget *parent )
    : QMainWindow(parent)
{
    ui.setupUi ( this );

    QObject::connect ( ui.actionOpen, SIGNAL(triggered()), this, SLOT(open()) );
    QObject::connect ( ui.actionExportImage, SIGNAL(triggered()), this, SLOT(exportImage()) );
    QObject::connect ( ui.actionPrint, SIGNAL(triggered()), this, SLOT(print()) );

    QObject::connect ( ui.actionUndo, SIGNAL(triggered()), this, SLOT(undo()) );
    QObject::connect ( ui.actionRedo, SIGNAL(triggered()), this, SLOT(redo()) );

    QObject::connect ( ui.actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()) );
    QObject::connect ( ui.actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()) );
    QObject::connect ( ui.actionZoomFit, SIGNAL(triggered()), this, SLOT(zoomFit()) );

    QObject::connect ( ui.actionExpandAll, SIGNAL(triggered()), this, SLOT(expandAll()) );
    QObject::connect ( ui.actionCollapseAll, SIGNAL(triggered()), this, SLOT(collapseAll()) );

    QObject::connect ( ui.actionReset, SIGNAL(triggered()), this, SLOT(reset()) );

    QObject::connect ( ui.actionHelp, SIGNAL(triggered()), this, SLOT(help()) );
    QObject::connect ( ui.actionHelpInput, SIGNAL(triggered()), this, SLOT(help_input()) );
    QObject::connect ( ui.actionHelpAbout, SIGNAL(triggered()), this, SLOT(help_about()) );

    this->hd = new HelpDialog ( this );
    this->hid = new HelpInputDialog ( this );
    this->had = new HelpAboutDialog ( this );

    /* main tab widget */
    ui.tabWidget->clear ( );

    QObject::connect ( ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(graphviewChanged(int)) );
    QObject::connect ( ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(graphviewClosing(int)) );

    this->updateActions ( );
}


/*
 * openFile
 */
void
AppKroket::openFile ( QString fname )
{
  GraphViewTab *gvt = new GraphViewTab ( this );
  gvt->graphView()->loadGraphFromFile ( fname.toAscii() );
  gvt->graphView()->synchronizeView ( );
  ui.tabWidget->setCurrentIndex ( ui.tabWidget->addTab((QWidget *)gvt,gvt->graphView()->getShortTitle()) );
}


/*
 * Callback for the Open signal.
 */
void
AppKroket::open ( )
{
  QStringList flist = QFileDialog::getOpenFileNames ( this, "Open Kroket File", "", "Krokets (*.kk)" );
  QStringList::Iterator fname;

  for ( fname=flist.begin(); fname!=flist.end(); ++fname )
    this->openFile ( *fname );

  this->updateActions ( );
}



/*
 * Callback for the Undo signal.
 */
void
AppKroket::undo ( )
{
  this->currentGraphView()->undo ( );
}



/*
 * Callback for the Redo signal.
 */
void
AppKroket::redo ( )
{
  this->currentGraphView()->redo ( );
}



/*
 * Callback for the ZoomIn signal.
 */
void
AppKroket::zoomIn ( )
{
  this->currentGraphView()->zoomIn ( );
}


/*
 * Callback for the ZoomOut signal.
 */
void
AppKroket::zoomOut ( )
{
  this->currentGraphView()->zoomOut ( );
}



/*
 * Callback for the ZoomFit signal.
 */
void
AppKroket::zoomFit ( )
{
  this->currentGraphView()->zoomFit ( );
}



/*
 * Callback for the ExpandAll signal.
 */
void
AppKroket::expandAll ( bool autoSync )
{
  this->currentGraphView()->expandAll ( autoSync );
}



/*
 * Callback for the CollapseAll signal.
 */
void
AppKroket::collapseAll ( bool autoSync )
{
  this->currentGraphView()->collapseAll ( autoSync );
}


/*
 * Callback for the Reset signal.
 */
void
AppKroket::reset ( )
{
  this->currentGraphView()->reset ( );
}


/*
 * Callback for the Export Image signal.
 */
void
AppKroket::exportImage ( )
{
  GraphView *gv = this->currentGraphView ( );
  QString fname = QFileDialog::getSaveFileName ( this, "Save Image", "", "Images (*.png)" );

  if ( fname.size() > 0 )
    {
      QImage *img;

      img = gv->getImage ( );
      img->save ( fname, "PNG" );
      delete img;

      QString msg;
      msg.sprintf ( "\'%s\' saved.", qPrintable(fname) );
      ui.statusbar->showMessage ( msg, 2000 );
    }
}


/*
 * Callback for the Print signal.
 */
void
AppKroket::print ( )
{
  GraphView *gv = this->currentGraphView ( );
  QPrinter *printer = new QPrinter ( );
  QPrintDialog *pd = new QPrintDialog ( printer, this );

  if ( pd->exec() == QDialog::Accepted )
    {
      gv->print ( printer );

      QString msg ( "Document printed." );
      ui.statusbar->showMessage ( msg, 2000 );
    }

  delete pd;
  delete printer;
}


/*
 * Callback for the Help signal.
 */
void
AppKroket::help ( )
{
  this->hd->show ( );
}


/*
 * Callback for the HelpInput signal.
 */
void
AppKroket::help_input ( )
{
  this->hid->show ( );
}


/*
 * Callback for the HelpAbout signal.
 */
void
AppKroket::help_about ( )
{
  this->had->show ( );
}


/*
 * Callback for the CurrentTab Changed signal.
 */
void
AppKroket::graphviewChanged ( int index )
{
  GraphView *gv;

  Q_UNUSED ( index );
  this->updateActions ( );

  gv = this->currentGraphView ( );

  if ( gv == NULL )
    this->setWindowTitle ( "Kroket" );
  else
    this->setWindowTitle ( "Kroket - " + this->currentGraphView()->getLongTitle() );
}


/*
 * Callback for the CurrentTab Closing signal.
 */
void
AppKroket::graphviewClosing ( int index )
{
  Q_UNUSED ( index );
  ui.tabWidget->removeTab ( index );
  this->updateActions ( );
}


/*
 * Update undo actions buttons and menu entries.
 */
void
AppKroket::updateUndoActions ( )
{
  GraphView *gv = this->currentGraphView ( );

  if ( gv == NULL )
    {
      ui.actionUndo->setEnabled ( false );
      ui.actionRedo->setEnabled ( false );
      return;
    }

  if ( gv->getUndoStack()->canUndo() )
    ui.actionUndo->setEnabled ( true );
  else
    ui.actionUndo->setEnabled ( false );

  if ( gv->getUndoStack()->canRedo() )
    ui.actionRedo->setEnabled ( true );
  else
    ui.actionRedo->setEnabled ( false );
}


/*
 * Update actions buttons and menu entries.
 */
void
AppKroket::updateActions ( )
{
  GraphView *gv = this->currentGraphView ( );
  bool enabled = true;

  this->updateUndoActions ( );

  if ( ( gv == NULL ) || ( gv->getGraph() == NULL ) )
    enabled = false;

  ui.actionExportImage->setEnabled ( enabled );
  ui.actionPrint->setEnabled ( enabled );
  ui.actionZoomIn->setEnabled ( enabled );
  ui.actionZoomOut->setEnabled ( enabled );
  ui.actionZoomFit->setEnabled ( enabled );
  ui.actionExpandAll->setEnabled ( enabled );
  ui.actionCollapseAll->setEnabled ( enabled );
  ui.actionReset->setEnabled ( enabled );
}


/*
 * Destructor
 */
AppKroket::~AppKroket ( )
{
  delete this->hd;
  delete this->had;
  delete this->hid;
}
