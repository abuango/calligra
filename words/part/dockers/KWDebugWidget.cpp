/* This file is part of the KDE project
 * Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
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

#include "KWDebugWidget.h"
#include <KoTextEditor.h>
#include <KoParagraphStyle.h>
#include <KoSection.h>

#include <QHBoxLayout>
#include <QTimer>

KWDebugWidget::KWDebugWidget(QWidget *parent)
    : QWidget(parent)
    , m_canvas(0)
{
    initUi();
    initLayout();

    updateData();
}

KWDebugWidget::~KWDebugWidget()
{
}

void KWDebugWidget::initUi()
{
    m_label = new QLabel(this);
    m_label->setText("Some debug info will be here."); // No i18n as it's for debug only.
}

void KWDebugWidget::initLayout()
{
    QHBoxLayout *mainBox = new QHBoxLayout(this);
    mainBox->addWidget(m_label);

    setLayout(mainBox);
}

void KWDebugWidget::updateData()
{
    QTimer().singleShot(100, this, SLOT(updateData()));
    if (!isVisible()) {
        return;
    }

    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(m_canvas);
    if (!editor) {
        return;
    }

    QTextBlock curBlock = editor->block();
    QTextBlockFormat fmt = curBlock.blockFormat();

    QString willShow;
    if (fmt.hasProperty(KoParagraphStyle::SectionStartings))
    {
        willShow = "This sections starts here :";
        QVariant var = fmt.property(KoParagraphStyle::SectionStartings);
        QList<QVariant> openList = var.value< QList<QVariant> >();
        foreach (const QVariant &sv, openList)
        {
            KoSection *sec = static_cast<KoSection *>(sv.value<void *>());
            willShow += sec->name() + " ";
        }
        willShow += "\n";
    }

    if (fmt.hasProperty(KoParagraphStyle::SectionEndings))
    {
        willShow += "This sections end here :";
        QVariant var = fmt.property(KoParagraphStyle::SectionEndings);
        QList<QVariant> closeList = var.value< QList<QVariant> >();
        foreach (const QVariant &sv, closeList)
        {
            KoSectionEnd *sec = static_cast<KoSectionEnd *>(sv.value<void *>());
            willShow += sec->name + " ";
        }
        willShow += "\n";
    }

    willShow += "block number is " + QString::number(editor->constCursor().block().blockNumber()) + "\n";
    willShow += "cur pos " + QString::number(editor->constCursor().position()) + "\n";
    willShow += "pos in block " + QString::number(editor->constCursor().positionInBlock()) + "\n";
    willShow += "length of block " + QString::number(editor->constCursor().block().length()) + "\n";

    m_label->setText(willShow);
}

void KWDebugWidget::setCanvas(KWCanvas* canvas)
{
    m_canvas = canvas;
}

void KWDebugWidget::unsetCanvas()
{
}
