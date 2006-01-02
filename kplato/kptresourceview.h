/* This file is part of the KDE project
   Copyright (C) 2003 - 2004 Dag Andersen <kplato@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTRESOURCEVIEW_H
#define KPTRESOURCEVIEW_H

#include <qsplitter.h>
#include <qdatetime.h>

#include "kptcontext.h"

class KListView;

class QPoint;
class QListViewItem;

class KPrinter;

namespace KPlato
{

class View;
class Project;
class Resource;
class Node;

class ResourceAppointmentsView;
class ResourceGroup;
class Resource;
class ResourceItemPrivate;


 class ResourceView : public QSplitter
{
    Q_OBJECT

 public:
    ResourceView(View *view, QWidget *parent);

    //~ResourceView();

    void zoom(double zoom);

    void draw(Project &project);
    View *mainView();

    Resource *currentResource();

    void print(KPrinter &printer);

    Node *currentNode() const { return m_currentNode; }
    
    virtual bool setContext(Context::Resourceview &context);
    virtual void getContext(Context::Resourceview &context) const;

protected slots:
    void resSelectionChanged();
    void resSelectionChanged(QListViewItem *item);
    void popupMenuRequested(QListViewItem * item, const QPoint & pos, int);

private:
    void drawResources(QListViewItem *parent, ResourceGroup *group);

private:
	View *m_mainview;
    int m_defaultFontSize;

    ResourceItemPrivate *m_selectedItem;
    KListView *resList;
    ResourceAppointmentsView *m_appview;
    Node *m_currentNode;
    QDate m_start;
    QDate m_end;

};

}  //KPlato namespace

#endif
