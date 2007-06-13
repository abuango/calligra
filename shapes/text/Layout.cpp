/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
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

#include "Layout.h"
#include "ListItemsHelper.h"
#include "TextShape.h"

#include <KoTextDocumentLayout.h>
#include <KoTextShapeData.h>
#include <KoParagraphStyle.h>
#include <KoCharacterStyle.h>
#include <KoListStyle.h>
#include <KoStyleManager.h>
#include <KoTextBlockData.h>
#include <KoTextBlockBorderData.h>
#include <KoShape.h>
#include <KoUnit.h>

#include <kdeversion.h>
#include <KDebug>
#include <QTextList>
#include <QStyle>

// #define DEBUG_TABS

// ---------------- layout helper ----------------
Layout::Layout(KoTextDocumentLayout *parent)
   : m_styleManager(0),
    m_blockData(0),
    m_data(0),
    m_reset(true),
    m_isRtl(false),
    m_demoText(false),
    m_endOfDemoText(false),
    m_defaultTabSizing(MM_TO_POINT(15))
{
    m_parent = parent;
    layout = 0;
    for(int i=0; i < 2000; i++)
        m_lotsOfTabs.append((double)i);
}

bool Layout::start() {
    if(m_reset)
        resetPrivate();
    else if(shape)
        nextParag();
    m_reset = false;
    return !(layout == 0 || m_parent->shapes().count() <= shapeNumber);
}

void Layout::end() {
    if(layout)
        layout->endLayout();
    layout = 0;
}

void Layout::reset() {
    m_reset = true;
}

bool Layout::interrupted() {
    return m_reset;
}

double Layout::width() {
    Q_ASSERT(shape);
    double ptWidth = shape->size().width() - m_format.leftMargin() - m_format.rightMargin();
    if(m_newParag)
        ptWidth -= m_format.textIndent();
    if(m_newParag && m_blockData)
        ptWidth -= m_blockData->counterWidth() + m_blockData->counterSpacing();
    ptWidth -= m_borderInsets.left + m_borderInsets.right + m_shapeBorder.right;
    return ptWidth;
}

double Layout::x() {
    double result = m_newParag?m_format.textIndent():0.0;
    result += m_isRtl ? m_format.rightMargin() : m_format.leftMargin();
    result += listIndent();
    result += m_borderInsets.left + m_shapeBorder.left;
    return result;
}

double Layout::y() {
    return m_y;
}

double Layout::docOffsetInShape() const {
    Q_ASSERT(m_data);
    return m_data->documentOffset();
}

bool Layout::addLine(QTextLine &line) {
    m_currentParagTabsData.append(applyTabs(line));

    double height = m_format.doubleProperty(KoParagraphStyle::FixedLineHeight);
    double objectHeight = 0.0;
    bool useFixedLineHeight = height != 0.0;
    if(! useFixedLineHeight) {
        const bool useFontProperties = m_format.boolProperty(KoParagraphStyle::LineSpacingFromFont);
        if(useFontProperties)
            height = line.height();
        else {
            if(m_fragmentIterator.atEnd()) // no text in parag.
                height = m_block.charFormat().fontPointSize();
            else {
                // read max font height
                height = qMax(height, m_fragmentIterator.fragment().charFormat().fontPointSize());
                objectHeight= qMax(objectHeight, inlineCharHeight(m_fragmentIterator.fragment()));
                while(! (m_fragmentIterator.atEnd() || m_fragmentIterator.fragment().contains(
                               m_block.position() + line.textStart() + line.textLength() -1))) {
                    m_fragmentIterator++;
                    height = qMax(height, m_fragmentIterator.fragment().charFormat().fontPointSize());
                    objectHeight= qMax(objectHeight, inlineCharHeight(m_fragmentIterator.fragment()));
                }
            }
            if(height < 0.01) height = 12; // default size for uninitialized styles.
        }
    }

    if(m_data->documentOffset() + shape->size().height() < m_y + height + m_shapeBorder.bottom) {
        // line does not fit.
        m_data->setEndPosition(m_block.position() + line.textStart()-1);

        bool ignoreLine = false;
        if(! m_newParag && m_format.nonBreakableLines()) { // then revert layouting of parag
            // TODO check height of parag so far; and if it does not fit in the rest of the shapes, just continue.
            m_data->setEndPosition(m_block.position() -1);
            m_block.layout()->endLayout();
            m_block.layout()->beginLayout();
            ignoreLine = true;
        }
        nextShape();
        if(m_data)
            m_data->setPosition(m_block.position() + ignoreLine?0:line.textStart());

        // the demo-text feature means we have exactly the same amount of text as we have frame-space
        if(m_demoText)
            m_endOfDemoText = true;
        return !ignoreLine;
    }

    // add linespacing
    if(! useFixedLineHeight) {
        double linespacing = m_format.doubleProperty(KoParagraphStyle::LineSpacing);;
        if(linespacing == 0.0) { // unset
            int percent = m_format.intProperty(KoParagraphStyle::PercentLineHeight);
            if(percent != 0)
                linespacing = height * ((percent - 100) / 100.0);
            else if(linespacing == 0.0)
                linespacing = height * 0.2; // default
        }
        height = qMax(height, objectHeight) + linespacing;
    }

    double minimum = m_format.doubleProperty(KoParagraphStyle::MinimumLineHeight);
    if(minimum > 0.0)
        height = qMax(height, minimum);
    if(qAbs(m_y - line.y()) < 0.126) // rounding problems due to Qt-scribe internally using ints.
        m_y += height;
    else
        m_y = line.y() + height; // The line got a pos <> from y(), follow that lead.
    m_newShape = false;
    m_newParag = false;
    return false;
}

