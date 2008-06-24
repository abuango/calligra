/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>

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

// Local
#include "ChartShape.h"
#include "ChartDocument.h"

// Qt
#include <QWidget>
#include <QIODevice>
#include <QDebug>
#include <QPainter>

// KOffice
#include <KoDocument.h>
#include <KoOdfReadStore.h>
#include <KoView.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

namespace KChart {

class ChartDocument::Private
{
public:
    Private();
    ~Private();
    
    ChartShape *parent;
};

ChartDocument::Private::Private()
{
}

ChartDocument::Private::~Private()
{
}

ChartDocument::ChartDocument( ChartShape *parent )
    : d ( new Private )
    , KoDocument( 0, 0 )
{
    d->parent = parent;
}

ChartDocument::~ChartDocument()
{
}


bool ChartDocument::loadOdf( KoOdfReadStore &odfStore )
{
    KoXmlDocument doc = odfStore.contentDoc();
    KoXmlNode bodyNode = doc.documentElement().namedItemNS( KoXmlNS::office, "body" );
    if ( bodyNode.isNull() )
        return false;
    KoXmlNode chartElementParentNode = bodyNode.namedItemNS( KoXmlNS::office, "chart" );
    if ( chartElementParentNode.isNull() )
        return false;
    KoXmlElement chartElement = chartElementParentNode.namedItemNS( KoXmlNS::chart, "chart" ).toElement();
    if ( chartElement.isNull() )
        return false;

    return d->parent->loadOdfEmbedded( chartElement, odfStore.styles() );
}

bool ChartDocument::loadXML( QIODevice *, const KoXmlDocument &doc )
{
    return false;
}

bool ChartDocument::saveOdf( KoDocument::SavingContext &context )
{
    return false;
}

KoView *ChartDocument::createViewInstance( QWidget *parent )
{
    return 0;
}

void ChartDocument::paintContent( QPainter &painter, const QRect &rect )
{
}

} // namespace KChart

