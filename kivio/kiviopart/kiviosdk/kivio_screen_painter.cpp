/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "kivio_config.h"
#include "kivio_line_style.h"
#include "kivio_point.h"
#include "kivio_screen_painter.h"

#include <qpen.h>
#include <qbrush.h>

#include <kdebug.h>

#define PAINTER_CHECK() { if(!m_pPainter) { kdDebug() << "KivioScreenPainter::PAINTER_CHECK() - no QPainter exists." << endl; } }


KivioScreenPainter::KivioScreenPainter()
    : m_pPainter(NULL)
{
   m_transX = 0.0f;
   m_transY = 0.0f;
   m_rotation = 0.0f;
}

KivioScreenPainter::KivioScreenPainter(QPainter* painter)
    : m_pPainter(painter)
{
   m_transX = 0.0f;
   m_transY = 0.0f;
   m_rotation = 0.0f;
}

KivioScreenPainter::~KivioScreenPainter()
{
    if( m_pPainter )
    {
       kdDebug() << "KivioScreenPainter::~KivioScreenPainter - A QPainter slipped through the cracks" << endl;
        delete m_pPainter;
        m_pPainter = NULL;
    }
}

/**
 * Starts the drawing on a @ref QPaintDevice
 *
 * @param dev The device to paint on
 *
 * Call this function before you call any other @ref KivioScreenPainter
 * functions.  This will create an internal QPainter object for drawing
 * with.  If you fail to call this function, nothing will work.
 */
bool KivioScreenPainter::start( QPaintDevice *dev )
{
    // Bomb out if one exists already
    if( m_pPainter )
    {
       kdDebug() << "KivioScreenPainter::start() - A QPainter already exists" << endl;
        return false;
    }

    // Allocate a new drawing thingy
    m_pPainter = new QPainter(dev);

    return true;
}


/**
 * Stops the drawing and flushes the drawing pipeline
 *
 * After using the @ref KivioScreenPainter object, call stop() to
 * finalize it all.
 */
bool KivioScreenPainter::stop()
{
    // Bomb out if we don't have a painter.  This means that they never
    // called @ref start().
    if( !m_pPainter )
    {
       kdDebug() <<"KivioScreenPainter::stop() called without previous call to start" << endl;
        return false;
    }

    // Otherwise delete and nullify the pointer
    delete m_pPainter;
    m_pPainter = NULL;

    return true;
}



/**
 * Draws a line from one point to another
 *
 * @param x1 The first x coord
 * @param y1 The first y coord
 * @param x2 The second x coord
 * @param y2 The second y coord
 *
 * This function will draw a line from one point to another in
 * the current fgColor and using the current lineWidth.
 */
void KivioScreenPainter::drawLine( float x1, float y1, float x2, float y2 )
{
    PAINTER_CHECK();

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));

    m_pPainter->drawLine( int(x1), int(y1), int(x2), int(y2) );

}


/**
 * Draws an arc
 *
 * @param x1 The x-coord of the arc-box
 * @param y1 The y-coord of the arc-box
 * @param w1 The width of the arc
 * @param h1 The height of the arc
 * @param a1 The starting angle of the arc ??
 * @param a2 The total angle of the arc???
 *
 * This will draw an arc in the current fgColor and lineWidth.
 */
void KivioScreenPainter::drawArc( float x1, float y1, float w1, float h1, float a1, float a2 )
{
    PAINTER_CHECK();

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));

    m_pPainter->drawArc( int(x1), int(y1), int(w1), int(h1), int(a1), int(a2) );
}


/**
 * Draws a single quad-bezier curve
 *
 * @param pArray The array of 4 points to draw
 *
 * This will draw a quad-bezier curve in the current fgColor
 * and lineWidth.  @ref pArray must have 4 points.
 */
void KivioScreenPainter::drawBezier( QPointArray &pArray )
{
    PAINTER_CHECK();

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));

    m_pPainter->drawCubicBezier(pArray);
}