bool Layout::nextParag() {
    m_inlineObjectHeights.clear();
    if(layout) { // guard against first time
        layout->endLayout();
        m_block = m_block.next();
        if(m_endOfDemoText) {
            layout = 0;
            m_blockData = 0;
            return false;
        }
        double borderBottom = m_y;
        if(m_block.isValid() && !m_newShape) { // only add bottom of prev parag if we did not go to a new shape for this parag.
            if(m_format.pageBreakPolicy() == QTextFormat::PageBreak_AlwaysAfter ||
                    m_format.boolProperty(KoParagraphStyle::BreakAfter)) {
                m_data->setEndPosition(m_block.position()-1);
                nextShape();
                if(m_data)
                    m_data->setPosition(m_block.position());
            }
            m_y += m_borderInsets.bottom;
            borderBottom = m_y; // don't inlude the bottom margin!
            m_y += m_format.bottomMargin();
        }
        if(m_blockData == 0) { // see if one is needed due to tabs data
            bool dataRequired = false;
            foreach(KoTextBlockData::TabLineData tab, m_currentParagTabsData) {
                if(tab.tabs.count() > 0) {
                    dataRequired = true;
                    break;
                }
            }
            if(dataRequired) {
                m_blockData = new KoTextBlockData();
                m_block.previous().setUserData(m_blockData);
            }
        }

        if(m_blockData) {
            m_blockData->setTabLineData(m_currentParagTabsData);
            if(m_blockData->border())
                m_blockData->border()->setParagraphBottom(borderBottom);
        }
    }
    layout = 0;
    m_blockData = 0;
    m_currentParagTabsData.clear();
    if(m_data == 0) // no shape to layout, so stop here.
        return true;
    if(! m_block.isValid()) {
        QTextBlock block = m_block.previous(); // last correct one.
        m_data->setEndPosition(block.position() + block.length());

        // repaint till end of shape.
        const double offsetInShape = m_y - m_data->documentOffset();
        shape->repaint(QRectF(0.0, offsetInShape, shape->size().width(), shape->size().width() - offsetInShape));
        // cleanup and repaint rest of shapes.
        cleanupShapes();
        return false;
    }
    m_format = m_block.blockFormat();
    m_blockData = dynamic_cast<KoTextBlockData*> (m_block.userData());
    KoParagraphStyle::Direction dir = static_cast<KoParagraphStyle::Direction> (m_format.intProperty(KoParagraphStyle::TextProgressionDirection));
    if(dir == KoParagraphStyle::AutoDirection)
        m_isRtl = m_block.text().isRightToLeft();
    else
        m_isRtl =  dir == KoParagraphStyle::RightLeftTopBottom;

    // initialize list item stuff for this parag.
    QTextList *textList = m_block.textList();
    if(textList) {
        QTextListFormat format = textList->format();
        int styleId = format.intProperty(KoListStyle::CharacterStyleId);
        KoCharacterStyle *charStyle = 0;
        if(styleId > 0 && m_styleManager)
            charStyle = m_styleManager->characterStyle(styleId);
        if(!charStyle && m_styleManager) { // try the one from paragraph style
            KoParagraphStyle *ps = m_styleManager->paragraphStyle(
                    m_format.intProperty(KoParagraphStyle::StyleId));
            if(ps)
                charStyle = ps->characterStyle();
        }

        if(! (m_blockData && m_blockData->hasCounterData())) {
            QFont font;
            if(charStyle)
                font = QFont(charStyle->fontFamily(), qRound(charStyle->fontPointSize()),
                        charStyle->fontWeight(), charStyle->fontItalic());
            else {
                QTextCursor cursor(m_block);
                font = cursor.charFormat().font();
            }
            ListItemsHelper lih(textList, font);
            lih.recalculate();
            m_blockData = dynamic_cast<KoTextBlockData*> (m_block.userData());
        }
    }
    else if(m_blockData) { // make sure it is empty
        m_blockData->setCounterText(QString());
        m_blockData->setCounterSpacing(0.0);
        m_blockData->setCounterWidth(0.0);
    }

    updateBorders(); // fill the border inset member vars.
    m_y += m_borderInsets.top;

    if(!m_newShape && (m_format.pageBreakPolicy() == QTextFormat::PageBreak_AlwaysBefore ||
            m_format.boolProperty(KoParagraphStyle::BreakBefore))) {
        m_data->setEndPosition(m_block.position()-1);
        nextShape();
        if(m_data)
            m_data->setPosition(m_block.position());
    }
    m_y += topMargin();
    layout = m_block.layout();
    QTextOption options = layout->textOption();
    options.setTabArray(m_lotsOfTabs);
    options.setWrapMode(QTextOption::WordWrap);
    options.setAlignment( QStyle::visualAlignment(m_isRtl ? Qt::RightToLeft : Qt::LeftToRight, m_format.alignment()) );
    if(m_isRtl)
        options.setTextDirection(Qt::RightToLeft);
    layout->setTextOption(options);

    layout->beginLayout();
    m_fragmentIterator = m_block.begin();
    m_newParag = true;

    if(textList) {
        // if list set list-indent. Do this after borders init to we can account for them.
        // Also after we account for indents etc so the y() pos is correct.
        if(m_isRtl)
            m_blockData->setCounterPosition(QPointF(shape->size().width() - m_borderInsets.right -
                m_shapeBorder.right - m_format.leftMargin() - m_blockData->counterWidth(), y()));
        else
            m_blockData->setCounterPosition(QPointF(m_borderInsets.left + m_shapeBorder.left +
                        m_format.textIndent() + m_format.leftMargin() , y()));
    }

    return true;
}

