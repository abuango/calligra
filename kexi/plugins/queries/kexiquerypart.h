/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIQUERYPART_H
#define KEXIQUERYPART_H

#include <kexidialogbase.h>
#include <kexipart.h>
#include <kexipartitem.h>
#include <kexipartdatasource.h>
#include <kexidb/queryschema.h>
#include <qmap.h>

class KexiMainWin;
namespace KexiDB
{
	class QuerySchema;
	class Connection;
}

class KexiQueryDataSource;
class KexiProject;

class KexiQueryPart : public KexiPart::Part
{
	Q_OBJECT

	public:
		KexiQueryPart(QObject *parent, const char *name, const QStringList &);
		virtual ~KexiQueryPart();

		virtual bool remove(KexiMainWindow *win, KexiPart::Item &item);

		virtual KexiViewBase* createView(QWidget *parent, KexiDialogBase* dialog, 
			KexiPart::Item &item, int viewMode = Kexi::DataViewMode);

//		KexiQueryDocument	*data(KexiDB::Connection *conn, KexiPart::Item &item);

		virtual KexiPart::DataSource *dataSource();

		class TempData : public KexiDialogTempData
		{
			public:
				TempData(QObject* parent);
				KexiDB::QuerySchema *query;
				/*! true, if \a query member has changed in previous view. Used on view switching.
				 We're checking this flag to see if we should rebuild internal structure for DesignViewMode
				 of regenerate sql text in TextViewMode after switch from other view. */
				bool queryChangedInPreviousView : 1;
		};

	protected:
//		virtual void initPartActions( KActionCollection *col );
//		virtual void initInstanceActions( int mode, KActionCollection *col );

		virtual void initPartActions();
		virtual void initInstanceActions();

		virtual KexiDB::SchemaData* loadSchemaData(KexiDialogBase *dlg, const KexiDB::SchemaData& sdata);
};

class KexiQueryDataSource : public KexiPart::DataSource
{
	public:
		KexiQueryDataSource(KexiPart::Part *part);
		~KexiQueryDataSource();

		virtual KexiDB::FieldList *fields(KexiProject *prj, const KexiPart::Item &);
		virtual KexiDB::Cursor *cursor(KexiProject *prj, const KexiPart::Item &i, bool buffer);
};


#endif

