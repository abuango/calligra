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

#ifndef BRIDGESERVER_H
#define BRIDGESERVER_H

#include <QObject>

class QLocalServer;
class QLocalSocket;
class QDir;
class BridgeRequestHandler;
class KoTextEditor;

class BridgeServer : public QObject
{
    Q_OBJECT
public:
    explicit BridgeServer(KoTextEditor *editor, QObject *parent = 0);
    ~BridgeServer();
    void initServer();

    static const QString pipeIn;
    static const QString pipeOut;
    
signals:
    
public slots:
    void handleNewEngine();
private:
    QLocalServer *m_server;
    QList<BridgeRequestHandler*> *m_handles;
    KoTextEditor *m_editor;
};

#endif // BRIDGESERVER_H