/**
 * Draws a hollow rectangle
 *
 * @param x1 The x coord
 * @param y1 The y coord
 * @param w1 The width
 * @param h1 The height
 */
void KivioScreenPainter::drawRect( float x1, float y1, float w1, float h1 )
{
    PAINTER_CHECK();

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(QBrush::NoBrush);
    m_pPainter->drawRect( int(x1), int(y1), int(w1), int(h1) );
}


/**
 * Draws a filled/outlined rectangle
 *
 * @param x1 The x coord
 * @param y1 The y coord
 * @param w1 The width
 * @param h1 The height
 *
 * This draws a filled, and outlined rectangle in the current
 * fgColor, lineWidth, and fillStyle.
 */
void KivioScreenPainter::fillRect( float x1, float y1, float w1, float h1 )
{
    PAINTER_CHECK();

    QBrush b;


    switch( m_pFillStyle->colorStyle() )
    {
        case KivioFillStyle::kcsSolid:
            b.setColor( m_pFillStyle->color() );
            b.setStyle( QBrush::SolidPattern );
            break;

        case KivioFillStyle::kcsGradient:
	   kdDebug() << "KivioScreenPainter::fillRect() - Gradient not supported yet" << endl;
            return;

        case KivioFillStyle::kcsNone:
	   kdDebug() << "KivioScreenPainter::fillRect() - don't call fillRect() without wanting to fill it!" << endl;
            return;

        default:
	   kdDebug() << "KivioScreenpainter::fillRect() - unknown color style" << endl;
            return;
    }

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(b);
    m_pPainter->drawRect( int(x1), int(y1), int(w1), int(h1) );
}


/**
 * Draws a hollow round-rectangle
 *
 * @param x1 The x coord
 * @param y1 The y coord
 * @param w1 The width
 * @param h1 The height
 * @param a1 ????
 * @param a2 ????
 *
 * This draws a hollow round-rectangle in the current
 * fgColor and lineWidth.
 */
void KivioScreenPainter::drawRoundRect( float x1, float y1, float w1, float h1, float a1, float a2 )
{
    PAINTER_CHECK();

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(QBrush::NoBrush);
    m_pPainter->drawRoundRect( int(x1), int(y1), int(w1), int(h1), int(a1), int(a2) );
}



/**
 * Draws a filled/outlined round-rectangle
 *
 * @param x1 The x coord
 * @param y1 The y coord
 * @param w1 The width
 * @param h1 The height
 * @param a1 ????
 * @param a2 ????
 *
 * This draws a hollow round-rectangle in the current
 * fgColor, lineWidth, and fillStyle.
 */
void KivioScreenPainter::fillRoundRect( float x1, float y1, float w1, float h1, float a1, float a2 )
{
    PAINTER_CHECK();

    QBrush b;


    switch( m_pFillStyle->colorStyle() )
    {
        case KivioFillStyle::kcsSolid:
            b.setColor( m_pFillStyle->color() );
            b.setStyle( QBrush::SolidPattern );
            break;

        case KivioFillStyle::kcsGradient:
	   kdDebug() << "fillRoundRect() - Gradient not supported yet" << endl;
            return;

        case KivioFillStyle::kcsNone:
	   kdDebug() << "don't call fillRoundRect() without wanting to fill it!" << endl;
            return;

        default:
           kdDebug() << "fillRoundRect() - unknown color style" << endl;
            return;
    }

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(b);
    m_pPainter->drawRoundRect( int(x1), int(y1), int(w1), int(h1), int(a1), int(a2) );
}


/**
 * Draws a hollow pie
 *
 * @param x1 The x coord
 * @param y1 The y coord
 * @param w1 The width
 * @param h1 The height
 * @param a1 ????
 * @param a2 ????
 *
 * This draws a hollow pie in the current
 * fgColor and lineWidth.
 */
void KivioScreenPainter::drawPie( float x1, float y1, float w1, float h1, float a1, float a2 )
{
    PAINTER_CHECK();

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(QBrush::NoBrush);
    m_pPainter->drawPie( int(x1), int(y1), int(w1), int(h1), int(a1), int(a2) );
}


