#include "sidebar.h"
#include "kpresenter_doc.h"
#include <qheader.h>

SideBar::SideBar( QWidget *parent, KPresenterDoc *d )
    : KListView( parent ), doc( d )
{
    rebuildItems();
    setSorting( -1 );
    header()->hide();
    addColumn( i18n( "Slide" ) );
    addColumn( i18n( "Number" ) );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );
    connect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
}

void SideBar::rebuildItems()
{
    QMap< QString, bool > checkedMap;
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it )
	checkedMap.insert( it.current()->text( 0 ), ( (QCheckListItem*)it.current() )->isOn() );
    
    clear();
    for ( int i = doc->getPageNums() - 1; i >= 0; --i ) {
	QCheckListItem *item = new QCheckListItem( this, "", QCheckListItem::CheckBox );
	QString title = doc->getPageTitle( i, i18n( "Slide %1" ).arg( i + 1 ) );
	QMap< QString, bool >::Iterator bit;
	item->setOn( ( bit = checkedMap.find( title ) ) != checkedMap.end() ? *bit : TRUE );
	item->setText( 1, QString( "%1" ).arg( i + 1 ) );
	item->setText( 0, title );
    }
    setCurrentItem( firstChild() );
    setSelected( firstChild(), TRUE );
}

void SideBar::itemClicked( QListViewItem *i )
{
    if ( !i )
	return;
    emit showPage( i->text( 1 ).toInt() - 1 );
}


void SideBar::setCurrentPage( int pg )
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	if ( it.current()->text( 1 ).toInt() - 1 == pg ) {
	    setCurrentItem( it.current() );
	    setSelected( it.current(), TRUE );
	}
    }
}

void SideBar::setOn( int pg, bool on )
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	if ( it.current()->text( 1 ).toInt() - 1 == pg ) {
	    ( (QCheckListItem*)it.current() )->setOn( on );
	    return;
	}
    }
}
