/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qheader.h>
#include <qevent.h>

#include <klocale.h>
#include <kdebug.h>
#include <kpushbutton.h>
#include <kiconloader.h>

#include "kexipropertyeditoritem.h"
#include "kexipropertybuffer.h"
#include "propertyeditorlist.h"
#include "propertyeditorinput.h"
#include "propertyeditorfile.h"
#include "propertyeditorfont.h"
#include "propertyeditordate.h"

#include "kexipropertyeditor.h"


KexiPropertyEditor::KexiPropertyEditor(QWidget *parent, bool returnToAccept, bool AutoSync, const char *name)
 : KListView(parent, name)
{
	addColumn(i18n("Property"), 145);
	addColumn(i18n("Value"), 100);

	m_currentEditor = 0;
	m_buffer = 0;
	m_topItem = 0;
	m_returnToAccept = returnToAccept;
	m_sync = AutoSync;

	connect(this, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(slotClicked(QListViewItem *)));
	connect(header(), SIGNAL(sizeChange(int, int, int)), this, SLOT(slotColumnSizeChanged(int, int, int)));
	connect(header(), SIGNAL(clicked(int)), this, SLOT(moveEditor()));
	connect(header(), SIGNAL(sectionHandleDoubleClicked (int)), this, SLOT(slotColumnSizeChanged(int)));

	m_defaults = new KPushButton(viewport());
	m_defaults->setPixmap(SmallIcon("reload"));
	m_defaults->hide();
	connect(m_defaults, SIGNAL(clicked()), this, SLOT(resetItem()));

	setFullWidth(true);
	setShowSortIndicator(true);
	setItemMargin(3);
}


void
KexiPropertyEditor::slotClicked(QListViewItem *item)
{
	if(item)
	{
		int y = viewportToContents(QPoint(0, itemRect(item).y())).y();
		kdDebug() << "KexiPropertyEditor::slotClicked() y: " << y << endl;
		QRect g(columnWidth(0), y, columnWidth(1), item->height());
		KexiPropertyEditorItem *i = static_cast<KexiPropertyEditorItem *>(item);
		createEditor(i, g);
	}
}

void
KexiPropertyEditor::createEditor(KexiPropertyEditorItem *i, const QRect &geometry)
{
	kdDebug() << "KexiPropertyEditor::createEditor: Create editor for type " << i->type() << endl;
	if(m_currentEditor)
	{
		slotEditorAccept(m_currentEditor);
		delete m_currentEditor;
	}
	
	m_defaults->hide();

	KexiPropertySubEditor *editor=0;
	switch(i->type())
	{
		case QVariant::Bool:
			editor = new PropertyEditorBool(viewport(), i->property());
			break;

		case QVariant::String:
		case QVariant::CString:
			editor = new PropertyEditorInput(viewport(), i->property());
			break;
   
		case QVariant::Int:
			editor = new PropertyEditorSpin(viewport(), i->property());
			break;
			
		case QVariant::Double:
			editor = new PropertyEditorDblSpin(viewport(), i->property());
			break;

		case QVariant::StringList:
			if(i->property()->value().type() == QVariant::StringList)
				editor = new PropertyEditorMultiList(viewport(), i->property());
			else
				editor = new PropertyEditorList(viewport(), i->property());
			break;

		case QVariant::BitArray:
//			editor = new EventEditorEditor(viewport(), i);
			break;
			
		case QVariant::Font:
			editor = new PropertyEditorFont(viewport(), i->property());
			break;
			
		case QVariant::Pixmap:
			editor = new PropertyEditorPixmap(viewport(), i->property());
			break;
			
		case QVariant::Color:
			editor = new PropertyEditorColor(viewport(), i->property());
			break;
			
		case QVariant::Date:
			editor = new PropertyEditorDate(viewport(), i->property());
			break;
		
		case QVariant::Time:
			editor = new PropertyEditorTime(viewport(), i->property());
			break;

		case QVariant::DateTime:
			editor = new PropertyEditorDateTime(viewport(), i->property());
			break;
	
		case QVariant::Cursor:
			editor = new PropertyEditorCursor(viewport(), i->property());
			break;

		default:
			m_currentEditor = 0;
			setFocus();
			kdDebug() << "PropertyEditor::createEditor: No editor created!" << endl;
			return;
	}

	connect(editor, SIGNAL(reject(KexiPropertySubEditor *)), this,
		SLOT(slotEditorReject(KexiPropertySubEditor *)));

	if(m_returnToAccept)
	{
		connect(editor, SIGNAL(accept(KexiPropertySubEditor *)), this,
			SLOT(slotEditorAccept(KexiPropertySubEditor *)));
	}

	connect(editor, SIGNAL(changed(KexiPropertySubEditor *)), this,
		SLOT(slotValueChanged(KexiPropertySubEditor *)));
	if(!i->modified())
	{
	editor->setGeometry(geometry);
	editor->resize(geometry.width(), geometry.height());
	}
	else
	{
	m_defaults->resize(geometry.height(), geometry.height());
	QPoint p = contentsToViewport(QPoint(0, geometry.y()));
	m_defaults->move(geometry.x() + geometry.width() - m_defaults->width(), p.y());
	editor->resize(geometry.width()-m_defaults->width(), geometry.height());
	m_defaults->show();
	}
	editor->show();
	addChild(editor);
	moveChild(editor, geometry.x(), geometry.y());
	
	editor->setFocus();

	m_currentEditor = editor;
	m_editItem = i;
}