/**
 * Draws a filled/outlined pie
 *
 * @param x1 The x coord
 * @param y1 The y coord
 * @param w1 The width
 * @param h1 The height
 * @param a1 ????
 * @param a2 ????
 *
 * This draws a filled/outlined pie in the current
 * fgColor, lineWidth, and fillStyle.
 */
void KivioScreenPainter::fillPie( float x1, float y1, float w1, float h1, float a1, float a2 )
{
    PAINTER_CHECK();

    QBrush b;

    switch( m_pFillStyle->colorStyle() )
    {
        case KivioFillStyle::kcsSolid:
            b.setColor( m_pFillStyle->color() );
            b.setStyle( QBrush::SolidPattern );
            break;

        case KivioFillStyle::kcsGradient:
	   kdDebug() << "fillPie() - Gradient not supported yet" << endl;
            return;

        case KivioFillStyle::kcsNone:
	   kdDebug() << "don't call fillPie() without wanting to fill it!" << endl;
            return;

        default:
	   kdDebug() << "fillPie() - unknown color style" << endl;
            return;
    }

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(b);
    m_pPainter->drawPie( int(x1), int(y1), int(w1), int(h1), int(a1), int(a2) );
}


/**
 * Draws a hollow chord
 *
 * @param x1 The x coord
 * @param y1 The y coord
 * @param w1 The width
 * @param h1 The height
 * @param a1 ????
 * @param a2 ????
 *
 * This draws a hollow chord in the current
 * fgColor and lineWidth.
 */
void KivioScreenPainter::drawChord( float x1, float y1, float w1, float h1, float a1, float a2 )
{
    PAINTER_CHECK();

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(QBrush::NoBrush);
    m_pPainter->drawChord( int(x1), int(y1), int(w1), int(h1), int(a1), int(a2) );
}


/**
 * Draws a filled/outlined chord
 *
 * @param x1 The x coord
 * @param y1 The y coord
 * @param w1 The width
 * @param h1 The height
 * @param a1 ????
 * @param a2 ????
 *
 * This draws a filled/outlined chord in the current
 * fgColor, fillStyle and lineWidth.
 */
void KivioScreenPainter::fillChord( float x1, float y1, float w1, float h1, float a1, float a2 )
{
    PAINTER_CHECK();

    QBrush b;

    switch( m_pFillStyle->colorStyle() )
    {
        case KivioFillStyle::kcsSolid:
            b.setColor( m_pFillStyle->color() );
            b.setStyle( QBrush::SolidPattern );
            break;

        case KivioFillStyle::kcsGradient:
	   kdDebug() << "fillChord() - Gradient not supported yet" << endl;
            return;

        case KivioFillStyle::kcsNone:
	   kdDebug() << "don't call fillChord() without wanting to fill it!" << endl;
            return;

        default:
	   kdDebug() << "fillChord() - unknown color style" << endl;
            return;
    }

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(b);
    m_pPainter->drawChord( int(x1), int(y1), int(w1), int(h1), int(a1), int(a2) );
}


/**
 * Draws a hollow ellipse
 *
 * @param x1 The x coord
 * @param y1 The y coord
 * @param w1 The width
 * @param h1 The height
 *
 * This draws a hollow ellipse in the current
 * fgColor and lineWidth.
 */
void KivioScreenPainter::drawEllipse( float x1, float y1, float w1, float h1 )
{
    PAINTER_CHECK();

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(QBrush::NoBrush);
    m_pPainter->drawEllipse( int(x1), int(y1), int(w1), int(h1) );
}


/**
 * Draws a filled/outlined ellipse
 *
 * @param x1 The x coord
 * @param y1 The y coord
 * @param w1 The width
 * @param h1 The height
 *
 * This draws a filled/outlined ellipse in the current
 * fgColor, fillstyle and lineWidth.
 */
