/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#include "KoTextEditingPlugin.h"

#include <QTextDocument>
#include <QTextBlock>
#include <QTextCursor>

KoTextEditingPlugin::KoTextEditingPlugin()
    : d(0)
{
}

KoTextEditingPlugin::~KoTextEditingPlugin() {
    // delete d;
}

void KoTextEditingPlugin::selectWord(QTextCursor &cursor, int cursorPosition) const {
    cursor.setPosition(cursorPosition);
    QTextBlock block = cursor.block();
    cursorPosition -= block.position();
    QString string = block.text();
    int pos = 0;
    QString::Iterator iter = string.begin();
    while(iter != string.end()) {
        if(iter->isSpace()) {
            if(pos < cursorPosition)
                cursor.setPosition(pos + block.position());
            else
                break;
        }
        pos++;
        iter++;
    }
    cursor.setPosition(pos + block.position(), QTextCursor::KeepAnchor);
}

QString KoTextEditingPlugin::paragraph(QTextDocument *document, int cursorPosition) const {
    QTextBlock block = document->findBlock(cursorPosition);
    return block.text();
}

void KoTextEditingPlugin::checkSection(QTextDocument *document, int startPosition, int endPosition) {
    // loop over the blocks indicated.
    // for each complete paragraph call finishedParagraph
    // for each (complete) word in the text; call finishedWord
}
