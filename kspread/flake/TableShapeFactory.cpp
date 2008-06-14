/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "TableShapeFactory.h"

#include <QStringList>

#include <kgenericfactory.h>
#include <klocale.h>

#include <KoProperties.h>
#include <KoToolRegistry.h>
#include <KoShapeRegistry.h>
#include <KoXmlNS.h>

#include "TableShape.h"
#include "TableToolFactory.h"

using namespace KSpread;

K_EXPORT_COMPONENT_FACTORY( spreadsheetshape, KGenericFactory<TableShapePlugin>( "TableShape" ) )

TableShapePlugin::TableShapePlugin( QObject * parent,  const QStringList& )
{
    KoShapeRegistry::instance()->add( new TableShapeFactory( parent ) );
    KoToolRegistry::instance()->add( new TableToolFactory( parent ) );
}


TableShapeFactory::TableShapeFactory( QObject* parent )
    : KoShapeFactory( parent, TableShapeId, i18n( "Table" ) )
{
    setToolTip( i18n( "A shape that shows a table" ) );
    setIcon( "spreadsheetshape" );
    setOdfElementNames(KoXmlNS::table, QStringList() << "table");
#if 0
    KoShapeTemplate t;
    t.name = "Simple table";
    t.toolTip = "Table shape containing some cells";
    KoProperties *props = new KoProperties();
    t.properties = props;
    props->setProperty( "columns", 2 );
    props->setProperty( "rows", 2 );
    addTemplate( t );


#endif
}

KoShape* TableShapeFactory::createDefaultShape(KoShapeControllerBase *shapeController)
{
    TableShape* shape = new TableShape();
    return shape;
}

KoShape* TableShapeFactory::createShape(const KoProperties* params, KoShapeControllerBase *shapeController)
{
    TableShape* shape = new TableShape();
    return shape;
}

bool TableShapeFactory::supports(const KoXmlElement &element) const
{
    return (element.namespaceURI() == KoXmlNS::table && element.localName() == "table");
}

KoShape* TableShapeFactory::createDefaultShape() const
{
    TableShape* shape = new TableShape();
    return shape;
}

KoShape* TableShapeFactory::createShape( const KoProperties* params ) const
{
    TableShape* shape = new TableShape();
    return shape;
}

#include "TableShapeFactory.moc"
