/* This file is part of the KDE project
 * Copyright (C) 2000-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2011 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
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

#ifndef KWTEXTFRAMESET_H
#define KWTEXTFRAMESET_H

#include "KWFrameSet.h"
#include "../KWPageStyle.h"
#include "../kword_export.h"

class QTextDocument;
class KWPageManager;
class KWDocument;
class KWRootAreaProvider;
class KWTextFrame;

/**
 * A frameset with a TextDocument backing it.
 */
class KWORD_EXPORT KWTextFrameSet : public KWFrameSet
{
public:
    /**
     * Constructor with a type of text specified
     * @param kwordDocument the document this frameset belongs to.
     * @param type the type of frameSet; this can indicate headers, footers etc.
     */
    explicit KWTextFrameSet(KWDocument *kwordDocument, KWord::TextFrameSetType type = KWord::OtherTextFrameSet);
    ~KWTextFrameSet();

    /// return the type of frameSet this is
    KWord::TextFrameSetType textFrameSetType() {
        return m_textFrameSetType;
    }

    /// return the document with the text that belongs to this frameset.
    QTextDocument *document() const {
        return m_document;
    }

    /// return the rootAreaProvider that is responsible for providing rootArea's to the KoTextDocumentLayout for this frameset.
    KWRootAreaProvider *rootAreaProvider() const {
        return m_rootAreaProvider;
    }

    /// return the pageManager for this frameSet
    KWPageManager* pageManager() {
        return m_pageManager;
    }
    /// return the document for this frameset
    KWDocument *kwordDocument() {
        return m_kwordDocument;
    }

    void setPageStyle(const KWPageStyle &style);
    const KWPageStyle& pageStyle() const;

protected:
    friend class TestTextFrameSorting;
    friend class TestTextFrameManagement;

    void setupFrame(KWFrame *frame);

private:
    QTextDocument *m_document;
    KWord::TextFrameSetType m_textFrameSetType;
    KWPageManager *m_pageManager;
    KWDocument *m_kwordDocument;
    KWPageStyle m_pageStyle; // the page Style this frameset is associated with.
    KWRootAreaProvider *m_rootAreaProvider;
};

#endif
