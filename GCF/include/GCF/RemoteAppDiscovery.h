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

#ifndef REMOTE_APP_DISCOVERY_H
#define REMOTE_APP_DISCOVERY_H

#include "Common.h"
#include <QObject>
#include <QHostAddress>

namespace GCF
{
	class RemoteAppAccessPoint;
    struct RemoteAppDiscoveryData;
    class GCF_EXPORT_CLASS RemoteAppDiscovery : public QObject
    {
        Q_OBJECT

    public:
        static RemoteAppDiscovery& instance();
        ~RemoteAppDiscovery();

        void startDiscovery();
        void stopDiscovery(bool close=false);

    signals:
        void discoveredRemoteApp(const QHostAddress& addr, int port);
        void discoveredRemoteApp(GCF::RemoteAppAccessPoint* rap);

    private slots:
        void sendBroadcastDatagram();
        void readBroadcastDatagram();
        void rapConnected();
        void rapCouldNotConnect();
        void rapDestroyed(QObject* object);

    protected:
        RemoteAppDiscovery();

    private:
        RemoteAppDiscoveryData* d;
    };

}

#endif