double Layout::documentOffsetInShape() {
    return m_data->documentOffset();
}

void Layout::nextShape() {
    m_newShape = true;

    if(m_data) {
        Q_ASSERT(m_data->endPosition() >= m_data->position());
        m_y = m_data->documentOffset() + shape->size().height() + 10.0;
        m_data->wipe();
    }

    shape = 0;
    m_data = 0;

    QList<KoShape *> shapes = m_parent->shapes();
    for(shapeNumber++; shapeNumber < shapes.count(); shapeNumber++) {
        shape = shapes[shapeNumber];
        m_data = dynamic_cast<KoTextShapeData*> (shape->userData());
        if(m_data != 0)
            break;
        shape = 0;
        m_data = 0;
    }

    if(shape == 0)
        return;
    m_data->setDocumentOffset(m_y);
    m_data->faul(); // make dirty since this one needs relayout at this point.
    m_shapeBorder = shape->borderInsets();
    m_y += m_shapeBorder.top;
}

// and the end of text, make sure the rest of the frames have something sane to show.
void Layout::cleanupShapes() {
    int i = shapeNumber + 1;
    QList<KoShape *> shapes = m_parent->shapes();
    while(i < shapes.count())
        cleanupShape(shapes[i++]);
}

void Layout::cleanupShape(KoShape *daShape) {
    KoTextShapeData *textData = dynamic_cast<KoTextShapeData*> (daShape->userData());
    if(textData == 0)
        return;
    if(textData->position() == -1)
        return;
    textData->setPosition(-1);
    textData->setDocumentOffset(m_y + 10);
    textData->wipe();
    daShape->repaint();
}

double Layout::listIndent() {
    if(m_blockData == 0)
        return 0;
    if(m_isRtl)
        return 0;
    return m_blockData->counterWidth();
}

void Layout::resetPrivate() {
    m_demoText = false;
    m_endOfDemoText = false;
    m_y = 0;
    m_data = 0;
    shape =0;
    layout = 0;
    m_newShape = true;
    m_blockData = 0;
    m_newParag = true;
    m_block = m_parent->document()->begin();

    shapeNumber = 0;
    int lastPos = -1;
    QList<KoShape *> shapes = m_parent->shapes();
    foreach(KoShape *shape, shapes) {
        KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (shape->userData());
        Q_ASSERT(data);
        if(data->isDirty()) {
            // this shape needs to be recalculated.
            data->setPosition(lastPos+1);
            m_block = m_parent->document()->findBlock( lastPos+1 );
            m_y = data->documentOffset();
            m_format = m_block.blockFormat();

            if(shapeNumber == 0) {
                // no matter what the previous data says, just start from zero.
                m_y = 0;
                data->setDocumentOffset(0);
                Q_ASSERT(lastPos == -1);
                break;
            }
            if(m_block.layout() && m_block.layout()->lineCount() > 0) {
                // block has been layouted. So use its offset.
                m_y = m_block.layout()->lineAt(0).position().y();
                if(m_y < data->documentOffset() - 0.126) { // 0.126 to account of rounding in Qt-scribe
                    Q_ASSERT(shapeNumber > 0);
                    // since we only recalc whole parags; we need to go back a little.
                    shapeNumber--;
                    shape = shapes[shapeNumber];
                    data = dynamic_cast<KoTextShapeData*> (shape->userData());
                    m_newShape = false;
                }
                if(m_y > data->documentOffset() + shape->size().height()) {
                    // hang on; this line is explicitly placed outside the shape. Shape is empty!
                    m_y = data->documentOffset();
                    break;
                }
                // in case this parag has a border we have to subtract that as well
                m_blockData = dynamic_cast<KoTextBlockData*> (m_block.userData());
                if(m_blockData && m_blockData->border()) {
                    double top = m_blockData->border()->inset(KoTextBlockBorderData::Top);
                    // but only when this border actually makes us have an indent.
                    if(qAbs(m_blockData->border()->rect().top() + top - m_y) < 1E-10)
                        m_y -= top;
                }
                // subtract the top margins as well.
                m_y -= topMargin();
            }
            break;
        }
        lastPos = data->endPosition();
        shapeNumber++;
    }
    Q_ASSERT(shapeNumber >= 0);
    if(shapes.count() == 0)
        return;
    shape = shapes[shapeNumber];
    m_demoText = (static_cast<TextShape*> (shape))->demoText();
    m_data = dynamic_cast<KoTextShapeData*> (shape->userData());
    m_shapeBorder = shape->borderInsets();
    if(m_y == 0)
        m_y = m_shapeBorder.top;

   if(! nextParag())
       shapeNumber++;
}