void KivioScreenPainter::fillEllipse( float x1, float y1, float w1, float h1 )
{
    PAINTER_CHECK();

    QBrush b;

    switch( m_pFillStyle->colorStyle() )
    {
        case KivioFillStyle::kcsSolid:
            b.setColor( m_pFillStyle->color() );
            b.setStyle( QBrush::SolidPattern );
            break;

        case KivioFillStyle::kcsGradient:
	   kdDebug() << "fillEllipse() - Gradient not supported yet" << endl;
            return;

        case KivioFillStyle::kcsNone:
	   kdDebug() << "don't call fillEllipse() without wanting to fill it!" << endl;
            return;

        default:
	   kdDebug() << "fillEllipse() - unknown color style" << endl;
            return;
    }

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(b);
    m_pPainter->drawEllipse( int(x1), int(y1), int(w1), int(h1) );
}


/**
 * Draws an array of lines
 *
 * @param pList The array of points to draw lines with
 *
 * The list of points should contain a list of points in
 * pairs.  Lines are drawn from p1->p2 p3->p4 p5->p6, etc...
 */
void KivioScreenPainter::drawLineArray( QPtrList<KivioPoint> *pList )
{
    PAINTER_CHECK();

    KivioPoint *pPoint;
    QPointArray points(pList->count());
    int i;
    QBrush b;

    b.setStyle( QBrush::NoBrush );

    i=0;
    pPoint = pList->first();
    while( pPoint )
    {
        points.setPoint( i++, int(pPoint->x()), int(pPoint->y()) );

        pPoint = pList->next();
    }

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(b);

    m_pPainter->drawLineSegments(points);
}


/**
 * Draws a polyline
 *
 * @param pList The array of points to draw the polyline with
 *
 * Draws a polyline (hollow) from p1->p2->p3->pn
 */
void KivioScreenPainter::drawPolyline( QPtrList<KivioPoint> *pList )
{
    PAINTER_CHECK();

    KivioPoint *pPoint;
    QPointArray points( pList->count() );
    int i;

    i=0;
    pPoint = pList->first();
    while( pPoint )
    {
        points.setPoint(i++, int(pPoint->x()), int(pPoint->y()) );

        pPoint = pList->next();
    }

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(QBrush::NoBrush);
    m_pPainter->drawPolyline(points);
}



/**
 * Draws a polygon
 *
 * @param pList The array of points to draw the polyline with
 *
 * Draws a filled (if set) polygon from p1->p2->p3->pn->p1
 */
void KivioScreenPainter::drawPolygon( QPtrList<KivioPoint> *pList )
{
    PAINTER_CHECK();

    KivioPoint *pPoint;
    QPointArray points( pList->count() );
    int i;
    QBrush b;

    switch( m_pFillStyle->colorStyle() )
    {
        case KivioFillStyle::kcsSolid:
            b.setColor( m_pFillStyle->color() );
            b.setStyle( QBrush::SolidPattern );
            break;

        case KivioFillStyle::kcsGradient:
            kdDebug() << "drawPolygon() - Gradient not supported yet" << endl;
            return;

        case KivioFillStyle::kcsNone:
            b.setStyle(QBrush::NoBrush);
            return;

        default:
            kdDebug() << "drawPolygon() - unknown color style" << endl;
            return;
    }

    i=0;
    pPoint = pList->first();
    while( pPoint )
    {
        points.setPoint(i++, int(pPoint->x()), int(pPoint->y()) );

        pPoint = pList->next();
    }

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(b);
    m_pPainter->drawPolygon(points,true);

}

void KivioScreenPainter::drawPolyline( QPointArray &pArray )
{
    PAINTER_CHECK();

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush( QBrush::NoBrush );

    m_pPainter->drawPolyline( pArray );
}

void KivioScreenPainter::drawPolygon( QPointArray &pArray )
{
    PAINTER_CHECK();

    QBrush b;

    switch( m_pFillStyle->colorStyle() )
    {
        case KivioFillStyle::kcsSolid:
            b.setColor( m_pFillStyle->color() );
            b.setStyle( QBrush::SolidPattern );
            break;

        case KivioFillStyle::kcsGradient:
	   kdDebug() << "drawPolygon() - Gradient not supported yet" << endl;
            return;

        case KivioFillStyle::kcsNone:
            b.setStyle(QBrush::NoBrush);
            return;

        default:
	   kdDebug() << "drawPolygon() - unknown color style" << endl;
            return;
    }

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(b);

    m_pPainter->drawPolygon( pArray, true );
}

