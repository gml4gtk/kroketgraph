/*
 * cmd-expand.h
 *
 * Declaration of the CmdExpand class.
 * This class describes an expand command (used by the undo system).
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

#ifndef __CMD_EXPAND_H__
#define __CMD_EXPAND_H__


#include <QUndoCommand>

#include "abstractgroupshape.h"


class CmdExpand : public QUndoCommand
{
 public:
  CmdExpand ( AbstractGroupShape *ags ) : group(ags) { }

  virtual void undo ( ) { group->collapse(); }
  virtual void redo ( ) { group->expand(); }


 protected:
  AbstractGroupShape *group;

} ;


#endif
