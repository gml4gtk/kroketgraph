/*
 * node.h
 *
 * Declaration of the Node class.
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


#ifndef __NODE_H__
#define __NODE_H__

#include <QtCore>
#include "../parser/entity.h"


#define qlist_remove(l,n) (l.removeAt(l.indexOf(n)))

#ifdef MSWIN
#define index(x,y) strchr(x,y)
#endif

#define NODE_ANCHOR_ATTR_NULL "@"


class Graph;

class Node
{
 public:
  Node ( Graph *, const bool isVirtual=false );
  virtual ~Node ( );

  virtual void initFromEntity ( const Entity * );

  void addChild ( Node * );     /* add a child to a node and register as a parent of it */
  void removeChild ( Node * );  /* remove a child from a node and unregister as a parent of it */
  void addParent ( Node * );    /* add a parent to a node and register as a child of it */
  void removeParent ( Node * ); /* remove a parent from a node and unregister as a child of it */

  inline void linkChild ( Node *c )    { children.append(c); ++nbChildren; }       /* add a child to a node */
  inline void unlinkChild ( Node *c )  { qlist_remove(children,c); --nbChildren; } /* remove a child from a node */
  inline void linkParent ( Node *p )   { parents.append(p);        } /* add a parent to a node */
  inline void unlinkParent ( Node *p ) { qlist_remove(parents,p);  } /* remove a parent from a node */

  inline bool hasParents ( ) const { return (parents.size() != 0); }    /* does the node have parents ? */
  inline bool hasChildren ( ) const { return (children.size() != 0); }  /* does the node have children ? */

  inline bool hasParent ( Node *p ) { return parents.contains(p); }  /* does the node has the given node for parent ? */
  inline bool hasChild ( Node *c )  { return children.contains(c); } /* does the node has the given node for child ? */

  void setActive ( const bool );                                /* set the state of a node */
  inline bool isActive ( ) const { return this->active; }       /* read the state of a node */
  void childActivated ( Node * );                               /* callback for the changing of a node's child state */
  void childDeactivated ( Node * );                             /* callback for the changing of a node's child state */
  void parentActivated ( Node * );                              /* callback for the changing of a node's parent state */
  void parentDeactivated ( Node * );                            /* callback for the changing of a node's parent state */


  /* Extract the node name from a port name */
  static inline QString extractNodeName ( const char *port_name )
    {
      const char *sep = index ( port_name, '/' );

      if ( sep == NULL )
        return QString::fromLatin1 ( port_name ); // QString::fromAscii ( port_name );
      else
        return QString::fromLatin1 ( port_name, sep-port_name ); // QString::fromAscii ( port_name, sep-port_name );
    }

  /* Extract the anchor region name from a port name (without extra attribute) */
  static inline QString extractAnchorRegion ( const char *port_name )
    {
      const char *sep1 = index ( port_name, '/' );

      if ( sep1 == NULL )
        return "default";
      else
        {
          const char *sep2 = index ( sep1+1, '/' );

          if ( sep2 != NULL )
            return QString::fromLatin1 ( sep1+1, sep2-sep1-1 ); // QString::fromAscii ( sep1+1, sep2-sep1-1 );
          else
            return QString::fromLatin1 ( sep1+1 ); // QString::fromAscii ( sep1+1 );
        }
    }

  /* Extract the extra anchor name attribute from a port name */
  static inline QString extractAnchorAttr ( const char *port_name )
    {
      const char *sep1 = index ( port_name, '/' );

      if ( sep1 == NULL )
        return NODE_ANCHOR_ATTR_NULL;
      else
        {
          const char *sep2 = index ( sep1, '/' );

          if ( sep2 != NULL )
            return QString::fromLatin1 ( sep2+1 ); // QString::fromAscii ( sep2+1 );
          else
            return NODE_ANCHOR_ATTR_NULL;
        }
    }


 public:
  QString id;                    /* text ID    */
  quint32 n_id;                  /* numeric ID */

  quint32 grid_x;                /* X coordinate in the grid (layers organization) */
  quint32 grid_y;                /* Y coordinate in the grid (layers organization) */

  quint32 subgraph_id;           /* id of the subgraph the node belongs to */

  quint32 tag;                   /* tag (used to hold temporary data) */
  double coef;                   /* storage for some floating tag/coefficient (used for layout) */

  bool isVirtual;                /* whether the node is virtual or not */

  QList<Node *> parents;         /* node's parents  */
  QList<Node *> children;        /* node's children */
  QList<Node *> inactiveParents;
  QList<Node *> inactiveChildren;

  int nbChildren;                /* the number of children, whereever there are active or inactive */

  bool active;                   /* state of the node */

  QStringList tags;              /* tags associated with a node */

} ;

#ifdef MSWIN
#undef index
#endif

#endif
