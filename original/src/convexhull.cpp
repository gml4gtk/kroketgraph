/*
 * convexhull.cpp
 *
 * Implementation of the ConvexHull object's methods.
 *
 * A description of the Graham Scan algorithm can be found
 * here : http://en.wikipedia.org/wiki/Graham_scan
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

#include <cmath>
#include "convexhull.h"


/*
 * Storage for a point of a processed convex hull.
 */
class Point
{
 public:
  Point ( qreal px, qreal py ) : x(px), y(py) { }
  Point ( const QPointF &p ) : x(p.x()), y(p.y()) { }

  qreal x;
  qreal y;
  qreal angle;
  qreal dst;

} ;


/*
 * Orientation constants
 */
typedef enum
{
  COLLINEAR,
  CLOCKWISE,
  C_CLOCKWISE

} Orientation ;


/*
 * Calculate the angle between two points.
 * We assume that p1 always satisfy p1->y <= p2->y, this is
 * guaranteed because p1 is the pivot of the convex hull algorithm.
 */
static inline qreal
angle ( Point *p1,
        Point *p2 )
{
  const qreal x = p2->x - p1->x;
  const qreal y = p2->y - p1->y;

  if ( x == 0.0 )
    {
      if ( y > 0.0 )
        return M_PI_2;
      else if ( y < 0.0 )
        return M_PI+M_PI_2;

      return 0.0;
    }

  if ( x > 0.0 )
    {
      if ( y > 0.0 )
        return atan(y/x );
      else if ( y < 0.0 )
        return ( atan(-x/y) + M_PI + M_PI_2 );

      return 0.0;
    }

  if ( y > 0.0 )
    return ( atan(-x/y) + M_PI_2 );
  else if ( y < 0.0 )
    return ( atan(y/x) + M_PI );

  return 0.0;
}


/*
 * Calculate the orientation of an angle.
 */
static inline Orientation
orientation ( const Point *p1,
              const Point *p2,
              const Point *p3 )
{
  qreal cp = (p2->x - p1->x) * (p3->y - p1->y) - (p3->x - p1->x) * (p2->y - p1->y); /* cross product */

  if ( cp < 0.0 )
    {
      if ( cp > -1.0e-12 )
        return COLLINEAR;

      return CLOCKWISE;
    }

  if ( cp < 1.0e-12 )
    return COLLINEAR;

  return C_CLOCKWISE;
}


/*
 * Calculate a distance measurement between two points.
 * We do not use sqrt() because we do not need a correct result.
 * The value is used for sorting (not using sqrt() maintains the order).
 */
static inline qreal
distance ( const Point *p1,
           const Point *p2 )
{
  qreal x = p2->x - p1->x;
  qreal y = p2->y - p1->y;

  return (x*x + y*y);
}


/*
 * Compare two points' angles (used by qSort).
 */
static bool
compareAngles ( const Point *p1,
                const Point *p2 )
{
  if ( p1->angle == p2->angle )
    return ( p1->dst < p2->dst ) ? true : false;
  else
    return ( p1->angle < p2->angle );
}


/*
 * Create the polygon which makes a hull around the provided shapes.
 */
QPolygonF
ConvexHull::compute ( QList<AbstractNodeShape *> &contents )
{
  QList<Point *> points;
  QStack<Point *> grahamStack;
  QPolygonF ret;
  int idx;
  int i;
  Point *p, *tmp;
  QRectF r;

  /* feed the points list using items bounding rectangle */
  foreach ( AbstractNodeShape *ans, contents )
    {
      if ( ( !((Node *)ans)->isActive() ) && ( !ans->isVisible() ) )
        continue; /* a group can be inactive but still visible */

      r = ans->boundingRect ( );
      r.translate ( ans->x(), ans->y() );
      r.adjust ( -6, -6, 6, 6 );
      points.append ( new Point ( r.topLeft() ) );
      points.append ( new Point ( r.topRight() ) );
      points.append ( new Point ( r.bottomRight() ) );
      points.append ( new Point ( r.bottomLeft() ) );
    }

  if ( points.size() == 0 )
    {
      ret.append ( QPointF(0,0) );
      return ret;
    }

  /* select starting point (pivot) */
  p = points[0];
  idx = 0;
  tmp = p;

  for ( i=1; i<points.size(); ++i )
    {
      tmp = points[i];

      if ( tmp->y < p->y )
        {
          p = tmp;
          idx = i;
        }
      else if ( tmp->y == p->y )
        {
          if ( tmp->x < p->x )
            {
              p = tmp;
              idx = i;
            }
        }
    }

  points.swap ( 0, idx );
  p->angle = 0.0;


  /* sort points according to their angle with the pivot */
  for ( i=1; i<points.size(); ++i )
    {
      points[i]->angle = angle ( p, points[i] );
      points[i]->dst = distance ( p, points[i] );
    }

  qSort ( ++points.begin(), points.end(), compareAngles );


  /* feed the polygon with the stack produced by the graham scan algorithm */
  grahamStack.push_front ( points[0] );
  grahamStack.push_front ( points[1] );

  i = 2;

  while ( i < points.size() )
    {
      if ( grahamStack.size() > 1 )
        {
          if ( orientation(grahamStack[1], grahamStack[0], points[i]) == C_CLOCKWISE )
            grahamStack.push_front ( points[i++] );
          else
            grahamStack.pop_front ( );
        }
      else
        grahamStack.push_front ( points[i++] );
    }

  foreach ( p, grahamStack )
    ret.append ( QPointF(p->x, p->y) );


  /* clean up */
  qDeleteAll ( points );

  return ret;
}