void Layout::updateBorders() {
    Q_ASSERT(m_data);
    m_borderInsets = m_data->shapeMargins();
    m_borderInsets.top += m_format.doubleProperty(KoParagraphStyle::TopPadding);
    m_borderInsets.left += m_format.doubleProperty(KoParagraphStyle::LeftPadding);
    m_borderInsets.bottom += m_format.doubleProperty(KoParagraphStyle::BottomPadding);
    m_borderInsets.right += m_format.doubleProperty(KoParagraphStyle::RightPadding);

    KoTextBlockBorderData border(QRectF(this->x() - listIndent(), m_y + m_borderInsets.top + topMargin(), width(), 1.));
    border.setEdge(border.Left, m_format, KoParagraphStyle::LeftBorderStyle,
        KoParagraphStyle::LeftBorderWidth, KoParagraphStyle::LeftBorderColor,
        KoParagraphStyle::LeftBorderSpacing, KoParagraphStyle::LeftInnerBorderWidth);
    border.setEdge(border.Right, m_format, KoParagraphStyle::RightBorderStyle,
        KoParagraphStyle::RightBorderWidth, KoParagraphStyle::RightBorderColor,
        KoParagraphStyle::RightBorderSpacing, KoParagraphStyle::RightInnerBorderWidth);
    border.setEdge(border.Top, m_format, KoParagraphStyle::TopBorderStyle,
        KoParagraphStyle::TopBorderWidth, KoParagraphStyle::TopBorderColor,
        KoParagraphStyle::TopBorderSpacing, KoParagraphStyle::TopInnerBorderWidth);
    border.setEdge(border.Bottom, m_format, KoParagraphStyle::BottomBorderStyle,
        KoParagraphStyle::BottomBorderWidth, KoParagraphStyle::BottomBorderColor,
        KoParagraphStyle::BottomBorderSpacing, KoParagraphStyle::BottomInnerBorderWidth);

    // check if prev parag had a border.
    QTextBlock prev = m_block.previous();
    KoTextBlockBorderData *prevBorder = 0;
    if(prev.isValid()) {
        KoTextBlockData *bd = dynamic_cast<KoTextBlockData*> (prev.userData());
        if(bd)
            prevBorder = bd->border();
    }
    if(border.hasBorders()) {
        if(m_blockData == 0) {
            m_blockData = new KoTextBlockData();
            m_block.setUserData(m_blockData);
        }

        // then check if we can merge with the previous parags border.
        if(prevBorder && prevBorder->equals(border))
            m_blockData->setBorder(prevBorder);
        else {
            // can't merge; then these are our new borders.
            KoTextBlockBorderData *newBorder = new KoTextBlockBorderData(border);
            m_blockData->setBorder(newBorder);
            if(prevBorder && !m_newShape)
                m_y += prevBorder->inset(KoTextBlockBorderData::Bottom);
        }
        m_blockData->border()->applyInsets(m_borderInsets, m_y + m_borderInsets.top, false);
    }
    else { // this parag has no border.
        if(prevBorder && !m_newShape)
            m_y += prevBorder->inset(KoTextBlockBorderData::Bottom);
        if(m_blockData)
            m_blockData->setBorder(0); // remove an old one, if there was one.
    }
}

double Layout::topMargin() {
    bool allowMargin = true; // wheather to allow margins at top of shape
    if(m_newShape) {
        allowMargin = false; // false by default, but check 2 exceptions.
        if(m_format.boolProperty(KoParagraphStyle::BreakBefore))
            allowMargin = true;
        else if( m_styleManager && m_format.topMargin() > 0) {
            // also allow it when the paragraph has the margin, but the style has a different one.
            KoParagraphStyle *ps = m_styleManager->paragraphStyle(
                    m_format.intProperty(KoParagraphStyle::StyleId));
            if(ps == 0 || ps->topMargin() != m_format.topMargin())
                allowMargin = true;
        }
    }
    if(allowMargin)
        return m_format.topMargin();
    return 0.0;
}

void Layout::draw(QPainter *painter, const QAbstractTextDocumentLayout::PaintContext &context) {
    painter->setPen(context.palette.color(QPalette::Text)); // for text that has no color.
    const QRegion clipRegion = painter->clipRegion();
    QTextBlock block = m_parent->document()->begin();
    KoTextBlockBorderData *lastBorder = 0;
    bool started=false;
    int selectionStart = -1, selectionEnd = -1;
    if(context.selections.count()) {
        QTextCursor cursor = context.selections[0].cursor;
        selectionStart = cursor.position();
        selectionEnd = cursor.anchor();
        if(selectionStart > selectionEnd)
            qSwap(selectionStart, selectionEnd);
    }

    while(block.isValid()) {
        QTextLayout *layout = block.layout();

        if(!painter->hasClipping() || ! clipRegion.intersect(QRegion(layout->boundingRect().toRect())).isEmpty()) {
            started=true;
            painter->save();
            decorateParagraph(painter, block);
            painter->restore();

#if 0       // code for drawing using the normal QTextLayout::draw().  Can't use it due to tabs.

            QVector<QTextLayout::FormatRange> selections;
            foreach(QAbstractTextDocumentLayout::Selection selection, context.selections) {
                QTextCursor cursor = selection.cursor;
                int begin = cursor.position();
                int end = cursor.anchor();
                if(begin > end)
                    qSwap(begin, end);

               if(end < block.position() || begin > block.position() + block.length())
                   continue; // selection does not intersect this block.
                QTextLayout::FormatRange fr;
                fr.start = begin - block.position();
                fr.length = end - begin;
                fr.format = selection.format;
                selections.append(fr);
            }
            layout->draw(painter, QPointF(0,0), selections);
#endif
            //drawParagraph(painter, layout, blockData, selectionStart - block.position(), selectionEnd - block.position());
            drawParagraph(painter, block, selectionStart - block.position(), selectionEnd - block.position());

            KoTextBlockBorderData *border = 0;
            KoTextBlockData *blockData = dynamic_cast<KoTextBlockData*> (block.userData());
            if(blockData)
                border = dynamic_cast<KoTextBlockBorderData*> (blockData->border());
            if(lastBorder && lastBorder != border) {
                painter->save();
                lastBorder->paint(*painter);
                painter->restore();
            }
            lastBorder = border;
        }
        else if(started) // when out of the cliprect, then we are done drawing.
            break;
        block = block.next();
    }
    if(lastBorder)
        lastBorder->paint(*painter);
}

