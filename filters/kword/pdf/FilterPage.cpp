/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "FilterPage.h"

#include <math.h>

#include <qfontmetrics.h>
#include <kglobal.h>

#include "GfxState.h"
#include "misc.h"


//-----------------------------------------------------------------------------
int FilterParagraph::findTab(double xMin, double epsilon,
                             bool firstLine) const {
    double dx = xMin - (firstLine ? firstIndent : leftIndent);
    if ( fabs(dx)<epsilon ) return -2;
    for (uint i=0; i<tabs.size(); i++)
            if ( fabs(xMin-tabs[i])<epsilon ) return i;
    return -1;
}

uint FilterParagraph::findNbTabs(uint i, double prevXMax) const {
    uint k = 0;
    for (; k<tabs.size(); k++)
        if ( tabs[k]>prevXMax ) break;
    if ( k>i ) return 0;
    return i-k+1;
}

//-----------------------------------------------------------------------------
FilterString::FilterString(GfxState *state, double x0, double y0,
                           double fontSize)
    : TextString(state, x0, y0, fontSize)
{
    GfxRGB rgb;
    state->getFillRGB(&rgb);
    QColor color(int(rgb.r), int(rgb.g), int(rgb.b));
    GfxFont *font = state->getFont();
    GString *gname = (font ? font->getName() : 0);
    QString name = (gname ? gname->getCString() : 0);
    _font = new FilterFont(name, qRound(fontSize), color);
    _link = 0;
}

FilterString::~FilterString()
{
    delete _font;
}

//-----------------------------------------------------------------------------
FilterPage::FilterPage(QDomDocument &document, QDomElement &mainFrameset)
    : TextPage(false), _document(document), _mainFrameset(mainFrameset)
{}

FilterPage::~FilterPage()
{
    for (uint i=0; i<_links.size(); i++)
        delete _links[i];
}

void FilterPage::beginString(GfxState *state, double x0, double y0) {
  // This check is needed because Type 3 characters can contain
  // text-drawing operations.
  if (curStr) {
    ++nest;
    return;
  }

  curStr = new FilterString(state, x0, y0, fontSize);
}

void FilterPage::createParagraph(const QString &text,
                                 const QValueVector<QDomElement> &layouts,
                                 const QValueVector<QDomElement> &formats)
{
    QDomElement paragraph = _document.createElement("PARAGRAPH");
    _mainFrameset.appendChild(paragraph);

    QDomElement textElement = _document.createElement("TEXT");
    textElement.appendChild( _document.createTextNode(text) );
    paragraph.appendChild(textElement);

    QDomElement layout = _document.createElement("LAYOUT");
    paragraph.appendChild(layout);
    QDomElement element = _document.createElement("NAME");
    element.setAttribute("value", "Standard");
    layout.appendChild(element);
    for (uint i=0; i<layouts.count(); i++)
        layout.appendChild(layouts[i]);

    if ( formats.count() ) {
        QDomElement format = _document.createElement("FORMATS");
        for (uint i=0; i<formats.count(); i++)
            format.appendChild(formats[i]);
        paragraph.appendChild(format);
    }
}

