/*
 * abstractnodeshape.h
 *
 * Declaration of the AbstractNodeShape class.
 * This class is abstract.
 * It provides the basic behaviour of nodes displayed.
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

#ifndef __ABSTRACTNODESHAPE_H__
#define __ABSTRACTNODESHAPE_H__


#include "abstractshape.h"
#include "graph/node.h"
#include "parser/entity.h"


class AbstractEdgeShape;
class AbstractGroupShape;
class AbstractAnchor;
class GraphView;
class Port;

class AbstractNodeShape : public AbstractShape, public Node
{
 public:
  AbstractNodeShape ( GraphView * );
  virtual ~AbstractNodeShape ( );

  /*
   * Initialize a node shape using an Entity object.
   */
  virtual void initFromEntity ( const Entity * );

  /*
   * Pack the shape according to its content (text).
   */
  virtual void pack ( ) = 0;

  /*
   * Set the position of an AbstractNodeShape (overloading QGraphicsItem).
   */
  virtual void setPos ( qreal, qreal );
  virtual void setPos ( const QPointF & );

  /*
   * Center the node on the provided coordinates.
   */
  virtual void centerOn ( qreal, qreal );
  virtual void centerOn ( const QPointF & );

  /*
   * Move the node to the given position using an animation.
   */
  virtual void animateSetPos ( qreal, qreal );
  virtual void animateSetPos ( const QPointF & );

  virtual void animateCenterOn ( qreal, qreal );
  virtual void animateCenterOn ( const QPointF & );

  /*
   * Get the final position of an AbstractNodeShape.
   */
  inline QPointF finalPos ( ) const { return this->anim.posAt(1.0); }

  /*
   * Compute the coordinates of the center of an AbstractNodeShape.
   */
  virtual QPointF center ( );
  virtual QPointF finalCenter ( );

  /*
   * Register an anchor with the provided port name.
   */
  virtual void addLoopingEdge ( AbstractEdgeShape * );

  /*
   * Register an anchor (with the provided port name).
   */
  virtual void registerAnchor ( AbstractAnchor *, const char * );
  virtual void registerAnchor ( AbstractAnchor * );

  /*
   * Update the anchors of an AbstractNodeShape.
   */
  virtual void updateAnchors ( );

  /*
   * Group handling
   */
  inline virtual void setGroup ( AbstractGroupShape *g ) { this->group = g; }
  inline bool hasGroup ( ) const { return (this->group != NULL); }

  /*
   * Visibility
   */
  void setVisible ( bool );
  void show ( );
  void hide ( );

  /*
   * Hide and deactivate an AbstractNodeShape and all its adjacent nodes below.
   */
  void hideAndDeactivate ( );

  /*
   * Does the node has an active group ? (in other words, is the node is overriden ?)
   * If the group is collapsed, it means that either the group itself or its parent group
   * is active and overrides the node.
   */
  virtual bool hasActiveGroup ( ) const;

  /*
   * Various callbacks
   */
  virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * );
  virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * );
  virtual QVariant itemChange ( GraphicsItemChange, const QVariant & );

  /*
   * Drawing related functions
   */
  virtual QSizeF size ( ) const = 0;
  virtual QRectF boundingRect ( ) const = 0;
  virtual qreal getWidth ( ) const = 0;
  virtual qreal getHeight ( ) const = 0;
  virtual QPainterPath shape ( ) const = 0;
  virtual void paint ( QPainter *, const QStyleOptionGraphicsItem *, QWidget * ) = 0;

 protected:
  /* add an anchor to the anchors table */
  void addAnchor ( AbstractAnchor * );

  /* remove an anchor from the anchors table */
  void removeAnchor ( AbstractAnchor * );


 public:
  GraphView *graphView;                              /* graph view, owner of the node */
  QHash<QString, QList<Port *> *> ports;             /* ports */
  QList<AbstractEdgeShape *> edges;                  /* attached edges */
  QList<AbstractEdgeShape *> loops;                  /* attached looping edges */
  QHash<AbstractEdgeShape *, QPair<Port *, Port *> > l_ports; /* ports of the loops */

  AbstractGroupShape *group;                         /* parent group */


 protected:
  QTimeLine timeline;
  QGraphicsItemAnimation anim;

} ;


#endif
