/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koGenStyles.h"
#include <koxmlwriter.h>
#include <float.h>

KoGenStyles::KoGenStyles()
{
}

KoGenStyles::~KoGenStyles()
{
}

QString KoGenStyles::lookup( const KoGenStyle& style, const QString& name, bool forceNumbering )
{
    StyleMap::iterator it = m_styles.find( style );
    if ( it == m_styles.end() ) {
        QString styleName( name );
        if ( styleName.isEmpty() ) {
            styleName = 'A'; // for "auto".
            forceNumbering = true;
        }
        styleName = makeUniqueName( styleName, forceNumbering );
        m_names.insert( styleName, true );
        it = m_styles.insert( style, styleName );
    }
    return it.data();
}

QString KoGenStyles::makeUniqueName( const QString& base, bool forceNumbering ) const
{
    if ( !forceNumbering && m_names.find( base ) == m_names.end() )
        return base;
    int num = 1;
    QString name;
    do {
        name = base;
        name += QString::number( num++ );
    } while ( m_names.find( name ) != m_names.end() );
    return name;
}

QValueList<KoGenStyles::NamedStyle> KoGenStyles::styles( int type ) const
{
    QValueList<KoGenStyles::NamedStyle> lst;
    StyleMap::const_iterator it = m_styles.begin();
    StyleMap::const_iterator end = m_styles.end();
    for ( ; it != end ; ++it ) {
        if ( it.key().type() == type ) {
            NamedStyle s;
            s.style = & it.key();
            s.name = it.data();
            lst.append( s );
        }
    }
    return lst;
}

////

void KoGenStyle::writeStyle( KoXmlWriter* writer, const char* elementName, const QString& name, const char* propertiesElementName, bool closeElement ) const
{
    writer->startElement( elementName );
    writer->addAttribute( "style:name", name );
    if ( !m_parentName.isEmpty() )
        writer->addAttribute( "style:parent-style-name", m_parentName );
    // ## maybe get the family from the parent style?
    if ( !m_familyName.isEmpty() )
        writer->addAttribute( "style:family", m_familyName );
    QMap<QString, QString>::const_iterator it = m_attributes.begin();
    for ( ; it != m_attributes.end(); ++it )
        writer->addAttribute( it.key().utf8(), it.data().utf8() );
    if ( !m_properties.isEmpty() ) {
        writer->startElement( propertiesElementName );
        it = m_properties.begin();
        for ( ; it != m_properties.end(); ++it )
            writer->addAttribute( it.key().utf8(), it.data().utf8() );
        writer->endElement();
    }
    if ( closeElement )
        writer->endElement();
}

void KoGenStyle::addPropertyPt( const QString& propName, double propValue )
{
    QString str;
    str.setNum( propValue, 'g', DBL_DIG );
    str += "pt";
    m_properties.insert( propName, str );
}
