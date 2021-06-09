/*
 * help.h
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

#ifndef __HELP_H__
#define __HELP_H__


#include "ui_help.h"


class HelpDialog : public QDialog
{
  Q_OBJECT

 public:
  HelpDialog ( QWidget *parent = 0 );
  ~HelpDialog ( );

 private:
  Ui::HelpWindow ui;

};


#include "ui_help-input.h"

class HelpInputDialog : public QDialog
{
  Q_OBJECT

 public:
  HelpInputDialog ( QWidget *parent = 0 );
  ~HelpInputDialog ( );

 private:
  Ui::HelpInputWindow ui;

};


#include "ui_help-about.h"

class HelpAboutDialog : public QDialog
{
  Q_OBJECT

 public:
  HelpAboutDialog ( QWidget *parent = 0 );
  ~HelpAboutDialog ( );

 private:
  Ui::HelpAboutWindow ui;

};


#endif
