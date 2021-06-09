/*
 * defs.h
 *
 * This file defines the entity and attributes names used for graph specification.
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

#ifndef __DEFS_H__
#define __DEFS_H__


/* info entity */
#define ENTITY_INFO  "info"

#define ATTR_INFO_TITLE     "title"
#define ATTR_INFO_COMMENTS  "comments"
#define ATTR_INFO_DATE      "date"
#define ATTR_INFO_REVISION  "revision"


/* node entity */
#define ENTITY_NODE  "node"

#define ATTR_NODE_ID     "id"
#define ATTR_NODE_TAGS   "tags"
#define ATTR_NODE_SHAPE  "shape"


/* edge entity */
#define ENTITY_EDGE  "edge"

#define ATTR_EDGE_ID          "id"

#define ATTR_EDGE_SRC_PORT    "src_port"
#define ATTR_EDGE_SRC_STYLE   "src_style"
#define ATTR_EDGE_SRC_TEXT    "src_text"

#define ATTR_EDGE_DEST_PORT   "dest_port"
#define ATTR_EDGE_DEST_STYLE  "dest_style"
#define ATTR_EDGE_DEST_TEXT   "dest_text"


/* group entity */
#define ENTITY_GROUP "group"

#define ATTR_GROUP_SHAPE     "shape"
#define ATTR_GROUP_CONTENTS  "contents"


/* shared attributes */
#define ATTR_PEN_COLOR    "stroke_color"
#define ATTR_PEN_STYLE    "stroke_style"
#define ATTR_PEN_WIDTH    "stroke_width"

#define ATTR_BRUSH_COLOR  "fill_color"
#define ATTR_BRUSH_STYLE  "fill_style"

#define ATTR_FONT_COLOR   "text_color"
#define ATTR_FONT_STYLE   "text_style"
#define ATTR_FONT_SIZE    "text_size"

#define ATTR_TEXT         "text"


#endif
