/*
 * layering-lazy.h
 *
 * Declaration of the LayeringLazy class.
 * It allows the layering (and the correction of the layering) of a graph.
 * The algorithm is described in layeringlazy.cpp
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


#ifndef __LAYERING_LAZY_H__
#define __LAYERING_LAZY_H__

#include <QtCore>
#include "node.h"


class LayeringLazy
{
 public:
  static void applyToLayers ( QList<QList<Node *> > & );
  static void applyToNodes ( QList<Node *> & );
} ;


#endif
