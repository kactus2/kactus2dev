/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef REMOTE_APP_ACCESS_POINT_H
#define REMOTE_APP_ACCESS_POINT_H

#include "Common.h"
#include "DataStore.h"
#include <QObject>
#include <QTcpSocket>

class QHostAddress;
class QAbstractSocket;

namespace GCF
{

	class RemoteObject;
	class Message;

	struct RemoteAppAccessPointData;
    class GCF_EXPORT_CLASS RemoteAppAccessPoint : public QObject
    {
        Q_OBJECT

    public:
        static RemoteAppAccessPoint* getRemoteAppAccessPoint(const QHostAddress& address, quint16 port, bool useSsl=false);
        static QList<RemoteAppAccessPoint*> getRemoteAppAccessPointList();
        static void closeAllRemoteAccessPoints();

        bool isValid() const;
        void disconnect();
        RemoteObject* getRemoteObject(const QString& completeName);

        QString localInfo() const;
        QString peerInfo() const;
        DataStore customData() const;

        QAbstractSocket* socket() const;

        void sendMessage(Message* message);

    protected:
        RemoteAppAccessPoint(QObject* parent=0);
        ~RemoteAppAccessPoint();

        void connectToApp(const QHostAddress& address, quint16 port, bool useSsl=false);
        void activateObject(RemoteObject* obj);

    protected slots:
        void slotConnected();
        void slotDisconnected();
        void slotError();
        void slotStateChanged();
        void slotAboutToClose();
        void slotReadyRead();

        // custom slots.
        void remoteObjectDestroyed(QObject* obj);
        void sendRequestAccessMessage();
        void activateObjects();
        void sendPendingMessages();
        void checkConnectStatus();
        void closeSocket();

    signals:
        void connectedToApp();
        void disconnectedFromApp();
        void couldNotConnectToApp();

    protected:
        bool event(QEvent* e);
        virtual bool handleMessage(Message* received, Message* sent);

    private:
        friend class RemoteObject;
        RemoteAppAccessPointData* d;
    };

}

Q_DECLARE_METATYPE(GCF::RemoteAppAccessPoint*)


#endif
