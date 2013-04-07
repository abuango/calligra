/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// Own
#include "KoOdfStyleManager.h"

// Qt
#include <QHash>
#include <QString>

// KDE
#include <kdebug.h>

// odf lib
#include "KoStore.h"
#include <KoOdfReadStore.h>
#include <KoXmlStreamReader.h>
#include <KoXmlNS.h>

#include "KoOdfStyle.h"


// ================================================================
//                 class KoOdfStyleManager


class KoOdfStyleManager::Private
{
public:
    Private() {};

    QHash<QString, KoOdfStyle*> styles;         // name, style
    QHash<QString, KoOdfStyle*> defaultStyles;  // family, style
};


// ----------------------------------------------------------------


KoOdfStyleManager::KoOdfStyleManager()
    : d(new KoOdfStyleManager::Private())
{
}

KoOdfStyleManager::~KoOdfStyleManager()
{
    delete d;
}


KoOdfStyle *KoOdfStyleManager::style(QString &name) const
{
    return d->styles.value(name, 0);
}

void KoOdfStyleManager::setStyle(QString &name, KoOdfStyle *style)
{
    d->styles.insert(name, style);
}


KoOdfStyle *KoOdfStyleManager::defaultStyle(QString &family) const
{
    return d->defaultStyles.value(family, 0);
}

void KoOdfStyleManager::setDefaultStyle(QString &family, KoOdfStyle *style)
{
    d->styles.insert(family, style);
}


void KoOdfStyleManager::clear()
{
    qDeleteAll(d->styles);
    qDeleteAll(d->defaultStyles);
}


// ----------------------------------------------------------------


bool KoOdfStyleManager::loadStyles(KoStore *odfStore)
{
    //KoXmlDocument doc;
    QString errorMsg;
    //int errorLine;
    //int errorColumn;


    // ----------------------------------------------------------------
    // Get styles from content.xml.

    // Try to open content.xml. Return if it failed.
    //kDebug(30503) << "parse content.xml styles";
    if (!odfStore->open("content.xml")) {
        kError(30503) << "Unable to open input file content.xml" << endl;
        return false;
    }

    kDebug() << "================================================================\n"
             << "Loading styles from content.xml";

    KoXmlStreamReader reader;
    prepareForOdf(reader);
    reader.setDevice(odfStore->device());
    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement() && reader.qualifiedName() == "office:automatic-styles") {
            break;
        }
    }
    // FIXME: Error handling

    // Collect the styles.
    collectStyleSet(reader);

    odfStore->close(); // end of parsing styles in content.xml

    // ----------------------------------------------------------------
    // Get styles from styles.xml.

    kDebug() << "================================================================\n"
             << "Loading styles from styles.xml";

    // Try to open and set styles.xml as a KoXmlDocument. Return if it failed.
    if (!odfStore->open("styles.xml")) {
        kError(30503) << "Unable to open input file styles.xml" << endl;
        return false;
    }

    reader.setDevice(odfStore->device());
    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement() && reader.qualifiedName() == "office:styles") {
            break;
        }
    }
    // FIXME: Error handling

    // Collect the styles.
    collectStyleSet(reader);

    odfStore->close();
    return true;
}

void KoOdfStyleManager::collectStyleSet(KoXmlStreamReader &reader)
{
    kDebug() << "current element:" << reader.qualifiedName().toString();

    while (reader.readNextStartElement()) {

        // For now: handle style:style and style:default-style
        // and only the text, paragraph and graphic families.
        QString tagName = reader.qualifiedName().toString();
        kDebug() << "tagname: " << tagName;
        if (tagName != "style:style" && tagName != "style:default-style")
            continue;

        KoXmlStreamAttributes  attrs = reader.attributes();
        for (int i = 0; i < attrs.size(); ++i) kDebug() << attrs[i].qualifiedName().toString();
        QString family = attrs.value("style:family").toString();
        kDebug() << "family: " << family;
        if (family == "text"
            || family == "paragraph"
            || family == "graphic")
        {
            // FIXME: In the future, create style per type (family).
            KoOdfStyle *style = new KoOdfStyle;

            kDebug() << "Loading style type:" << family;

            style->setIsDefaultStyle(tagName == "style:default-style");
            style->loadOdf(reader);
#if 0 // debug
            kDebug(30503) << "==" << styleName << ":\t"
                          << style->family()
                          << style->parent()
                          << style->isDefaultStyle;
#endif
            if (style->isDefaultStyle()) {
                QString family = style->family();
                setDefaultStyle(family, style);
            }
            else {
                QString styleName = style->name();
                setStyle(styleName, style);
            }
        }
    }
}
