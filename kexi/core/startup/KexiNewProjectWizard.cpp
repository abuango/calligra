/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include "KexiNewProjectWizard.h"

#include "KexiConnSelector.h"
#include "KexiNewPrjTypeSelector.h"
#include "KexiOpenExistingFile.h"
#include "KexiDBTitlePage.h"
#include "KexiServerDBNamePage.h"
#include "KexiProjectSelector.h"
#include "kexi.h"

#include <kapplication.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>
#include <klistview.h>
#include <kurlcombobox.h>
#include <kiconview.h>
#include <kmessagebox.h>

#include <qobjectlist.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qcheckbox.h>

class KexiNewProjectWizardPrivate
{
	public:
	KexiNewProjectWizardPrivate()
	{
		le_dbname_txtchanged_disable = false;
		le_dbname_autofill = true;
		conndata_to_show = 0;
		project_set_to_show = 0;
	}
	~KexiNewProjectWizardPrivate()
	{
		delete conndata_to_show;
		delete project_set_to_show;
	}
	KIconView *lv_types;
	KIconViewItem *lvi_file, *lvi_server;
	QString chk_file_txt, chk_server_txt; //!< helper
	
	QString server_db_name_dblist_lbl_txt; //!< helper

	//for displaying db list of the selected conn.
	KexiDB::ConnectionData *conndata_to_show;
	KexiProjectSet *project_set_to_show;
	
	bool le_dbname_txtchanged_disable : 1;
	bool le_dbname_autofill : 1;
};

KexiNewProjectWizard::KexiNewProjectWizard(const KexiDBConnectionSet& conn_set,
	QWidget *parent, const char *name, bool modal, WFlags f)
: KWizard(parent, name, modal, f)
, d(new KexiNewProjectWizardPrivate() )
{
	setIcon( DesktopIcon("filenew") );
	setCaption( i18n("Creating a new project") );
	finishButton()->setText(i18n("Create"));
	
	//page: type selector
	m_prjtype_sel = new KexiNewPrjTypeSelector(this, "KexiNewPrjTypeSelector");
	d->lv_types = new KIconView(m_prjtype_sel, "types");
	QString none;
	d->lvi_file = new KIconViewItem( d->lv_types, i18n("New project stored in a file"), 
		KGlobal::iconLoader()->loadIcon( KMimeType::mimeType("application/x-kexiproject-sqlite")->icon(none,0), KIcon::Desktop ) );
	d->lvi_server = new KIconViewItem( d->lv_types, i18n("New project stored on a database server"), DesktopIcon("socket") );
	d->lv_types->setFocus();
	QString txt_dns = i18n("Don't show me this question again.");
	d->chk_file_txt = m_prjtype_sel->chk_always->text() +"\n"+txt_dns;
	d->chk_server_txt = i18n("Always &use database server for creating new projects.")
		+"\n"+txt_dns;
	
	connect(d->lv_types,SIGNAL(executed(QIconViewItem*)),this,SLOT(slotLvTypesExecuted(QIconViewItem*)));
	connect(d->lv_types,SIGNAL(returnPressed(QIconViewItem*)),this,SLOT(slotLvTypesExecuted(QIconViewItem*)));
	connect(d->lv_types,SIGNAL(selectionChanged( QIconViewItem*)),this,SLOT(slotLvTypesSelected(QIconViewItem*)));

	static_cast<QVBoxLayout*>(m_prjtype_sel->layout())->insertWidget(1,d->lv_types);
	static_cast<QVBoxLayout*>(m_prjtype_sel->layout())->insertStretch(3,1);

	addPage(m_prjtype_sel, i18n("Select a storage method"));
	d->lv_types->setMinimumHeight(QMAX(d->lvi_file->height(),d->lvi_server->height())+25);

	//page: db title
	m_db_title = new KexiDBTitlePage(this, "KexiDBTitlePage");
	addPage(m_db_title, i18n("Select project's caption"));
		
	//page: connection selector
	m_conn_sel = new KexiConnSelectorWidget(conn_set, this, "KexiConnSelectorWidget");
	
	//"Select database server connection"
	m_conn_sel->m_file->btn_advanced->hide();
	m_conn_sel->m_file->label->hide();
	m_conn_sel->m_file->lbl->setText( i18n("Enter a new Kexi project's file name:") );
	
	m_conn_sel->m_remote->label->setText(
	 i18n("Select database server's connection you wish use to create a new Kexi project. "
	 "<p>Here you may also add, edit or remove connections from the list."));
	m_conn_sel->m_remote->label_back->hide();
	m_conn_sel->m_remote->btn_back->hide();

	m_conn_sel->showSimpleConn();
	//anyway, db files will be _saved_
	m_conn_sel->m_fileDlg->setMode( KexiStartupFileDialog::SavingFileBasedDB );
//	m_conn_sel->m_fileDlg->setMode( KFile::LocalOnly | KFile::File );
//	m_conn_sel->m_fileDlg->setOperationMode( KFileDialog::Saving );
////js	connect(m_conn_sel->m_fileDlg,SIGNAL(cancelClicked()),this,SLOT(reject()));
//	connect(m_conn_sel->m_fileDlg,SIGNAL(fileHighlighted(const QString&)),this,SLOT(slotFileHighlighted(const QString&)));
	connect(m_conn_sel->m_fileDlg,SIGNAL(accepted()),this,SLOT(accept()));
	m_conn_sel->showAdvancedConn();
	connect(m_conn_sel,SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
		this,SLOT(next()));
	
	addPage(m_conn_sel, i18n("Select project's location"));

	//page: server db name
	m_server_db_name = new KexiServerDBNamePage(this, "KexiServerDBNamePage");
	d->server_db_name_dblist_lbl_txt = i18n("Existing project databases on server <b>%1</b>:");
	connect(m_server_db_name->le_caption, SIGNAL(textChanged(const QString&)),
		this,SLOT(slotServerDBCaptionTxtChanged(const QString&)));
	connect(m_server_db_name->le_dbname, SIGNAL(textChanged(const QString&)),
		this,SLOT(slotServerDBNameTxtChanged(const QString&)));
	connect(m_server_db_name->le_caption, SIGNAL(returnPressed()),
		this,SLOT(accept()));
	connect(m_server_db_name->le_dbname, SIGNAL(returnPressed()),
		this,SLOT(accept()));
	m_server_db_name->le_caption->setText(i18n("New database"));
	m_server_db_name->le_dbname->setValidator(new Kexi::IdentifierValidator(this, "id_val"));
	m_project_selector = new KexiProjectSelectorWidget( 
		m_server_db_name->frm_dblist, "KexiProjectSelectorWidget", 0, false, false );
	GLUE_WIDGET(m_project_selector, m_server_db_name->frm_dblist);
	m_project_selector->setFocusPolicy(NoFocus);
	m_project_selector->setSelectable(false);
	
	addPage(m_server_db_name, i18n("Select project's caption and database name"));
	setFinishEnabled(m_server_db_name,true);
		
	//finish:
	updateGeometry();
	d->lv_types->setMaximumWidth(width()/2);
	d->lv_types->setSelected(d->lvi_file, true);
	
	//get settings
	KGlobal::config()->setGroup("Startup");
	//"" means goto 1st page
	QString default_storage = KGlobal::config()->readEntry("DefaultStorageForNewProjects","");
	if (default_storage.lower()=="file") {
		m_prjtype_sel->chk_always->setChecked(true);
		showPage(m_db_title);
	} else if (default_storage.lower()=="server") {
		d->lv_types->setSelected(d->lvi_server, true);
		m_prjtype_sel->chk_always->setChecked(true);
		showPage(m_conn_sel);
	}

}

