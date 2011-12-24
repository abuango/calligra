/* This file is part of the Calligra project
   Copyright (C) 2010 Pramod S G <pramod.xyle@gmail.com>
   Copyright (C) 2012 Stuart Dickson <stuart@kogmbh.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QFile>
#include <QString>

//Added by qt3to4:
#include <QByteArray>
#include <QFileInfo>
#include <QDir>

#include <KoStoreDevice.h>
#include <KStandardDirs>
#include <QtXmlPatterns>
#include "convert.h"
#include <QFile>

void Conversion::convert(const QString& inputFileName, QFile* outputFile, const QString& resourcesPath)
{
    QByteArray contall("<?xml version='1.0' encoding='UTF-8'?>");
    contall.append("<office:document xmlns:office='urn:oasis:names:tc:opendocument:xmlns:office:1.0'>");

    QByteArray cont;
    QByteArray sty;
    QByteArray met;

    KoStore* storecont = KoStore::createStore(inputFileName, KoStore::Read);
    storecont->extractFile("meta.xml",met);
    met.remove(0,38);
    contall.append(met);

    storecont->extractFile("styles.xml",sty);
    sty.remove(0,38);
    contall.append(sty);

    storecont->extractFile("content.xml",cont);
    cont.remove(0,38);
    contall.append(cont);

    contall.append("</office:document>");

    QFile temp1(KStandardDirs::locate("data","words/html-odf/converter.xsl"));
    temp1.open(QIODevice::ReadOnly);

    
    // Temporary step to output the concatenated file to test validation etc.

    QXmlQuery myQuery(QXmlQuery::XSLT20);
    myQuery.bindVariable(QString("html-odf-resourcesPath"), QVariant(resourcesPath));
    myQuery.setFocus(contall);
    myQuery.setQuery(temp1.readAll());
    myQuery.evaluateTo(outputFile);

    
    temp1.close();
    contall.clear();
    met.clear();
    sty.clear();
    cont.clear();

    delete storecont;
  
    
}
