/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.t-com.hr)
   Copyright (C) 2005, The Karbon Developers
   Copyright (c) 2008 Jan Hambrecht <jaham@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KarbonSmallStylePreview.h"
#include <KarbonGradientHelper.h>
#include <KoGradientBackground.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoShapeManager.h>
#include <KoShape.h>
#include <KoSelection.h>
#include <KoLineBorder.h>

#include <KLocale>
#include <KGlobalSettings>

#include <QtGui/QColor>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QPixmap>
#include <QtGui/QGridLayout>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtCore/QPointF>
#include <QtCore/QRectF>

#define FRAMEWIDTH 100
#define FRAMEHEIGHT 15

class KarbonFillStyleWidget : public QFrame
{
public:
    KarbonFillStyleWidget( QWidget * parent )
    : QFrame( parent ), m_fill(0), m_checkerPainter( 5 )
    {
    }
    void setFill( const KoShapeBackground * fill )
    {
        m_fill = fill;
        update();
    }
protected:
    virtual void paintEvent( QPaintEvent* event )
    {
        QPainter painter( this );
        painter.setClipRect( event->rect() );

        if( m_fill )
        {
            m_checkerPainter.paint( painter, rect() );

            const KoGradientBackground * gradientFill = dynamic_cast<const KoGradientBackground*>( m_fill );
            if( gradientFill )
            {
                const QGradient * gradient = gradientFill->gradient();
                QGradient * defGradient = KarbonGradientHelper::defaultGradient( size(), gradient->type(), gradient->spread(), gradient->stops() );
                QBrush brush( *defGradient );
                delete defGradient;
                painter.setBrush( brush );
                painter.setPen( Qt::NoPen );
                painter.drawRect( rect() );
            }
            else
            {
                // use the background to draw
                QPainterPath p;
                p.addRect( rect() );
                m_fill->paint( painter, p );
            }
        }
        else
        {
            painter.setFont( KGlobalSettings::smallestReadableFont() );
            painter.setBrush( Qt::black );
            painter.setPen( Qt::black );
            painter.drawText( rect(), Qt::AlignCenter, i18n( "None" ) );
        }

        painter.end();

        QFrame::paintEvent( event );
    }

private:
    const KoShapeBackground * m_fill; ///< the fill to preview
    KoCheckerBoardPainter m_checkerPainter;
};

class KarbonStrokeStyleWidget : public QFrame
{
public:
    KarbonStrokeStyleWidget( QWidget * parent )
    : QFrame( parent ), m_stroke(0), m_checkerPainter( 5 )
    {
    }
    void setStroke( const KoShapeBorderModel * stroke )
    {
        m_stroke = stroke;
        update();
    }
protected:
    virtual void paintEvent( QPaintEvent* event )
    {
        QPainter painter( this );
        painter.setClipRect( event->rect() );

        if( m_stroke )
        {
            m_checkerPainter.paint( painter, rect() );
            const KoLineBorder * line = dynamic_cast<const KoLineBorder*>( m_stroke );
            if( line )
            {
                QBrush brush = line->lineBrush();
                if( brush.gradient() )
                {
                    QGradient * defGradient = KarbonGradientHelper::defaultGradient( size(), brush.gradient()->type(), brush.gradient()->spread(), brush.gradient()->stops() );
                    QBrush brush( *defGradient );
                    delete defGradient;
                    painter.setBrush( brush );
                    painter.setPen( Qt::NoPen );
                    painter.drawRect( rect() );
                }
                else if( brush.style() == Qt::TexturePattern )
                {
                    painter.fillRect( rect(), brush );
                }
                else
                {
                    painter.fillRect( rect(), QBrush( line->color() ) );
                }
            }
            else
            {
                painter.setFont( KGlobalSettings::smallestReadableFont() );
                painter.setBrush( Qt::black );
                painter.setPen( Qt::black );
                painter.drawText( rect(), Qt::AlignCenter, i18n( "Custom" ) );
            }
        }
        else
        {
            painter.setFont( KGlobalSettings::smallestReadableFont() );
            painter.setBrush( Qt::black );
            painter.setPen( Qt::black );
            painter.drawText( rect(), Qt::AlignCenter, i18n( "None" ) );
        }

        painter.end();

        QFrame::paintEvent( event );
    }

private:
    const KoShapeBorderModel * m_stroke; ///< the stroke to preview
    KoCheckerBoardPainter m_checkerPainter;
};

KarbonSmallStylePreview::KarbonSmallStylePreview( QWidget* parent )
    : QWidget( parent )
{
    /* Create widget layout */
    QGridLayout *layout = new QGridLayout(this);
    QLabel * strokeLabel = new QLabel( i18n( "Stroke:" ), this );
    m_strokeFrame = new KarbonStrokeStyleWidget( this );
    m_strokeFrame->setFrameStyle( QFrame::StyledPanel | QFrame::Plain );
    m_strokeFrame->setFixedSize( QSize(FRAMEWIDTH,FRAMEHEIGHT) );
    m_strokeFrame->setMinimumSize( QSize(FRAMEWIDTH,FRAMEHEIGHT) );

    QLabel * fillLabel = new QLabel( i18n( "Fill:" ), this );
    m_fillFrame = new KarbonFillStyleWidget( this );
    m_fillFrame->setFrameStyle( QFrame::StyledPanel | QFrame::Plain );
    m_fillFrame->setFixedSize( QSize(FRAMEWIDTH,FRAMEHEIGHT) );
    m_fillFrame->setMinimumSize( QSize(FRAMEWIDTH,FRAMEHEIGHT) );

    layout->addWidget( strokeLabel, 0, 0 );
    layout->addWidget( m_strokeFrame, 0, 1 );
    layout->addWidget( fillLabel, 1, 0 );
    layout->addWidget( m_fillFrame, 1, 1 );
    layout->setContentsMargins( 0, 0, 0, 0 );

    setFont( KGlobalSettings::smallestReadableFont() );
    setLayout( layout );

    connect( KoToolManager::instance(), SIGNAL(toolCodesSelected(const KoCanvasController *, QList<QString>)),
             this, SLOT(toolCodesSelected(const KoCanvasController *, QList<QString>)));
}

KarbonSmallStylePreview::~KarbonSmallStylePreview()
{
}

void KarbonSmallStylePreview::toolCodesSelected(const KoCanvasController *canvas, QList<QString> types)
{
    Q_UNUSED(types);
    connect( canvas->canvas()->shapeManager(), SIGNAL(selectionChanged()),
             this, SLOT(selectionChanged()) );
    connect( canvas->canvas()->shapeManager(), SIGNAL(selectionContentChanged()),
             this, SLOT(selectionChanged()) );

    selectionChanged();
}

void KarbonSmallStylePreview::selectionChanged()
{
    KoCanvasBase * canvas = KoToolManager::instance()->activeCanvasController()->canvas();
    KoShape * shape = canvas->shapeManager()->selection()->firstSelectedShape();
    if( shape )
    {
        m_fillFrame->setFill( shape->background() );
        m_strokeFrame->setStroke( shape->border() );
    }
    else
    {
        m_fillFrame->setFill( 0 );
        m_strokeFrame->setStroke( 0 );
    }
    QWidget::update();
}

#include "KarbonSmallStylePreview.moc"

