/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <wtrobin@carinthia.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <powerpointfilter.h>
#include <powerpointfilter.moc>

const QDomDocument * const PowerPointFilter::part() {
    
    m_part=QDomDocument("presenter");
    m_part.setContent(QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<DOC author=\"Reginald Stadlbauer\" email=\"reggie@kde.org\" editor=\"KPresenter\" mime=\"application/x-kpresenter\" url=\"/home/koffice/sorry.kpr\">"
            "<PAPER format=\"5\" ptWidth=\"680\" ptHeight=\"510\" mmWidth =\"240\" mmHeight=\"180\" inchWidth =\"9.44882\" inchHeight=\"7.08661\" orientation=\"0\" unit=\"0\">"
            "<PAPERBORDERS mmLeft=\"0\" mmTop=\"0\" mmRight=\"0\" mmBottom=\"0\" ptLeft=\"0\" ptTop=\"0\" ptRight=\"0\" ptBottom=\"0\" inchLeft=\"0\" inchTop=\"0\" inchRight=\"0\" inchBottom=\"0\"/>"
            "</PAPER>"
            "<BACKGROUND rastX=\"10\" rastY=\"10\" bred=\"255\" bgreen=\"255\" bblue=\"255\">"
            "<PAGE>"
            "<BACKTYPE value=\"0\"/>"
            "<BACKVIEW value=\"1\"/>"
            "<BACKCOLOR1 red=\"255\" green=\"255\" blue=\"255\"/>"
            "<BACKCOLOR2 red=\"255\" green=\"255\" blue=\"255\"/>"
            "<BCTYPE value=\"0\"/>"
            "<BGRADIENT unbalanced=\"0\" xfactor=\"100\" yfactor=\"100\"/>"
            "<PGEFFECT value=\"0\"/>"
            "</PAGE>"
            "</BACKGROUND>"
            "<HEADER show=\"0\">"
            "<ORIG x=\"0\" y=\"0\"/>"
            "<SIZE width=\"-1\" height=\"-1\"/>"
            "<SHADOW distance=\"0\" direction=\"5\" red=\"160\" green=\"160\" blue=\"164\"/>"
            "<EFFECTS effect=\"0\" effect2=\"0\"/>"
            "<PRESNUM value=\"0\"/>"
            "<ANGLE value=\"0\"/>"
            "<FILLTYPE value=\"0\"/>"
            "<GRADIENT red1=\"255\" green1=\"0\" blue1=\"0\" red2=\"0\" green2=\"255\" blue2=\"0\" type=\"1\" unbalanced=\"0\" xfactor=\"100\" xfactor=\"100\"/>"
            "<PEN red=\"0\" green=\"0\" blue=\"0\" width=\"1\" style=\"0\"/>"
            "<BRUSH red=\"0\" green=\"0\" blue=\"0\" style=\"0\"/>"
            "<DISAPPEAR effect=\"0\" doit=\"0\" num=\"1\"/>"
            "<TEXTOBJ objType=\"0\">"
            "<ENUMLISTTYPE type=\"1\" before=\"\" after=\".\" start=\"1\" family=\"times\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"0\" green=\"0\" blue=\"0\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<PARAGRAPH horzAlign=\"0\" depth=\"0\" lineSpacing=\"0\" distBefore=\"0\" distAfter=\"0\">"
            "<LINE>"
            "<OBJ>"
            "<TYPE value=\"1\"/>"
            "<FONT family=\"utopia\" pointSize=\"20\" bold=\"1\" italic=\"0\" underline=\"0\"/>"
            "<COLOR red=\"255\" green=\"0\" blue=\"0\"/>"
            "<VERTALIGN value=\"1\"/>"
            "<TEXT> </TEXT>"
            "</OBJ>"
            "</LINE>"
            "</PARAGRAPH>"
            "</TEXTOBJ>"
            "</HEADER>"
            "<FOOTER show=\"0\">"
            "<ORIG x=\"0\" y=\"0\"/>"
            "<SIZE width=\"-1\" height=\"-1\"/>"
            "<SHADOW distance=\"0\" direction=\"5\" red=\"160\" green=\"160\" blue=\"164\"/>"
            "<EFFECTS effect=\"0\" effect2=\"0\"/>"
            "<PRESNUM value=\"0\"/>"
            "<ANGLE value=\"0\"/>"
            "<FILLTYPE value=\"0\"/>"
            "<GRADIENT red1=\"255\" green1=\"0\" blue1=\"0\" red2=\"0\" green2=\"255\" blue2=\"0\" type=\"1\" unbalanced=\"0\" xfactor=\"100\" xfactor=\"100\"/>"
            "<PEN red=\"0\" green=\"0\" blue=\"0\" width=\"1\" style=\"0\"/>"
            "<BRUSH red=\"0\" green=\"0\" blue=\"0\" style=\"0\"/>"
            "<DISAPPEAR effect=\"0\" doit=\"0\" num=\"1\"/>"
            "<TEXTOBJ objType=\"0\">"
            "<ENUMLISTTYPE type=\"1\" before=\"\" after=\".\" start=\"1\" family=\"times\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"0\" green=\"0\" blue=\"0\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"helvetica\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<PARAGRAPH horzAlign=\"0\" depth=\"0\" lineSpacing=\"0\" distBefore=\"0\" distAfter=\"0\">"
            "<LINE>"
            "<OBJ>"
            "<TYPE value=\"1\"/>"
            "<FONT family=\"utopia\" pointSize=\"20\" bold=\"1\" italic=\"0\" underline=\"0\"/>"
            "<COLOR red=\"255\" green=\"0\" blue=\"0\"/>"
            "<VERTALIGN value=\"1\"/>"
            "<TEXT> </TEXT>"
            "</OBJ>"
            "</LINE>"
            "</PARAGRAPH>"
            "</TEXTOBJ>"
            "</FOOTER>"
            "<OBJECTS>"
            "<OBJECT type=\"4\">"
            "<ORIG x=\"30\" y=\"30\"/>"
            "<SIZE width=\"611\" height=\"61\"/>"
            "<SHADOW distance=\"0\" direction=\"5\" red=\"160\" green=\"160\" blue=\"164\"/>"
            "<EFFECTS effect=\"0\" effect2=\"0\"/>"
            "<PRESNUM value=\"0\"/>"
            "<ANGLE value=\"0\"/>"
            "<FILLTYPE value=\"0\"/>"
            "<GRADIENT red1=\"255\" green1=\"0\" blue1=\"0\" red2=\"0\" green2=\"255\" blue2=\"0\" type=\"1\" unbalanced=\"0\" xfactor=\"100\" xfactor=\"100\"/>"
            "<PEN red=\"0\" green=\"0\" blue=\"0\" width=\"1\" style=\"0\"/>"
            "<BRUSH red=\"0\" green=\"0\" blue=\"0\" style=\"0\"/>"
            "<DISAPPEAR effect=\"0\" doit=\"0\" num=\"1\"/>"
            "<TEXTOBJ objType=\"0\">"
            "<ENUMLISTTYPE type=\"1\" before=\"\" after=\".\" start=\"1\" family=\"times\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"0\" green=\"0\" blue=\"0\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<PARAGRAPH horzAlign=\"1\" depth=\"0\" lineSpacing=\"0\" distBefore=\"0\" distAfter=\"0\">"
            "<LINE>"
            "<OBJ>"
            "<TYPE value=\"0\"/>"
            "<FONT family=\"utopia\" pointSize=\"48\" bold=\"1\" italic=\"0\" underline=\"0\"/>"
            "<COLOR red=\"0\" green=\"0\" blue=\"0\"/>"
            "<VERTALIGN value=\"1\"/>"
            "<TEXT>Sorry</TEXT>"
            "</OBJ>"
            "<OBJ>"
            "<TYPE value=\"1\"/>"
            "<FONT family=\"utopia\" pointSize=\"48\" bold=\"1\" italic=\"0\" underline=\"0\"/>"
            "<COLOR red=\"0\" green=\"0\" blue=\"0\"/>"
            "<VERTALIGN value=\"1\"/>"
            "<TEXT> </TEXT>"
            "</OBJ>"
            "<OBJ>"
            "<TYPE value=\"0\"/>"
            "<FONT family=\"utopia\" pointSize=\"48\" bold=\"1\" italic=\"0\" underline=\"0\"/>"
            "<COLOR red=\"0\" green=\"0\" blue=\"0\"/>"
            "<VERTALIGN value=\"1\"/>"
            "<TEXT>:(</TEXT>"
            "</OBJ>"
            "<OBJ>"
            "<TYPE value=\"1\"/>"
            "<FONT family=\"utopia\" pointSize=\"48\" bold=\"1\" italic=\"0\" underline=\"0\"/>"
            "<COLOR red=\"0\" green=\"0\" blue=\"0\"/>"
            "<VERTALIGN value=\"1\"/>"
            "<TEXT> </TEXT>"
            "</OBJ>"
            "</LINE>"
            "</PARAGRAPH>"
            "</TEXTOBJ>"
            "</OBJECT>"
            "<OBJECT type=\"4\">"
            "<ORIG x=\"30\" y=\"130\"/>"
            "<SIZE width=\"611\" height=\"341\"/>"
            "<SHADOW distance=\"0\" direction=\"5\" red=\"160\" green=\"160\" blue=\"164\"/>"
            "<EFFECTS effect=\"0\" effect2=\"0\"/>"
            "<PRESNUM value=\"0\"/>"
            "<ANGLE value=\"0\"/>"
            "<FILLTYPE value=\"0\"/>"
            "<GRADIENT red1=\"255\" green1=\"0\" blue1=\"0\" red2=\"0\" green2=\"255\" blue2=\"0\" type=\"1\" unbalanced=\"0\" xfactor=\"100\" xfactor=\"100\"/>"
            "<PEN red=\"0\" green=\"0\" blue=\"0\" width=\"1\" style=\"0\"/>"
            "<BRUSH red=\"0\" green=\"0\" blue=\"0\" style=\"0\"/>"
            "<DISAPPEAR effect=\"0\" doit=\"0\" num=\"1\"/>"
            "<TEXTOBJ objType=\"2\">"
            "<ENUMLISTTYPE type=\"1\" before=\"\" after=\".\" start=\"1\" family=\"times\" pointSize=\"12\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"0\" green=\"0\" blue=\"0\"/>"
            "<UNSORTEDLISTTYPE family=\"utopia\" pointSize=\"20\" bold=\"1\" italic=\"0\" underline=\"0\" red=\"0\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<UNSORTEDLISTTYPE family=\"times\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" red=\"255\" green=\"0\" blue=\"0\" chr=\"45\"/>"
            "<PARAGRAPH horzAlign=\"0\" depth=\"0\" lineSpacing=\"0\" distBefore=\"0\" distAfter=\"0\">"
            "<LINE>"
            "<OBJ>"
            "<TYPE value=\"1\"/>"
            "<FONT family=\"utopia\" pointSize=\"20\" bold=\"1\" italic=\"0\" underline=\"0\"/>"
            "<COLOR red=\"0\" green=\"0\" blue=\"0\"/>"
            "<VERTALIGN value=\"1\"/>"
            "<TEXT> </TEXT>"
            "</OBJ>"
            "</LINE>"
            "</PARAGRAPH>"
            "</TEXTOBJ>"
            "</OBJECT>"
            "</OBJECTS>"
            "<INFINITLOOP value=\"0\"/>"
            "<MANUALSWITCH value=\"1\"/>"
            "<PRESSPEED value=\"1\"/>"
            "<PRESSLIDES value=\"137029784\"/>"
            "<SELSLIDES>"
            "<SLIDE nr=\"0\" show=\"1\"/>"
            "</SELSLIDES>"
            "<PIXMAPS>"
            "</PIXMAPS>"
            "<CLIPARTS>"
            "</CLIPARTS>"
            "</DOC>"));
    return &m_part;
}