void KivioScreenPainter::drawLineArray( QPointArray &pArray )
{
    PAINTER_CHECK();

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush(QBrush::NoBrush);

    m_pPainter->drawLineSegments(pArray);
}


/**
 * Draws a filled path object
 *
 * @param pPoints The array of points to draw the polyline with
 *
 * Draws a filled (if set) curve with the points stored
 * in pPoints.  They can be made up of bezier, arc, and normal points.
 */
void KivioScreenPainter::drawClosedPath( QPtrList<KivioPoint> *pPoints )
{
    PAINTER_CHECK();

    QBrush brush;

    KivioPoint *pPoint, *pPoint2, *pPoint3, *pPoint4;
    QPtrList <KivioPoint> *pPointList = pPoints;
    QPointArray controlPoints(4), bPoints(0), tmpPoints;

    int pointIndex=0;
    pPoint = pPointList->first();
    while( pPoint )
    {
        if( pPoint->pointType() == KivioPoint::kptNormal )
        {
            bPoints.putPoints( pointIndex, 1, int(pPoint->x()), int(pPoint->y()) );
            pointIndex++;
        }
        else if( pPoint->pointType() == KivioPoint::kptBezier )
        {
            pPoint2 = pPointList->next();
            pPoint3 = pPointList->next();
            pPoint4 = pPointList->next();

            if( !pPoint2 || !pPoint3 || !pPoint4 )
            {
                kdDebug() << "drawClosedPath() - incorrect # of bezier points" << endl;
                return;
            }

            if( pPoint2->pointType() != KivioPoint::kptBezier ||
                pPoint3->pointType() != KivioPoint::kptBezier ||
                pPoint4->pointType() != KivioPoint::kptBezier )
            {
                kdDebug() << "drawClosedPath() - bezier curves must have 4 points" << endl;
                return;
            }

            controlPoints.setPoint( 0, pPoint->x(), pPoint->y() );
            controlPoints.setPoint( 1, pPoint2->x(), pPoint2->y() );
            controlPoints.setPoint( 2, pPoint3->x(), pPoint3->y() );
            controlPoints.setPoint( 3, pPoint4->x(), pPoint4->y() );

            tmpPoints = controlPoints.cubicBezier();

            for( int j=0; j<int(tmpPoints.size()); j++ )
            {
                bPoints.putPoints( pointIndex, 1, tmpPoints.point(j).x(), tmpPoints.point(j).y() );
                pointIndex++;
            }
        } // end pointtype==bezier
        else if( pPoint->pointType() == KivioPoint::kptArc )
        {
            pPoint2 = pPointList->next();
            pPoint3 = pPointList->next();

            if( !pPoint2 || !pPoint3 )
            {
                kdDebug() << "drawClosedPath() - incorrect # of arc points" << endl;
                return;
            }
            if( pPoint2->pointType() != KivioPoint::kptArc ||
                pPoint3->pointType() != KivioPoint::kptArc )
            {
                kdDebug() << "drawClosedPath() - Arc points must come in triplets" << endl;
                return;
            }

            tmpPoints.makeArc( pPoint->x(), pPoint->y(), pPoint2->x(), pPoint2->y(),
                               pPoint3->x(), pPoint3->y() );

            for( int j=0; j<int(tmpPoints.size()); j++ )
            {
                bPoints.putPoints( pointIndex, 1, tmpPoints.point(j).x(),tmpPoints.point(j).y());
                pointIndex++;
            }
        } // end pointtype==arc
        else
        {
          kdDebug() << "drawClosedPath() - Unknown point type discovered. WOOO!!!" << endl;
        }

        pPoint = pPointList->next();
    }

    // If we make it here, the list is built
    switch( m_pFillStyle->colorStyle() )
    {
        case KivioFillStyle::kcsNone:
            m_pPainter->setPen(m_pLineStyle->pen(1.0f));
            m_pPainter->setBrush( QBrush::NoBrush );

            // A hollow path? That's a polypath!
            m_pPainter->drawPolyline( bPoints );
            break;

        case KivioFillStyle::kcsSolid:
            // Setup the line style
            m_pPainter->setPen(m_pLineStyle->pen(1.0f));

            // Setup the fill color
            brush.setColor( m_pFillStyle->color() );
            brush.setStyle( QBrush::SolidPattern );
            m_pPainter->setBrush( brush );

            // draw the bitch
            m_pPainter->drawPolygon( bPoints, true );
            break;

        case KivioFillStyle::kcsGradient:
            kdDebug() << "drawClosedPath() - Gradient unsupported" << endl;
            break;

        default:
            kdDebug() << "drawClosedPath() - Unknown colors style" << endl;
            break;
    }
}