void Layout::drawParagraph(QPainter *painter, const QTextBlock &block, int selectionStart, int selectionEnd) {
    // this method replaces QTextLayout::draw() because we need to do some stuff per line for tabs. :/
    QTextLayout *layout = block.layout();
    QList<KoTextBlockData::TabLineData> tabsData;
    KoTextBlockData *data = dynamic_cast<KoTextBlockData*> (block.userData());
    if(data)
        tabsData = data->tabLineData();
    QTextOption textOption = layout->textOption();

    QList<KoText::Tab> tabFormat;
    QVariant variant = block.blockFormat().property(KoParagraphStyle::TabPositions);
    if(! variant.isNull())
        foreach(QVariant tab, qvariant_cast<QList<QVariant> >(variant))
            tabFormat.append(tab.value<KoText::Tab>());


    for(int i=0; i < layout->lineCount(); i++) {
        KoTextBlockData::TabLineData tabs;
        if(tabsData.count() > i) {
            tabs = tabsData[i];
            textOption.setTabArray(tabs.tabs);
            layout->setTextOption(textOption);
        }
        QTextLine line = layout->lineAt(i);
        if(line.textStart() < selectionEnd && line.textStart() + line.textLength() > selectionStart) {
            // paint selection!
            const double x1 = line.cursorToX(qMax(selectionStart, line.textStart()));
            const double x2 = line.cursorToX(qMin(selectionEnd, line.textStart() + line.textLength()));
            QRectF rect(line.position().x() + x1, line.position().y(), x2 - x1, line.height());
            painter->fillRect(rect, QBrush(QColor(255, 255, 0, 130))); // TODO use proper selection color
        }

        line.draw(painter, layout->position());
        
        QTextCharFormat fmt = block.charFormat();
        if (fmt.intProperty(KoCharacterStyle::FontStrikeOutStyle) != 0) {
            double x1 = line.cursorToX(line.textStart());
            double x2 = line.cursorToX(line.textStart() + line.textLength());
            double y = line.position().y() + line.height()/2;
            QPen penBackup = painter->pen();
            QPen pen = painter->pen();
            pen.setStyle((Qt::PenStyle) fmt.intProperty(KoCharacterStyle::FontStrikeOutStyle));
            painter->setPen((Qt::PenStyle) fmt.intProperty(KoCharacterStyle::FontStrikeOutStyle));
            painter->drawLine(x1, y, x2, y);
            painter->setPen(penBackup);
        }

        for(int x=0; x < tabs.tabLength.count(); x++) { // fill tab-gaps
            const double tabStop = tabs.tabs[x];
            const double pos = tabStop - tabs.tabLength[x];
            QRectF tabArea(layout->position() + line.position() + QPointF(pos, 0),
                    QSizeF(tabs.tabLength[x], line.ascent()));

            KoText::Tab tab;
            // choose the one with a position just bigger (or equal) to tabstop.
            for(int i = tabFormat.count()-1; i >= 0; i--) {
                KoText::Tab t = tabFormat[i];
                if(tabStop > t.position)
                    break;
                tab = t;
            }
            if(tab.position == 0) // can't do anything if there is no tab-defintion
                continue;

            QPen pen;
            switch(tab.leaderStyle) {
            case QTextCharFormat::SingleUnderline: pen = QPen(Qt::SolidLine); break;
            case QTextCharFormat::DashUnderline: pen = QPen(Qt::DashLine); break;
            case QTextCharFormat::DotLine: pen = QPen(Qt::DotLine); break;
            case QTextCharFormat::DashDotLine: pen = QPen(Qt::DashDotLine); break;
            case QTextCharFormat::DashDotDotLine: pen = QPen(Qt::DashDotDotLine); break;
            case QTextCharFormat::WaveUnderline:
                // TODO
                continue;
            case QTextCharFormat::NoUnderline:
                // fall through
            default:
                continue; // do nothing!
            }

            painter->save();
            pen.setCapStyle(Qt::FlatCap);
            pen.setWidthF(1.); // TODO set proper width
            if(tab.leaderColor.isValid())
                pen.setColor(tab.leaderColor);
            else { // fetch color from text
                QTextCursor cursor(block);
                cursor.setPosition(line.xToCursor(tabArea.left()));
                QTextCharFormat cf = cursor.blockCharFormat();
                pen.setColor(cf.foreground().color());
            }
            painter->setPen(pen);
            const int y = qRound(tabArea.bottom() - pen.widthF() / 2.0);
            painter->drawLine(qRound(tabArea.left()), y, (int) tabArea.right(), y);
            painter->restore();
        }
    }
}

