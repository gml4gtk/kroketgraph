/*
 * abstractgroupshape.h
 *
 * Declaration of the AbstractGroupShape class.
 * This class is abstract.
 * It provides the basic behaviour of groups displayed.
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

#ifndef __ABSTRACTGROUPSHAPE_H__
#define __ABSTRACTGROUPSHAPE_H__


#include "abstractshape.h"
#include "abstractnodeshape.h"
#include "parser/entity.h"


typedef enum { GRP_EXPANDED, GRP_COLLAPSED } GrpMode ;

#define GRP_NODE_SHAPE_ELLIPSE_TXT   "ellipse"
#define GRP_NODE_SHAPE_RECTANGLE_TXT "rectangle"
typedef enum { GRP_NODE_SHAPE_RECTANGLE, GRP_NODE_SHAPE_ELLIPSE } GrpNodeShape ;


class AbstractGroupShape : public AbstractNodeShape
{
 public:
  AbstractGroupShape ( GraphView * );
  virtual ~AbstractGroupShape ( );

  /*
   * Initialize a group shape using an Entity object.
   */
  virtual void initFromEntity ( const Entity * );

  /*
   * Add a shape to an AbstractGroupShape.
   */  
  virtual void addShape ( AbstractNodeShape * );
  virtual void insertedSubNode ( AbstractNodeShape * );
  virtual void addGroupShape ( AbstractGroupShape * );
  virtual void insertedSubGroup ( AbstractGroupShape *, qreal );

  /*
   * Does an AbstractGroupShape contains the provided shape.
   */
  virtual bool containsShape ( AbstractNodeShape * );

  /*
   * Freeze the content of a group (generate group's outgoing edges)
   */
  virtual QList<AbstractEdgeShape *> *freezeContent ( );

  /*
   * Pack the group shape according to its content.
   */
  virtual void pack ( );
  
  /*
   * Update the anchors.
   */
  virtual void updateAnchors ( );

  /*
   * Expand/Collapse a group.
   */
  void expand ( bool showAll=true );
  void collapse ( );
  inline bool isCollapsed ( ) const { return (this->mode == GRP_COLLAPSED); }
  inline bool isExpanded ( ) const { return (this->mode == GRP_EXPANDED); }

  /*
   * Switch the mode of the group.
   */
  void switchGrpMode ( );

  /*
   * Does the node has an active group ? (in other words, is the node is overriden ?)
   * If the group is collapsed, it means that either the group itself or its parent group
   * is active and overrides the node.
   */
  virtual bool hasActiveGroup ( ) const;

  /*
   * Callbacks for mouse clicks.
   */
  void mousePressEvent ( QGraphicsSceneMouseEvent * );
  void mouseReleaseEvent ( QGraphicsSceneMouseEvent * );

  /*
   * Drawing related functions
   */
  virtual QSizeF size ( ) const { return QSizeF ( width, height ); }
  virtual QRectF boundingRect ( ) const = 0;
  inline virtual qreal getWidth ( ) const { return width; }
  inline virtual qreal getHeight ( ) const { return height; }
  virtual QPainterPath shape ( ) const = 0;
  virtual void paint ( QPainter *, const QStyleOptionGraphicsItem *, QWidget * ) = 0;

 private:
  int nbActive ( ) const;    /* Count the number of active elements currently in the group */
  void joinGroup ( AbstractEdgeShape *, AbstractGroupShape *, QList<AbstractEdgeShape *> * ); /* link two groups */
  void hideContent ( );      /* Hide the content of a group and propagate the signal to its subgroups */
  void subgroupExpanded ( ); /* Callback used by a subgroup to tell its group that it just expanded */


 public:
  QList<AbstractNodeShape *> content;    /* content of the group */
  QList<AbstractGroupShape *> subgroups; /* subgroups are also stored here */

  QList<AbstractEdgeShape *> internalEdges; /* internal edges of a group */
  QList<AbstractEdgeShape *> externalEdges; /* external edges of a group (those edges are connected to the content of a group) */
  /* the edges used when the group is in node mode are stored in the 'edges' attribute of the AbstractNodeShape class */

  GrpMode mode;
  GrpNodeShape n_shape;

  QPainterPath contour;
  qreal width, height;
  QPointF txtPos; /* position of the text in collapsed mode */

} ;


#endif
