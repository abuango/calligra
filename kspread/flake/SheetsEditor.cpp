/* This file is part of the KDE project
   Copyright 2007 Sebastian Sauer <mail@dipe.org>

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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "SheetsEditor.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>

#include "TableShape.h"
#include "Doc.h"
#include "Sheet.h"
#include "Map.h"

using namespace KSpread;

class SheetsEditor::Private
{
    public:
        TableShape* tableShape;
        QListWidget* list;
        QPushButton* addbtn;
        QPushButton* rembtn;
};

SheetsEditor::SheetsEditor(TableShape* tableShape, QWidget* parent)
    : QWidget(parent)
    , d(new Private)
{
    d->tableShape = tableShape;

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    d->list = new QListWidget(this);
    connect(d->list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    layout->addWidget(d->list);

    Map *map = d->tableShape->doc()->map();
    connect(map, SIGNAL(sig_addSheet(Sheet*)), this, SLOT(sheetAdded(Sheet*)));
    foreach(Sheet* sheet, map->sheetList())
        d->list->addItem( sheet->sheetName() );

    QVBoxLayout* btnlayout = new QVBoxLayout(this);
    layout->addLayout(btnlayout);
    d->addbtn = new QPushButton(/*KIcon("edit-add"),*/ i18n("Add"), this);
    connect(d->addbtn, SIGNAL(clicked()), this, SLOT(addClicked()));
    btnlayout->addWidget(d->addbtn);

    d->rembtn = new QPushButton(/*KIcon("edit-delete"),*/ i18n("Remove"), this);
    connect(d->rembtn, SIGNAL(clicked()), this, SLOT(removeClicked()));
    btnlayout->addWidget(d->rembtn);

    btnlayout->addStretch(1);
    selectionChanged();
}

SheetsEditor::~SheetsEditor()
{
    delete d;
}

void SheetsEditor::sheetAdded(Sheet* sheet)
{
    d->list->addItem(sheet->sheetName());
}

void SheetsEditor::selectionChanged()
{
    d->rembtn->setEnabled( d->list->currentItem() );
}

void SheetsEditor::addClicked()
{
    d->tableShape->doc()->map()->addNewSheet();
}

void SheetsEditor::removeClicked()
{
    QListWidgetItem* item = d->list->currentItem();
    if( ! item )
        return;
    Map *map = d->tableShape->doc()->map();
    Sheet* sheet = map->findSheet( item->text() );
    if( ! sheet )
        return;
    map->takeSheet(sheet);
    delete item;
}

#include "SheetsEditor.moc"