void KivioScreenPainter::drawOpenPath( QPtrList<KivioPoint> *pPoints )
{
    PAINTER_CHECK();

    QBrush brush;

    KivioPoint *pPoint, *pPoint2, *pPoint3, *pPoint4;
    QPtrList <KivioPoint> *pPointList = pPoints;
    QPointArray controlPoints(4), bPoints(0), tmpPoints;

    int pointIndex=0;
    pPoint = pPointList->first();
    while( pPoint )
    {
        if( pPoint->pointType() == KivioPoint::kptNormal )
        {
            bPoints.putPoints( pointIndex, 1, int(pPoint->x()), int(pPoint->y()) );
            pointIndex++;
        }
        else if( pPoint->pointType() == KivioPoint::kptBezier )
        {
            pPoint2 = pPointList->next();
            pPoint3 = pPointList->next();
            pPoint4 = pPointList->next();

            if( !pPoint2 || !pPoint3 || !pPoint4 )
            {
	       kdDebug() << "drawOpenPath() - incorrect # of bezier points" << endl;
                return;
            }

            if( pPoint2->pointType() != KivioPoint::kptBezier ||
                pPoint3->pointType() != KivioPoint::kptBezier ||
                pPoint4->pointType() != KivioPoint::kptBezier )
            {
	       kdDebug() << "drawOpenPath() - bezier curves must have 4 points" << endl;
                return;
            }

            controlPoints.setPoint( 0, pPoint->x(), pPoint->y() );
			controlPoints.setPoint( 1, pPoint2->x(), pPoint2->y() );
			controlPoints.setPoint( 2, pPoint3->x(), pPoint3->y() );
			controlPoints.setPoint( 3, pPoint4->x(), pPoint4->y() );

			tmpPoints = controlPoints.cubicBezier();

			for( int j=0; j<int(tmpPoints.size()); j++ )
			{
			    bPoints.putPoints( pointIndex, 1, tmpPoints.point(j).x(), tmpPoints.point(j).y() );
			    pointIndex++;
			}
        } // end pointtype==bezier
        else if( pPoint->pointType() == KivioPoint::kptArc )
        {
            pPoint2 = pPointList->next();
            pPoint3 = pPointList->next();

            if( !pPoint2 || !pPoint3 )
            {
	       kdDebug() << "drawOpenPath() - incorrect # of arc points" << endl;
                return;
            }
            if( pPoint2->pointType() != KivioPoint::kptArc ||
                pPoint3->pointType() != KivioPoint::kptArc )
            {
	       kdDebug() << "drawOpenPath() - Arc points must come in triplets" << endl;
                return;
            }

            tmpPoints.makeArc( pPoint->x(), pPoint->y(), pPoint2->x(), pPoint2->y(),
                               pPoint3->x(), pPoint3->y() );

            for( int j=0; j<int(tmpPoints.size()); j++ )
            {
                bPoints.putPoints( pointIndex, 1, tmpPoints.point(j).x(),tmpPoints.point(j).y());
                pointIndex++;
            }
        } // end pointtype==arc
        else
        {
	   kdDebug() << "drawOpenPath() - Unknown point type discovered. WOOO!!!" << endl;
        }

        pPoint = pPointList->next();
    }

    m_pPainter->setPen(m_pLineStyle->pen(1.0f));
    m_pPainter->setBrush( QBrush::NoBrush );

    m_pPainter->drawPolyline( bPoints );
}

