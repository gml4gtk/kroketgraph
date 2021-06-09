/*
 * node.cpp
 *
 * Implementation of the Node object's methods.
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

#include <iostream>
#include "../parser/defs.h"
#include "node.h"
#include "graph.h"


/*
 * Constructor
 */
Node::Node ( Graph *g,
             const bool isVirtual )
{
  this->n_id = g->getNextNodeId ( );
  this->id.setNum ( this->n_id);
  this->id.append ( "__node" );
  this->isVirtual = isVirtual;;
  this->active = true;
  this->grid_y = 0;
  this->grid_x = 0;
  this->subgraph_id = 0;
  this->nbChildren = 0;
}


/*
 * Initialization of a node using an Entity object.
 */
void
Node::initFromEntity ( const Entity *e )
{
  const char *attr_v;
  Attribute *a;

  /* load ID */
  attr_v = e->getValueOfAttribute ( ATTR_NODE_ID );

  if ( attr_v != NULL )
    this->id = attr_v; /* NOTE: we do not check for duplicate IDs */

  /* load tags */
  a = e->getAttribute ( ATTR_NODE_TAGS );

  if ( a != NULL )
    {
      QStringList *sl = a->getValueAsListOfString ( );
      this->tags = QStringList ( *sl );
      delete sl;
    }
}


/*
 * Add a child to a node.
 * The node automatically registers itself as a parent of the provided node.
 */
void
Node::addChild ( Node *child )
{
  this->linkChild ( child );
  child->linkParent ( this );
}


/*
 * Remove a child from a node.
 * The node automatically unregisters itself as a parent of the provided node.
 */
void
Node::removeChild ( Node *child )
{
  if ( this->children.contains(child) )
    this->unlinkChild ( child );

  if ( child->parents.contains(this) )
    child->unlinkParent ( this );
}


/*
 * Add a parent to a node.
 * The node automatically registers itself as a child of the provided node.
 */
void
Node::addParent ( Node *parent )
{
  this->linkParent ( parent );
  parent->linkChild ( this );
}


/*
 * Remove a parent from a node.
 * The node automatically unregisters itself as a child of the provided node.
 */
void
Node::removeParent ( Node *parent )
{
  if ( this->parents.contains(parent) )
    this->unlinkParent ( parent );

  if ( parent->children.contains(this) )
    parent->unlinkChild ( this );
}


/*
 * Callback for the activation of a child.
 */
void
Node::childActivated ( Node *child )
{
  if ( this->inactiveChildren.contains(child) )
    qlist_remove ( this->inactiveChildren, child );

  this->children.append ( child );
}


/*
 * Callback for the deactivation of a child.
 */
void
Node::childDeactivated ( Node *child )
{
  if ( this->children.contains(child) )
    qlist_remove ( this->children, child );

  this->inactiveChildren.append ( child );
}


/*
 * Callback for the activation of a parent.
 */
void
Node::parentActivated ( Node *parent )
{
  if ( this->inactiveParents.contains(parent) )
    qlist_remove ( this->inactiveParents, parent );

  this->parents.append ( parent );
}


/*
 * Callback for the deactivation of a parent.
 */
void
Node::parentDeactivated ( Node *parent )
{
  if ( this->parents.contains(parent) )
    qlist_remove ( this->parents, parent );

  this->inactiveParents.append ( parent );
}


/*
 * Set the state of the node.
 */
void
Node::setActive ( const bool active )
{
  /* it is very important to check that the state of the node has changed *
   * otherwise, we may get the active* and inactive* lists corrupted      */
  if ( this->active == active )
    return;

  this->active = active;

  if ( active )
    {
      /*
        std::cerr << "node \'" << this->id.toAscii().constData() << "\' is set active\n";
      */
      foreach ( Node *child, this->children )
        child->parentActivated ( this );

      foreach ( Node *child, this->inactiveChildren )
        child->parentActivated ( this );

      foreach ( Node *parent, this->parents )
        parent->childActivated ( this );

      foreach ( Node *parent, this->inactiveParents )
        parent->childActivated ( this );
    }
  else
    {
      /*
        std::cerr << "node \'" << this->id.toAscii().constData() << "\' is set inactive\n";
      */
      foreach ( Node *child, this->children )
        child->parentDeactivated ( this );

      foreach ( Node *child, this->inactiveChildren )
        child->parentDeactivated ( this );

      foreach ( Node *parent, this->parents )
        parent->childDeactivated ( this );

      foreach ( Node *parent, this->inactiveParents )
        parent->childDeactivated ( this );
    }
}


/*
 * Destructor
 */
Node::~Node ( )
{
}
