/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <dfaure@kde.org>

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

#include <xmltree.h>

XMLTree::XMLTree(const QString & inputFileName)
{
  root = new QDomDocument("spreadsheet");

  QDomProcessingInstruction pro;
  pro = root->createProcessingInstruction("xml", "version=\"1.0\"");
  root->appendChild(pro);

  doc = root->createElement("spreadsheet");
  doc.setAttribute("editor", "KSpread CSV Filter");
  doc.setAttribute("mime", "application/x-kspread");
  root->appendChild(doc);

  QDomElement paper = root->createElement("PAPER");
  paper.setAttribute("format", "A4");
  paper.setAttribute("orientation", "Portrait");
  doc.appendChild(paper);

  map = root->createElement("MAP");
  doc.appendChild(map);

  table = root->createElement("TABLE");
  table.setAttribute("name", inputFileName);
  map.appendChild(table);

  row = 1;
  column = 1;
}

XMLTree::~XMLTree() 
{
  if(root)
    delete root;
}

const QString XMLTree::part()
{
  QString s;
  QTextStream t(s, IO_WriteOnly);

  t << "<?xml version=\"1.0\"?>\n";
  doc.save(t);
  t << '\0';

  return s;
}

bool XMLTree::cell( const QString & contents )
{
  QDomElement e = root->createElement("CELL");
  //e.appendChild(getFormat(xf));
  //e.appendChild(getFont(xf));

  e.setAttribute("row", row);
  e.setAttribute("column", column++);
  e.appendChild(root->createTextNode(contents));

  table.appendChild(e);

  return true;
}
