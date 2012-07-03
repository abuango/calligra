/* This file is part of the KDE project
 * Copyright (C) 2012 Smit Patel <smitpatel24@gmail.com>
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

#ifndef BRIDGEACTION_H
#define BRIDGEACTION_H

#include <QVariantMap>

class QDataStream;
class KoTextEditor;

class BridgeAction
{
public:
    enum Action {
        InsertCitation,
        InsertBibliography,
        InsertCiteRecord
    };

    BridgeAction(const QVariantMap &map, Action _name);
    virtual ~BridgeAction();
    virtual void performAction() = 0;
    virtual QByteArray data();
    QVariantMap m_data;
private:
    QDataStream m_out;
    QByteArray m_block;
    Action name;
};

class InsertCitationBridgeAction : public BridgeAction
{
public:
    InsertCitationBridgeAction(const QVariantMap &map, KoTextEditor *editor);
    virtual void performAction();
private:
    KoTextEditor *m_editor;
};

class InsertBibliographyBridgeAction : public BridgeAction
{
public:
    InsertBibliographyBridgeAction(const QVariantMap &map, KoTextEditor *editor);
    virtual void performAction();
private:
    KoTextEditor *m_editor;
};

class InsertCiteRecordBridgeAction : public BridgeAction
{
public:
    InsertCiteRecordBridgeAction(const QVariantMap &map);
    virtual void performAction();
};

#endif // BRIDGEACTION_H
