#ifndef TABLEOFCONTENTSPREVIEW_H
#define TABLEOFCONTENTSPREVIEW_H
/* This file is part of the KDE project
 * Copyright (C) 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
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

#include <KoZoomHandler.h>
#include <KoInlineTextObjectManager.h>

#include <QFrame>

class TextShape;
class KoTableOfContentsGeneratorInfo;
class KoStyleManager;

class TableOfContentsPreview : public QFrame
{
    Q_OBJECT
public:
    explicit TableOfContentsPreview(QWidget *parent = 0);
    void setStyleManager(KoStyleManager *styleManager);

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:
    void updatePreview(KoTableOfContentsGeneratorInfo *info);

private slots:
    void finishedPreviewLayout();

private:
    TextShape *m_textShape;
    QPixmap *m_pm;
    KoZoomHandler m_zoomHandler;
    KoStyleManager *m_styleManager;
    KoInlineTextObjectManager m_itom;

};

#endif // TABLEOFCONTENTSPREVIEW_H