KexiNewProjectWizard::~KexiNewProjectWizard()
{
	delete d;
}
	
void KexiNewProjectWizard::show()
{
	KDialog::centerOnScreen(this);
	KWizard::show();
}

void KexiNewProjectWizard::slotLvTypesExecuted(QIconViewItem *)
{
	next();//showPage(m_conn_sel);
}

void KexiNewProjectWizard::slotLvTypesSelected(QIconViewItem *item)
{
	if (item==d->lvi_file) {
		m_prjtype_sel->chk_always->setText(d->chk_file_txt);
		setAppropriate( m_db_title, true );
		setAppropriate( m_server_db_name, false );
	}
	else {
		m_prjtype_sel->chk_always->setText(d->chk_server_txt);
		setAppropriate( m_db_title, false );
		setAppropriate( m_server_db_name, true );
	}
}

void KexiNewProjectWizard::showPage(QWidget *page)
{
	if (page==m_prjtype_sel) {//p 1
		d->lv_types->setFocus();
		d->lv_types->setCurrentItem(d->lv_types->currentItem());
	} else if (page==m_db_title) {//p 2
		if (m_db_title->le_caption->text().stripWhiteSpace().isEmpty())
			m_db_title->le_caption->setText(i18n("New database"));
		m_db_title->le_caption->selectAll();
		m_db_title->le_caption->setFocus();
	} else if (page==m_conn_sel) {//p 3
		if (d->lv_types->currentItem()==d->lvi_file) {
			m_conn_sel->showSimpleConn();
			QString fn = Kexi::string2FileName( m_db_title->le_caption->text() );
			if (!fn.endsWith(".kexi"))
				fn += ".kexi";
			m_conn_sel->m_fileDlg->locationWidget()->setCurrentText(fn);
			setFinishEnabled(m_conn_sel,true);
			m_conn_sel->setFocus();
		}
		else {
			m_conn_sel->showAdvancedConn();
			setFinishEnabled(m_conn_sel,false);
			m_conn_sel->setFocus();
			m_server_db_name->le_caption->selectAll();
		}
	} else if (page==m_server_db_name) {
		if (m_conn_sel->selectedConnectionData() 
		 && (d->conndata_to_show != m_conn_sel->selectedConnectionData())) {
			d->conndata_to_show = m_conn_sel->selectedConnectionData();
			m_project_selector->setProjectSet(0);
			delete d->project_set_to_show;
			d->project_set_to_show = new KexiProjectSet(*d->conndata_to_show);
			//-refresh projects list
			m_project_selector->setProjectSet( d->project_set_to_show );
		}
	}
	KWizard::showPage(page);
}

