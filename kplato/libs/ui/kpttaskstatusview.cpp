/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskstatusview.h"
#include "kpttaskstatusmodel.h"

#include "kptglobal.h"
#include "kptitemviewsettup.h"
#include "kptcommand.h"
#include "kptproject.h"
#include "kptschedule.h"

#include <KoDocument.h>

#include <QAbstractItemModel>
#include <QApplication>
#include <QComboBox>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QHeaderView>
#include <QItemDelegate>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QMap>
#include <QMenu>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QVBoxLayout>
#include <QWidget>

#include <kicon.h>
#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <ktoggleaction.h>
#include <kactionmenu.h>
#include <kstandardaction.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>
#include <kactioncollection.h>

namespace KPlato
{


TaskStatusTreeView::TaskStatusTreeView( KoDocument *part, QWidget *parent )
    : DoubleTreeViewBase( parent )
{
    setContextMenuPolicy( Qt::CustomContextMenu );
    setModel( new TaskStatusItemModel( part ) );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setStretchLastSection( false );
    
    connect( this, SIGNAL( activated ( const QModelIndex ) ), this, SLOT( slotActivated( const QModelIndex ) ) );

    QList<int> lst1; lst1 << 1 << -1;
    QList<int> lst2;
    for ( int i = 0; i < 38; ++i ) {
        lst2 << i;
    }
    lst2 << 48 << -1;
    hideColumns( lst1, lst2 );
}

TaskStatusItemModel *TaskStatusTreeView::itemModel() const
{
    return static_cast<TaskStatusItemModel*>( model() );
}

Project *TaskStatusTreeView::project() const
{
    return itemModel()->project();
}

void TaskStatusTreeView::setProject( Project *project )
{
    itemModel()->setProject( project );
}

void TaskStatusTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<index.column();
}

void TaskStatusTreeView::dragMoveEvent(QDragMoveEvent *event)
{
/*    if (dragDropMode() == InternalMove
        && (event->source() != this || !(event->possibleActions() & Qt::MoveAction)))
        return;

    TreeViewBase::dragMoveEvent( event );
    if ( ! event->isAccepted() ) {
        return;
    }
    //QTreeView thinks it's ok to drop
    event->ignore();
    QModelIndex index = indexAt( event->pos() );
    if ( ! index.isValid() ) {
        event->accept();
        return; // always ok to drop on main project
    }
    Node *dn = itemModel()->node( index );
    if ( dn == 0 ) {
        kError()<<"no node to drop on!"<<endl;
        return; // hmmm
    }
    switch ( dropIndicatorPosition() ) {
        case AboveItem:
        case BelowItem:
            //dn == sibling
            if ( itemModel()->dropAllowed( dn->parentNode(), event->mimeData() ) ) {
                event->accept();
            }
            break;
        case OnItem:
            //dn == new parent
            if ( itemModel()->dropAllowed( dn, event->mimeData() ) ) {
                event->accept();
            }
            break;
        default:
            break;
    }*/
}


//-----------------------------------
TaskStatusView::TaskStatusView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent ),
    m_id( -1 )
{
    kDebug()<<"-------------------- creating TaskStatusView -------------------"<<endl;
    setupGui();

    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new TaskStatusTreeView( part, this );
    l->addWidget( m_view );

    connect( m_view, SIGNAL( contextMenuRequested( const QModelIndex&, const QPoint& ) ), SLOT( slotContextMenuRequested( const QModelIndex&, const QPoint& ) ) );
    
    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void TaskStatusView::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

void TaskStatusView::updateReadWrite( bool rw )
{
    m_view->setReadWrite( rw );
}

void TaskStatusView::slotCurrentScheduleManagerChanged( ScheduleManager *sm )
{
    //kDebug()<<endl;
    static_cast<TaskStatusItemModel*>( m_view->model() )->setManager( sm );
}

Node *TaskStatusView::currentNode() const 
{
    Node * n = m_view->itemModel()->node( m_view->selectionModel()->currentIndex() );
    if ( n && n->type() != Node::Type_Task ) {
        return 0;
    }
    return n;
}

void TaskStatusView::setProject( Project *project )
{
    m_project = project;
    m_view->itemModel()->setProject( m_project );
}

void TaskStatusView::draw( Project &project )
{
    setProject( &project );
}

void TaskStatusView::setGuiActive( bool activate )
{
    kDebug()<<activate;
//    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
}

void TaskStatusView::slotContextMenuRequested( const QModelIndex &index, const QPoint& pos )
{
    kDebug()<<index<<pos<<endl;
    if ( ! index.isValid() ) {
        return;
    }
    Node *node = m_view->itemModel()->node( index );
    if ( node == 0 ) {
        return;
    }
    slotContextMenuRequested( node, pos );
}

void TaskStatusView::slotContextMenuRequested( Node *node, const QPoint& pos )
{
    kDebug()<<node->name()<<" :"<<pos;
    QString name;
    switch ( node->type() ) {
        case Node::Type_Task:
            name = "taskstatus_popup";
            break;
        case Node::Type_Milestone:
            break;
        case Node::Type_Summarytask:
            break;
        default:
            break;
    }
    kDebug()<<name;
    if ( name.isEmpty() ) {
        return;
    }
    emit requestPopupMenu( name, pos );
}

void TaskStatusView::setupGui()
{
    // Add the context menu actions for the view options
    actionOptions = new KAction(KIcon("configure"), i18n("Configure..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void TaskStatusView::slotOptions()
{
    kDebug();
    ItemViewSettupDialog dlg( m_view->slaveView() );
    dlg.exec();
}

bool TaskStatusView::loadContext( const KoXmlElement &context )
{
    kDebug()<<endl;
    return m_view->loadContext( context );
}

void TaskStatusView::saveContext( QDomElement &context ) const
{
    m_view->saveContext( context );
}



} // namespace KPlato

#include "kpttaskstatusview.moc"