void Layout::decorateParagraph(QPainter *painter, const QTextBlock &block) {
    KoTextBlockData *data = dynamic_cast<KoTextBlockData*> (block.userData());
    if(data == 0)
        return;
    
    QTextList *list = block.textList();
    if(list && data->hasCounterData()) {
        QTextListFormat listFormat = list->format();
        QTextCharFormat cf;
        bool filled=false;
        if(m_styleManager) {
            const int id = listFormat.intProperty(KoListStyle::CharacterStyleId);
            KoCharacterStyle *cs = m_styleManager->characterStyle(id);
            if(cs) {
                cs->applyStyle(cf);
                filled = true;
            }
        }
        if(! filled) {
            // use first char of block.
            QTextCursor cursor(block); // I know this is longwinded, but just using the blocks
            // charformat does not work, apparantly
            cf = cursor.charFormat();
        }
        if(! data->counterText().isEmpty()) {
            QFont font(cf.font(), m_parent->paintDevice());
            QTextLayout layout(data->counterText(), font, m_parent->paintDevice());
            layout.setCacheEnabled(true);
            QList<QTextLayout::FormatRange> layouts;
            QTextLayout::FormatRange format;
            format.start=0;
            format.length=data->counterText().length();
            format.format = cf;
            layouts.append(format);
            layout.setAdditionalFormats(layouts);

            Qt::Alignment align = static_cast<Qt::Alignment> (listFormat.intProperty(KoListStyle::Alignment));
            if(align == 0)
                align = Qt::AlignLeft;
            else if(align != Qt::AlignAuto)
                align |= Qt::AlignAbsolute;
            QTextOption option( align );
            option.setTextDirection(block.layout()->textOption().textDirection());
            if(option.textDirection() == Qt::RightToLeft || data->counterText().isRightToLeft())
                option.setAlignment(Qt::AlignRight);
            layout.setTextOption(option);
            layout.beginLayout();
            QTextLine line = layout.createLine();
            line.setLineWidth(data->counterWidth() - data->counterSpacing());
            layout.endLayout();
            layout.draw(painter, data->counterPosition());
        }

        KoListStyle::Style listStyle = static_cast<KoListStyle::Style> ( listFormat.style() );
        if(listStyle == KoListStyle::SquareItem || listStyle == KoListStyle::DiscItem ||
                listStyle == KoListStyle::CircleItem || listStyle == KoListStyle::BoxItem ||
                listStyle == KoListStyle::RhombusItem ||
                listStyle == KoListStyle::HeavyCheckMarkItem || listStyle == KoListStyle::BallotXItem ||
                listStyle == KoListStyle::RightArrowItem || listStyle == KoListStyle::RightArrowHeadItem
        ) {
            QFontMetricsF fm(cf.font(), m_parent->paintDevice());
#if 0
// helper lines to show the anatomy of this font.
painter->setPen(Qt::green);
painter->drawLine(QLineF(-1, data->counterPosition().y(), 200, data->counterPosition().y()));
painter->setPen(Qt::yellow);
painter->drawLine(QLineF(-1, data->counterPosition().y() + fm.ascent() - fm.xHeight(), 200, data->counterPosition().y() + fm.ascent() - fm.xHeight()));
painter->setPen(Qt::blue);
painter->drawLine(QLineF(-1, data->counterPosition().y() + fm.ascent(), 200, data->counterPosition().y() + fm.ascent()));
painter->setPen(Qt::gray);
painter->drawLine(QLineF(-1, data->counterPosition().y() + fm.height(), 200, data->counterPosition().y() + fm.height()));
#endif

            double width = fm.xHeight();
            double y = data->counterPosition().y() + fm.ascent() - fm.xHeight(); // at top of text.
            int percent = listFormat.intProperty(KoListStyle::BulletSize);
            if(percent > 0)
                width *= percent / 100.0;
            y -= width / 10.; // move it up just slightly
            double x = qMax(1., data->counterPosition().x() + fm.width(listFormat.stringProperty( KoListStyle::ListItemPrefix )));
                switch( listStyle ) {
                case KoListStyle::SquareItem: {
                    painter->fillRect(QRectF(x, y, width, width), QBrush(Qt::black));
                } break;
                case KoListStyle::DiscItem:
                    painter->setBrush(QBrush(Qt::black));
                    // fall through!
                case KoListStyle::CircleItem: {
                    painter->drawEllipse(QRectF(x, y, width, width));
                } break;
                case KoListStyle::BoxItem: {
                    painter->drawRect(QRectF(x, y, width, width));
                } break;
                case KoListStyle::RhombusItem: {
                    painter->translate(QPointF(x+(width/2.0),y));
                    painter->rotate(45.0);
                    painter->fillRect(QRectF(0, 0, width, width), QBrush(Qt::black));
                } break;
                case KoListStyle::RightArrowItem: {
                    const double half = width/2.0;
                    painter->translate(QPointF(x,y));
                    QPointF points[] = { QPointF(half,0), QPointF(width,half), QPointF(half,width) };
                    painter->drawPolyline(points,3);
                    painter->drawLine(QLineF(0,half,width,half));
                } break;
                case KoListStyle::RightArrowHeadItem: {
                    painter->translate(QPointF(x,y));
                    QPointF points[] = { QPointF(0,0), QPointF(width,width/2.0), QPointF(0,width) };
                    painter->drawPolyline(points,3);
                } break;
                case KoListStyle::HeavyCheckMarkItem: {
                    const double half = width/2.0;
                    painter->translate(QPointF(x,y));
                    QPointF points[] = { QPointF(half,half), QPointF(half,width), QPointF(width,0) };
                    painter->drawPolyline(points,3);
                } break;
                case KoListStyle::BallotXItem: {
                    painter->translate(QPointF(x,y));
                    painter->drawLine(QLineF(0.0,0.0,width,width));
                    painter->drawLine(QLineF(0.0,width,width,0.0));
                } break;
                default:; // others we ignore.
            }
        }
    }
}