void KexiNewProjectWizard::next()
{
	//let's check if move to next page is allowed:
	if (currentPage()==m_db_title) { //pg 2
		if (m_db_title->le_caption->text().stripWhiteSpace().isEmpty()) {
			KMessageBox::information(this, i18n("Enter project caption"));
			m_db_title->le_caption->setText("");
			m_db_title->le_caption->setFocus();
			return;
		}
	} else if (currentPage()==m_conn_sel) {//p 3
		if (d->lv_types->currentItem()==d->lvi_file) {
			//test for db file selection
		}
		else {
			//test for db conn selection
			if (!m_conn_sel->selectedConnectionData()) {
				KMessageBox::information(this, i18n("Select server connection for a new project"));
				return;
			}
			m_project_selector->label->setText(
				d->server_db_name_dblist_lbl_txt.arg(m_conn_sel->selectedConnectionData()->serverInfoString(false)) );
			m_server_db_name->le_caption->setFocus();
			
		}
	}
	KWizard::next();
}

void KexiNewProjectWizard::accept()
{
	if (d->lv_types->currentItem()==d->lvi_file) {//FILE:
		//check if new db file name is ok 
		kdDebug() << "********** sender() " << sender()->className() << endl;
		if (sender()==finishButton()) { /*(only if signal does not come from filedialog)*/
			kdDebug() << "********** sender()==finishButton() ********" << endl;
			if (!m_conn_sel->m_fileDlg->checkURL()) {
				return;
			}
		}
	} else {//SERVER:
		//check if we have enough of data
		if (m_server_db_name->le_caption->text().stripWhiteSpace().isEmpty()) {
			KMessageBox::information(this, i18n("Enter project caption"));
			m_server_db_name->le_caption->setText("");
			m_server_db_name->le_caption->setFocus();
			return;
		}
		QString dbname = m_server_db_name->le_dbname->text().stripWhiteSpace();
		if (dbname.isEmpty()) {
			KMessageBox::information(this, i18n("Enter project's database name"));
			m_server_db_name->le_dbname->setText("");
			m_server_db_name->le_dbname->setFocus();
			return;
		}
		//check for duplicated dbname
		if (m_project_selector->projectSet() && m_project_selector->projectSet()
			->findProject( m_server_db_name->le_dbname->text() )) {
			if (KMessageBox::warningYesNo( this, "<qt>"
				+i18n("<b>A project with database name \"%1\" already exists</b>"
				"<p>Do you want to delete it and create new one?")
				.arg( m_server_db_name->le_dbname->text() ) ) != KMessageBox::Yes)
			{
				m_server_db_name->le_dbname->setFocus();
				return;
			}
		}
	}
	
	KWizard::accept();
}

void KexiNewProjectWizard::done(int r)
{
	//save state (always, no matter if dialog is accepted or not)
	KGlobal::config()->setGroup("Startup");
	if (!m_prjtype_sel->chk_always->isChecked())
		KGlobal::config()->deleteEntry("DefaultStorageForNewProjects");
	else if (d->lv_types->currentItem()==d->lvi_file)
		KGlobal::config()->writeEntry("DefaultStorageForNewProjects","File");
	else
		KGlobal::config()->writeEntry("DefaultStorageForNewProjects","Server");
	
	KGlobal::config()->sync();
	
	KWizard::done(r);
}

QString KexiNewProjectWizard::projectDBName() const
{
	if (d->lv_types->currentItem()==d->lvi_server) {
		return m_server_db_name->le_dbname->text();
	}
	return m_conn_sel->m_fileDlg->currentURL().path();
}

QString KexiNewProjectWizard::projectCaption() const
{
	if (d->lv_types->currentItem()==d->lvi_server) {
		return m_server_db_name->le_caption->text();
	}
	return m_db_title->le_caption->text();
}

KexiDB::ConnectionData* KexiNewProjectWizard::projectConnectionData() const
{
	if (d->lv_types->currentItem()==d->lvi_file)
		return 0;
	return m_conn_sel->selectedConnectionData();
}

void KexiNewProjectWizard::slotServerDBCaptionTxtChanged(const QString &capt)
{	
	if (m_server_db_name->le_dbname->text().isEmpty())
		d->le_dbname_autofill=true;
	if (d->le_dbname_autofill) {
		d->le_dbname_txtchanged_disable = true;
		m_server_db_name->le_dbname->setText( Kexi::string2Identifier( capt ) );
		d->le_dbname_txtchanged_disable = false;
	}
}
	
void KexiNewProjectWizard::slotServerDBNameTxtChanged(const QString &)
{
	if (d->le_dbname_txtchanged_disable)
		return;
	d->le_dbname_autofill = false;
}

#include "KexiNewProjectWizard.moc"

