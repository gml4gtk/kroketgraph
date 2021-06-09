/*
 * help.cpp
 *
 * This code handles the help windows' GUI.
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
#include "help.h"


/*
 * Constructor for Help Dialog
 */
HelpDialog::HelpDialog ( QWidget *parent )
    : QDialog(parent)
{
    ui.setupUi ( this );
    this->hide ( );
}


/*
 * Destructor for Help Dialog
 */
HelpDialog::~HelpDialog ( )
{
}


/*
 * Constructor for Kroket Input Help Dialog
 */
HelpInputDialog::HelpInputDialog ( QWidget *parent )
    : QDialog(parent)
{
    ui.setupUi ( this );
    this->hide ( );
}


/*
 * Destructor for Kroket Input Help Dialog
 */
HelpInputDialog::~HelpInputDialog ( )
{
}


/*
 * Constructor for Kroket About Dialog
 */
HelpAboutDialog::HelpAboutDialog ( QWidget *parent )
    : QDialog(parent)
{
  QString version;
  ui.setupUi ( this );
  QTextStream(&version) << "Kroket " << KROKET_VER_MAJ << "." << KROKET_VER_MIN << "." << KROKET_VER_PAT;
  ui.labelVersion->setText ( version );
  this->hide ( );
}


/*
 * Destructor for Kroket About Dialog
 */
HelpAboutDialog::~HelpAboutDialog ( )
{
}