bool Layout::setFollowupShape(KoShape *followupShape) {
    if(m_demoText)
        return false;
    Q_ASSERT(shape == 0);
    Q_ASSERT(followupShape);

    m_data = dynamic_cast<KoTextShapeData*> (followupShape->userData());
    if(m_data == 0)
        return false;

    m_newShape = false;
    shape = followupShape;
    m_data->setDocumentOffset(m_y);
    m_shapeBorder = shape->borderInsets();
    return true;
}

void Layout::clearTillEnd() {
    QTextBlock block = m_block.next();
    while(block.isValid()) {
        if(block.layout()->lineCount() == 0)
            return;
        // erase the layouted lines
        block.layout()->beginLayout();
        block.layout()->endLayout();
        block = block.next();
    }
}

int Layout::cursorPosition() const {
    int answer = m_block.position();
    if(!m_newParag && layout && layout->lineCount()) {
        QTextLine tl = layout->lineAt(layout->lineCount() -1);
        answer += tl.textStart() + tl.textLength() - 1;
    }
    return answer;
}

bool Layout::previousParag() {
    if(m_block.position() == 0 && layout->lineCount() == 0)
        return false;

    layout->endLayout();
    if(layout->lineCount() == 0) {
        m_block = m_block.previous();
        layout = m_block.layout();
    }
    QTextLine tl = layout->lineAt(0);
    Q_ASSERT(tl.isValid());
    m_y = tl.y();

    m_format = m_block.blockFormat();
    m_blockData = dynamic_cast<KoTextBlockData*> (m_block.userData());
    m_isRtl = m_block.text().isRightToLeft();

    m_fragmentIterator = m_block.begin();
    m_newParag = true;

    if(m_data->position() > m_block.position()) { // go back a shape.
        QList<KoShape *> shapes = m_parent->shapes();
        for(--shapeNumber; shapeNumber >= 0; shapeNumber--) {
            shape = shapes[shapeNumber];
            m_data = dynamic_cast<KoTextShapeData*> (shape->userData());
            if(m_data != 0)
                break;
        }
        Q_ASSERT(m_data); // should never happen since the first shape is always a proper shape.
        if(m_data == 0)
            return false;

        m_shapeBorder = shape->borderInsets();
    }
    m_newShape = m_block.position() == m_data->position();
    updateBorders(); // fill the border inset member vars.
    layout->beginLayout();
    return true;
}