void KivioScreenPainter::setFont( const QFont &f )
{
    PAINTER_CHECK();

    m_pPainter->setFont( f );
}

void KivioScreenPainter::drawText( int x, int y, int w, int h, int tf, const QString &str )
{
    PAINTER_CHECK();

    m_pPainter->setPen( m_textColor );
    m_pPainter->drawText( x, y, w, h, tf, str );
}

QRect KivioScreenPainter::boundingRect( int x, int y, int w, int h, int tf, const QString &str )
{
   PAINTER_CHECK();

   return m_pPainter->boundingRect( x,y,w,h, tf, str );
}

void KivioScreenPainter::drawPixmap( float x, float y, const QPixmap &pix )
{
   PAINTER_CHECK();
  m_pPainter->drawPixmap( (int)x, (int)y, pix );
}

void KivioScreenPainter::drawHandle( float x, float y, int flags )
{
   PAINTER_CHECK();
   QColor fillColor;
   QBrush b;
   QPen p;
   QPixmap *pLock;

   const float HW = 6.0f;
   const float HWP1 = HW+1.0f;
   const float HWo2 = HW/2.0f;

   float x1, y1;

   // Is it a locked handle?
   if( flags & cpfLock )
   {
      x1 = x - 4;
      y1 = y - 4;

      pLock = KivioConfig::config()->lockPixmap();

      m_pPainter->drawPixmap( x1, y1, *pLock );
      return;
   }

   if( flags & cpfConnected )
   {
      fillColor = QColor(200,0,0);
   }
   else
   {
      fillColor = QColor(0,200,0);
   }

   b.setColor(fillColor);
   b.setStyle(Qt::SolidPattern);
   p.setColor(QColor(0,0,0));
   m_pPainter->setPen(p);
   m_pPainter->setBrush(b);


   x1 = x - HWo2;
   y1 = y - HWo2;

   // first fill it
//   m_pPainter->fillRect( x1, y1, HWP1, HWP1, b );
   m_pPainter->drawRect( x1, y1, HWP1, HWP1 );

   // Now put something in it if needed
   if( flags & cpfStart )
   {
      m_pPainter->drawLine( x, y1+2, x, y1+4 );
      m_pPainter->drawLine( x1+2, y, x1+4, y );
   }
   else if( flags & cpfEnd )
   {
      m_pPainter->drawLine( x1+2, y1+2, x1+4, y1+4 );
      m_pPainter->drawLine( x1+2, y1+4, x1+4, y1+2 );
   }
   else if( flags & cpfConnectable )
   {
      b.setColor(QColor(0,0,0));

      m_pPainter->fillRect(x-1,y-1,3,3, b);
   }
}

void KivioScreenPainter::saveState()
{
   PAINTER_CHECK();

   m_pPainter->save();
}

void KivioScreenPainter::restoreState()
{
   PAINTER_CHECK();

   m_pPainter->restore();
}

void KivioScreenPainter::setTranslation( float _x, float _y )
{
   PAINTER_CHECK();

   m_transX = _x;
   m_transY = _y;

   m_pPainter->translate(_x, _y);
}

void KivioScreenPainter::translateBy( float _x, float _y )
{
   PAINTER_CHECK();

   m_transX += _x;
   m_transY += _y;

   m_pPainter->translate( m_transX, m_transY );
}

void KivioScreenPainter::setRotation( float _d )
{
   PAINTER_CHECK();

   m_rotation = _d;

   m_pPainter->rotate(_d);
}

void KivioScreenPainter::rotateBy( float _d )
{
   PAINTER_CHECK();

   m_rotation += _d;

   m_pPainter->rotate(m_rotation);
}