void
KexiPropertyEditor::slotValueChanged(KexiPropertySubEditor *editor)
{
	if(m_currentEditor)
	{
		QVariant value = m_currentEditor->getValue();
		m_editItem->setValue(value);
		if(m_buffer && m_sync)
		{
		if(m_editItem->depth()==1)
			m_buffer->changeProperty(m_editItem->name().latin1(), value);
		else if(m_editItem->depth()==2)
		{
			KexiPropertyEditorItem *parent = static_cast<KexiPropertyEditorItem*>(m_editItem->parent());
			m_buffer->changeProperty(parent->name().latin1(), parent->getComposedValue());
		}
		}
		else
		if(m_editItem->depth()==2)
		{
			KexiPropertyEditorItem *parent = static_cast<KexiPropertyEditorItem*>(m_editItem->parent());
			parent->getComposedValue();
		}
		emit valueChanged(m_editItem->text(0), value);
	}

	if(!m_returnToAccept)
	{
		emit itemRenamed(m_editItem);
	}
}

void
KexiPropertyEditor::slotEditorAccept(KexiPropertySubEditor *editor)
{
	if(m_currentEditor)
	{
		QVariant value = m_currentEditor->getValue();
		m_editItem->setValue(value);
		if(m_buffer)
		{
			if(m_editItem->depth()==1)
				m_buffer->changeProperty(m_editItem->name().latin1(), value);
			else if(m_editItem->depth()==2)
			{
				KexiPropertyEditorItem *parent = static_cast<KexiPropertyEditorItem*>(m_editItem->parent());
				m_buffer->changeProperty(parent->name().latin1(), parent->getComposedValue());
			}
		}
//		m_currentEditor->hide();
//		m_currentEditor->clearFocus();
		emit valueChanged(m_editItem->text(0), value);
	}
}

void
KexiPropertyEditor::slotEditorReject(KexiPropertySubEditor *editor)
{
	if(m_currentEditor)
	{
		if(!m_sync)
		{
			m_editItem->setValue(m_editItem->property()->value());
			m_currentEditor->setValue(m_editItem->property()->value());
		}
		else
		{
			m_editItem->setValue(m_editItem->oldValue());
			m_currentEditor->setValue(m_editItem->oldValue());
		}
	}
//	editor->hide();
//	editor->setFocusPolicy(QWidget::NoFocus);
}

void
KexiPropertyEditor::slotColumnSizeChanged(int section, int, int newS)
{
	if(m_currentEditor)
	{
		if(section == 0)
		{
			m_currentEditor->move(newS, m_currentEditor->y());
		}
		else
		{
		if(m_defaults->isVisible())
			m_currentEditor->resize(newS - m_defaults->width(), m_currentEditor->height());
		else
			m_currentEditor->resize(newS, m_currentEditor->height());
		}
	}
}

void
KexiPropertyEditor::slotColumnSizeChanged(int section)
{
	setColumnWidth(1, viewport()->width() - columnWidth(0));
	slotColumnSizeChanged(section, 0, header()->sectionSize(section));
	if(m_currentEditor)
	{
		if(m_defaults->isVisible())
			m_currentEditor->resize(columnWidth(1) - m_defaults->width(), m_currentEditor->height());
		else
			m_currentEditor->resize(columnWidth(1), m_currentEditor->height());
	}
}

void
KexiPropertyEditor::reset(bool editorOnly)
{
	delete m_currentEditor;
	m_currentEditor = 0;
	if(m_defaults->isVisible())
		m_defaults->hide();

	if(!editorOnly)
	{
		clear();
		m_topItem = 0;
	}
}

QSize KexiPropertyEditor::sizeHint() const
{
	if (firstChild())
		 return QSize(KListView::sizeHint().width(), firstChild()->height()*childCount());
	return KListView::sizeHint();
}

void
KexiPropertyEditor::setBuffer(KexiPropertyBuffer *b)
{
	m_buffer = b; 
	fill(); 
}

void
KexiPropertyEditor::fill()
{
	reset(false);

	KexiPropertyBuffer::Iterator it;
	
	if(!m_topItem)
	{
	m_topItem = new KexiPropertyEditorItem(this,"Top Item");
	}
	
	for(it = m_buffer->begin(); it != m_buffer->end(); ++it)
	{
		new KexiPropertyEditorItem(m_topItem, &(it.data()) );
	}
}

void
KexiPropertyEditor::resetItem()
{
	if(m_editItem)
	{
	if(m_currentEditor)
		m_currentEditor->setValue(m_editItem->oldValue());
	else
		m_editItem->setValue(m_editItem->oldValue());
	}
}

void
KexiPropertyEditor::moveEditor()
{
	QPoint p = contentsToViewport(QPoint(0, itemPos(m_editItem)));
	if(m_currentEditor)
		m_currentEditor->move(m_currentEditor->x(), p.y());
	if(m_defaults->isVisible())
		m_defaults->move(m_defaults->x(), p.y());
}

void
KexiPropertyEditor::resizeEvent(QResizeEvent *ev)
{
	KListView::resizeEvent(ev);
	if(m_defaults->isVisible())
	{
		QRect r = itemRect(m_editItem);
		if(r.y()) // r.y() == 0 if the item is not visible on the screen
			m_defaults->move(r.x() + r.width() - m_defaults->width(), r.y());
		if(m_currentEditor)
			m_currentEditor->resize(columnWidth(1) - m_defaults->width(), m_currentEditor->height());
	}
	
}

KexiPropertyEditor::~KexiPropertyEditor()
{
}


#include "kexipropertyeditor.moc"
