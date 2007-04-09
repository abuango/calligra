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

#include "ChangeTracker.h"
#include "TextTool.h"

#include <KDebug>

ChangeTracker::ChangeTracker(TextTool *parent)
    : QObject(parent),
    m_document(0),
    m_tool(parent)
{
}

void ChangeTracker::setDocument(QTextDocument * document) {
    if(m_document)
        disconnect(m_document, SIGNAL(contentsChange(int,int,int)), this, SLOT(contentsChange(int,int,int)));
    m_document = document;
    if(m_document)
        connect(m_document, SIGNAL(contentsChange(int,int,int)), this, SLOT(contentsChange(int,int,int)));
}

void ChangeTracker::contentsChange (int from, int charsRemoves, int charsAdded) {
    kDebug() << "ChangeTracker::contentsChange " << from << ", " << charsRemoves << ", " << charsAdded << endl;

    if(charsRemoves == 0 && charsAdded == 0)
        return;
    if(charsRemoves == 0) { // easy
        QTextCursor cursor(m_document);
        cursor.setPosition(from);
        cursor.setPosition(from + charsAdded, QTextCursor::KeepAnchor);
        kDebug() << "   added text: \"" << cursor.selectedText() << "\"\n";
    }
    else {
        bool prev = m_tool->m_allowAddUndoCommand;
        m_tool->m_allowAddUndoCommand = false;
        m_document->undo();
        QTextCursor cursor(m_document);
        cursor.setPosition(from);
        cursor.setPosition(from + charsRemoves, QTextCursor::KeepAnchor);
        QString previousText = cursor.selectedText();
        m_document->redo();
        m_tool->m_allowAddUndoCommand = prev;

        cursor.setPosition(from);
        cursor.setPosition(from + charsAdded, QTextCursor::KeepAnchor);

        kDebug() << "   - \"" << previousText << "\"\n";
        kDebug() << "   + \"" << cursor.selectedText() << "\"\n";
    }
}

#include "ChangeTracker.moc"