void FilterPage::prepare()
{
    // associate links to strings
    for (uint i=0; i<_links.size(); i++)
        for (TextLine *line = lines; line; line = line->next)
            for (TextBlock *blk = line->blocks; blk; blk = blk->next)
                for (TextString *str = blk->strings; str; str = str->next)
                    if ( _links[i]->inside(str->xMin, str->xMax, str->yMin,
                                           str->yMax) )
                        static_cast<FilterString *>(str)->_link = _links[i];

    // create paragraphs
    FilterParagraph par;
    par.nbLines = 1;
    for (TextLine *line = lines; line; line = line->next) {
        // compute indents
        double xMin = line->blocks->xMin;
        if ( par.nbLines==1 ) {
            par.firstIndent = xMin;
            par.leftIndent = xMin;
        } else par.leftIndent = kMin(par.leftIndent, xMin);

        // compute tabulations
        for (TextBlock *blk = line->blocks; blk; blk = blk->next) {
            double epsilon = 0.1 * (line->yMax-line->yMin);
            int res = par.findTab(blk->xMin, epsilon, par.nbLines==1);
            if ( res==-1 ) par.tabs.append(blk->xMin);
        }
        qHeapSort(par.tabs);

        // new paragraph ?
        if ( line->next==0 || (line->next->yMin - line->yMax) >
             0.5*(line->next->yMax - line->next->yMin) ) {
            _pars.append(par);
            par.nbLines = 1;
            par.tabs.clear();
        } else par.nbLines++;
    }

    // fill paragraphs
    double height = 0;
    TextLine *line = lines;
    for (uint i=0; i<_pars.size(); i++) {
        uint pos = 0;

        _pars[i].offset = line->yMin - height;
        if ( _pars[i].offset>0 ) height += _pars[i].offset;

        for (uint l=0; l<_pars[i].nbLines; l++) {
            // end of previous line (inside a paragraph)
            if ( l!=0 ) {
                FilterBlock b;
                b.text = '\n';
                b.pos = pos;
                b.font =
                    static_cast<FilterString *>(line->blocks->strings)->_font;
                _pars[i].blocks.append(b);
                pos++;
            }

            int lineHeight = 0;
            TextBlock *prevBlk = 0;
            for (TextBlock *blk = line->blocks; blk; blk = blk->next) {
                // tabulations
                double epsilon = 0.1 * (line->yMax-line->yMin);
                int res = _pars[i].findTab(blk->xMin, epsilon, l==0);
                if ( res>=0 ) {
                    if (prevBlk) res = _pars[i].findNbTabs(res, prevBlk->xMax);
                    else res++;
                    for (uint k=0; k<uint(res); k++) {
                        FilterBlock b;
                        b.text = '\t';
                        b.pos = pos;
                        b.font =
                            static_cast<FilterString *>(blk->strings)->_font;
                        _pars[i].blocks.append(b);
                        pos++;
                    }
                }

                // text & format
                for (TextString *str = blk->strings; str; str = str->next) {
                    QString tmp;
                    for (uint k = 0; k<uint(str->len); k++)
                        tmp += QChar(str->text[k]);
                    if (str->spaceAfter) tmp += ' ';

                    FilterBlock b;
                    b.pos = pos;
                    FilterString *fstr = static_cast<FilterString *>(str);
                    b.font = fstr->_font;
                    if (fstr->_link) {
                        b.text = '#';
                        b.link = fstr->_link;
                        b.linkText = tmp;
                        pos++;
                    } else {
                        b.text = tmp;
                        pos += tmp.length();
                    }
                    _pars[i].blocks.append(b);

                    QFontMetrics fm( b.font->font() );
                    lineHeight = kMax(lineHeight, fm.lineSpacing());
                }

                prevBlk = blk;
            }

            height += lineHeight;
            line = line->next;
        }
    }

    // coalesce formats
    int dec = 0;
    for (uint i=0; i<_pars.size(); i++) {
        QValueVector<FilterBlock> blocks;
        blocks.append(_pars[i].blocks[0]);
        for (uint k=1; k<_pars[i].blocks.size(); k++) {
            FilterBlock &b = _pars[i].blocks[k];
            b.pos += dec;
            if ( (b.link==blocks.last().link) &&
                 (*b.font)==(*blocks.last().font) ) {
                if (b.link) {
                    blocks.last().linkText += b.linkText;
                    dec--;
                } else blocks.last().text += b.text;
            } else blocks.append(b);
        }
        _pars[i].blocks = blocks;
    }
}

void FilterPage::dump()
{
    // add paragraphs
    for (uint i=0; i<_pars.size(); i++) {
        QValueVector<QDomElement> layouts;
        QValueVector<QDomElement> formats;

        // tabulations
        for (uint k=0; k<_pars[i].tabs.size(); k++) {
            QDomElement element = _document.createElement("TABULATOR");
            element.setAttribute("type", 0);
            element.setAttribute("ptpos", _pars[i].tabs[k]);
            element.setAttribute("width", 0);
            element.setAttribute("filling", 0);
            layouts.append(element);
        }

        // indents
        QDomElement element = _document.createElement("INDENTS");
        element.setAttribute("left", _pars[i].leftIndent);
        double dx = _pars[i].firstIndent - _pars[i].leftIndent;
        if ( dx!=0 ) element.setAttribute("first", dx);
        layouts.append(element);

        // offset before
        if ( _pars[i].offset>0 ) {
            QDomElement element = _document.createElement("OFFSETS");
            element.setAttribute("before", _pars[i].offset);
            layouts.append(element);
        }

        // page break
        if ( (i+1)==_pars.size() ) {
            QDomElement element = _document.createElement("PAGEBREAKING");
            element.setAttribute("hardFrameBreakAfter", "true");
            layouts.append(element);
        }

        // formats
        QString text;
        for (uint k=0; k<_pars[i].blocks.size(); k++) {
            const FilterBlock &b = _pars[i].blocks[k];
            text += b.text;
            QDomElement element = _document.createElement("FORMAT");
            bool r =
                b.font->format(_document, element, b.pos, b.text.length());
            if (b.link) b.link->format(_document, element, b.pos, b.linkText);
            if ( r || b.link ) formats.append(element);
        }

        createParagraph(text, layouts, formats);
    }
}
