/*
 * masked_isspace.h
 *
 * This header defines a macro that uses the table defined in masked_isspace.cpp
 * to determine whether a character is a whitespace or not.
 *
 * Compared to the usual isspace() function, it offers a very nice speed up.
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


#ifndef __MASKED_ISSPACE__
#define __MASKED_ISSPACE__

extern char mask_space [ ];

#define masked_isspace(c) (mask_space[(int)c])
#define _isspace masked_isspace


#endif