KoTextBlockData::TabLineData Layout::applyTabs(QTextLine &line) {
#ifdef DEBUG_TABS
    kDebug() << "applyTabs (" << line.textStart() << ")--------------------\n";
#endif
    Q_ASSERT(m_block.layout());
    QTextOption textOption = m_block.layout()->textOption();
    QVariant variant = m_format.property(KoParagraphStyle::TabPositions);
    if(variant.isNull()) {
        KoTextBlockData::TabLineData data;
        if(m_block.text().mid(line.textStart(), line.textLength()).contains('\t')) {
            QList<double> tabs;
            for(double i =m_defaultTabSizing; i <= line.width(); i+= m_defaultTabSizing)
                tabs.append(i);
            data.tabs = tabs;
            textOption.setTabArray(data.tabs);
            m_block.layout()->setTextOption(textOption);

            line.setLineWidth(line.width()); // relayout line
        }
        return data;
    }
    Qt::Alignment paragAlign = textOption.alignment();
    if(paragAlign != Qt::AlignLeft) {
        textOption.setAlignment(Qt::AlignLeft);
        layout->setTextOption(textOption);
    }

#ifdef DEBUG_TABS
{
    QString line1, line2;
    const int end = line.textStart() + line.textLength();
    for(int i=line.textStart(); i < end; i++) {
        QChar c= m_block.text().at(i);
        if(c == '\t')
            c = QChar(' ');
        line1 += "   " + QString(c);
        QString number = QString::number(line.cursorToX(i));
        if(number.length() == 1)
            line2 += "  ";
        else if(number.length() == 2)
            line2 += ' ';
        line2 += ' '+ number;
    }
    kDebug() << line1 << endl;
    kDebug() << line2 << endl;
}
#endif

    class TabsHelper {
      public:
        TabsHelper(const QTextLine &line, const QVariant &variant, const QString &paragText)
            : m_line(line),
            m_initialized(false),
            m_dirty(false),
            m_variant(variant),
            m_offset(0.),
            m_paragText(paragText) {}

        void insertTab(int cursorPosition) {
//kDebug() << "insertTab " << cursorPosition << endl;
            calculateTabSize(cursorPosition);
            if(! m_initialized) {
                m_initialized = true;
                foreach(QVariant tab, qvariant_cast<QList<QVariant> >(m_variant))
{//KoText::Tab foo = tab.value<KoText::Tab>(); kDebug() << "  found tab : " << foo.position << endl;
                    m_tabs.append(tab.value<KoText::Tab>());
}
            }
            double pos = m_line.cursorToX(cursorPosition) + m_offset;
            foreach(KoText::Tab tab, m_tabs) {
                if(tab.position >= pos) {
                    if(tab.type == KoText::LeftTab) {
                        m_tabData.tabs.append(tab.position);
                        const double length = tab.position - pos;
                        m_tabData.tabLength.append(length);
                        m_offset += length;
                        return;
                    }
                    // delay the rest until we know where the break off point is.
                    m_dirty = true;
                    m_previousPosition = cursorPosition;
                    m_previousTab = tab;
                }
            }
        }

        const KoTextBlockData::TabLineData &tabLineData() {
            calculateTabSize(m_line.textStart() + m_line.textLength());
            return m_tabData;
        }

      private:
        void calculateTabSize(int cursorPosition) {
            if(! m_dirty) return;
//kDebug() << "calculateTabSize cursor pos: " << m_previousPosition << "|" << cursorPosition << endl;
            // the alignment is done on the block of text between the prev and current tab position
            const double startSection = m_line.cursorToX(m_previousPosition+1) + m_offset;
            const double endSection = m_line.cursorToX(cursorPosition) + m_offset;
            const bool currentNotInLine = cursorPosition > m_line.textStart() + m_line.textLength();
//kDebug() << "  startSection: " << startSection << ", endSection: " << endSection << endl;

            bool convertToSpace = false;
            if(startSection > m_line.width()) { // tab found will not end up in line.
                m_tabData.tabs.append(m_line.width());
                m_tabData.tabLength.append(0);
            }
            else if(currentNotInLine) // aligned portion is too big for line
                convertToSpace = true;
            else if(m_previousTab.type == KoText::RightTab) {
                if(endSection > m_previousTab.position)
                    convertToSpace = true;
                else {
//kDebug() << "    " << m_previousTab.position << " " << m_previousTab.position - (endSection - startSection) << endl;
                    m_tabData.tabs.append(m_previousTab.position - (endSection - startSection));
                    m_tabData.tabLength.append(m_tabData.tabs.last() - m_line.cursorToX(m_previousPosition) - m_offset);
                    m_offset += m_previousTab.position - endSection;
                }
            }
            else if(m_previousTab.type == KoText::CenterTab) {
//kDebug() << "    " << m_previousTab.position << " " << m_previousTab.position - (endSection - startSection) << endl;
                // TODO check fit
                m_tabData.tabs.append(m_previousTab.position - (endSection - startSection) / 2.0);
                m_tabData.tabLength.append(m_tabData.tabs.last() - m_line.cursorToX(m_previousPosition) + m_offset);
                m_offset += m_previousTab.position + (endSection - startSection) / 2.0 - endSection;
            }
            else if(m_previousTab.type == KoText::DelimiterTab) {
                // TODO check fit
                const int index = m_paragText.indexOf(m_previousTab.delimiter, m_previousPosition);
                const double charPos = m_line.cursorToX(index);
                const double wantedPos = m_offset + charPos + (m_line.cursorToX(index + 1) - charPos)  / 2.0; // half on the char
                m_tabData.tabs.append(m_previousTab.position - (wantedPos - startSection));
                m_tabData.tabLength.append(m_tabData.tabs.last() - m_line.cursorToX(m_previousPosition) + m_offset);
                m_offset += m_tabData.tabLength.last() - 1;
            }

            if(convertToSpace) { // aligned text doesn't fit, so make the tab a normal space.
                m_tabData.tabs.append(startSection + 4); // TODO replace 4 with width of a space
                m_tabData.tabLength.append(4);
                m_offset += 4 - 1;
            }

            m_dirty = false;
        }

        QTextLine m_line;
        bool m_initialized, m_dirty;
        QVariant m_variant;
        QList<KoText::Tab> m_tabs;
        KoText::Tab m_previousTab;
        int m_previousPosition;
        double m_offset;
        KoTextBlockData::TabLineData m_tabData;
        const QString m_paragText;
    };

    QString paragText = m_block.text();
    TabsHelper tabsHelper(line, variant, paragText);

    const int end = line.textStart() + line.textLength();
    for(int i=line.textStart(); i < paragText.length(); i++) {
        if(paragText.at(i) == '\t') {
            tabsHelper.insertTab(i);
            if(i >= end)
                break;
        }
    }

    if(paragAlign != Qt::AlignLeft || tabsHelper.tabLineData().tabs.count() > 0) {
#ifdef DEBUG_TABS
{ KoTextBlockData::TabLineData foo = tabsHelper.tabLineData(); Q_ASSERT(foo.tabs.count() == foo.tabLength.count());
 for(int i=0; i < foo.tabs.count(); i++){ printf("tab: %0.4f, length: %0.4f\n", foo.tabs[i], foo.tabLength[i]); }
}
#endif
        textOption.setTabArray(tabsHelper.tabLineData().tabs);
        textOption.setAlignment(paragAlign);
        m_block.layout()->setTextOption(textOption);

        line.setLineWidth(line.width()); // relayout line

#ifdef DEBUG_TABS
        {
        QString line1, line2;
        const int end = line.textStart() + line.textLength();
        for(int i=line.textStart(); i < end; i++) {
            QChar c= m_block.text().at(i);
            if(c == '\t')
                c = QChar(' ');
            line1 += "   " + QString(c);
            QString number = QString::number(line.cursorToX(i));
            if(number.length() == 1)
                line2 += "  ";
            else if(number.length() == 2)
                line2 += ' ';
            line2 += ' '+ number;
        }
        kDebug() << line1 << endl;
        kDebug() << line2 << endl;
        }
#endif
        textOption.setTabArray(m_lotsOfTabs);
        m_block.layout()->setTextOption(textOption);
    }

    return tabsHelper.tabLineData();
}

void Layout::registerInlineObject(const QTextInlineObject &inlineObject) {
    m_inlineObjectHeights.insert(m_block.position() + inlineObject.textPosition(), inlineObject.height());
}

double Layout::inlineCharHeight(const QTextFragment &fragment) {
    if(m_inlineObjectHeights.contains(fragment.position()))
        return m_inlineObjectHeights[fragment.position()];
    return 0.0;
}

