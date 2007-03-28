/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "InsertVariableAction.h"
#include "KoVariable.h"

#include <KoCanvasBase.h>
#include <kpagedialog.h>

#include <KLocale>
#include <QLayout>

InsertVariableAction::InsertVariableAction(KoCanvasBase *base, KoInlineObjectFactory *factory, const KoInlineObjectTemplate &templ)
    : InsertVariableActionBase(base, templ.name),
    m_factory(factory),
    m_templateId(templ.id),
    m_properties (templ.properties)
{
    Q_ASSERT(factory->type() == KoInlineObjectFactory::TextVariable);
}

KoVariable *InsertVariableAction::createVariable() {
    KoVariable *variable = static_cast<KoVariable*> (m_factory->createInlineObject(m_properties));
    QWidget *widget = variable->createOptionsWidget();
    if(widget) {
        if(widget->layout())
            widget->layout()->setMargin(0);
        KPageDialog *dialog = new KPageDialog(m_canvas->canvasWidget());
        dialog->setCaption(i18n("%1 Options", text()));
        dialog->addPage(widget, "");
        if(dialog->exec() != KPageDialog::Accepted) {
            delete variable;
            variable = 0;
        }
        delete dialog;
    }
    return variable;
}
