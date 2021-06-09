/*
 * graphview.h
 *
 * Declaration of the GraphView class.
 * This class describes a widget able to display graphs.
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

#ifndef __GRAPHVIEW_H__
#define __GRAPHVIEW_H__


#include <QGraphicsView>

#include "graph/graph.h"
#include "abstractgroupshape.h"

class AppKroket;

class GraphView : public QGraphicsView
{
  Q_OBJECT

 public:
  GraphView ( QWidget *parent=NULL );
  ~GraphView ( );

  inline void setApp ( AppKroket *app ) { this->app = app; }
  inline bool hasApp ( ) { return ( this->app != NULL ); }

  inline QString getGraphTitle ( ) const { return this->title; }
  inline QString getShortTitle ( ) const { return this->title + " (" + this->finfo.fileName() + ")"; }
  inline QString getLongTitle ( ) const { return this->title + " (" + this->finfo.absoluteFilePath() + ")"; }

  void loadGraphFromFile ( const char * );          /* open a graph description file */
  inline Graph *getGraph ( ) const { return this->graph; }
  void synchronizeView ( );                         /* synchronize the view with the graph (layout, display, etc.) */
  QImage *getImage ( );                             /* output the view to an image */
  void print ( QPrinter * );                        /* print the view */
  bool focusOn ( QString & );                       /* focus on the specified node */

  void expand ( QString &, bool autoSync=true );    /* expand the specified group */
  void collapse ( QString &, bool autoSync=true );  /* collapse the specified group */

  bool traceFrom ( QString &, bool autoSync=true ); /* trace from the specified node */
  void traceFrom ( AbstractNodeShape *, bool autoSync=true );

  void filterWithTag ( QString &, bool autoSync=true );     /* filter the view with the provided tag (nodes with) */
  void filterWithoutTag ( QString &, bool autoSync=true );  /* filter the view with the provided tag (nodes without) */

  void closeGraph ( );                              /* close the current graph */

  void addUndoCommand ( QUndoCommand * );           /* add an undo command to the commands stack */
  inline QUndoStack *getUndoStack ( ) { return this->undoStack; }

  virtual QSize sizeHint ( ) const;
  QPointF center ( ) const;

  void computeGridHVSpacing ( const QPair<quint32, quint32> & );
  QPair<quint32, quint32> gridSize;
  qreal *gridCellHCenter;
  qreal *gridCellVCenter;
  qreal *gridCellHSize;
  qreal *gridCellVSize;
  inline qreal HSpacing ( ) const { return 75.0; }
  inline qreal VSpacing ( ) const { return 100.0; }

  void undo ( );
  void redo ( );
  void zoomIn ( );
  void zoomOut ( );
  void zoomFit ( );

  void expandAll ( bool autoSync=true );
  void collapseAll ( bool autoSync=true );

  void reset ( );

 public Q_SLOTS:
  void changeViewPos ( qreal ); /* slot for the animations timeline */

 protected:
  virtual void paintEvent ( QPaintEvent * );
  virtual void wheelEvent ( QWheelEvent * );
  virtual void mouseMoveEvent ( QMouseEvent * );
  virtual void mousePressEvent ( QMouseEvent * );
  virtual void mouseReleaseEvent ( QMouseEvent * );

  void adaptEdgesWidth ( );    /* adapt the width of edges according to the zoom factor */


  AppKroket *app;

  QString fname;
  QFileInfo finfo;
  QString title;

  Graph *graph;
  QHash<QString, AbstractGroupShape *> groups;

  QPointF mousePos;

  /* animated focusOn() objects */
  QTimeLine timeline;
  QLineF viewPos;

  /* undo stack */
  QUndoStack *undoStack;

} ;


#endif
