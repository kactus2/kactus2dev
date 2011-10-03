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

#include <GCF/RemoteAppDiscovery.h>
#include <GCF/AppAccessPoint.h>
#include <GCF/RemoteAppAccessPoint.h>

#include <QUdpSocket>
#include <QProcess>
#include <QTimerEvent>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QByteArray>
#include <QTimer>

/**
\class GCF::RemoteAppDiscovery GCF::RemoteAppDiscovery.h
\ingroup gcfipc
\brief Discovers remote applications on the LAN

If you want to automatically connect to remote applications (of the application name
as that of the host) as and when they are launched in the LAN, this is the class to use.
Just connect to discoveredRemoteAppAccessPoint() signal of this class to receive pointers
to valid \c GCF::RemoteAppAccessPoint objects as they are discovered and created.

\code
connect(&GCF::RemoteAppDiscovery::instance(), SIGNAL(discoveredRemoteApp(GCF::RemoteAppAccessPoint*)),
        receiver, SLOT(newRAP(GCF::RemoteAppAccessPoint*)));
GCF::RemoteAppDiscovery::instance().startDiscovery();

// From now on the newRAP() method of the receiver will be called when ever
// GCF::RemoteAppDiscovery discovers a new remote application. Note that the
// initial greeting message has already been sent and the GCF::RemoteAppAccessPoint
// will be ready to use by the time newRAP is called.
\endcode
*/

// BTW, anyone who has looked at Qt/examples/network/chat will realize that most of the
// code in this class is based on that example. I wont deny it :-)

namespace GCF
{
	struct RAPInfo
	{
		RAPInfo() : Port(-1), RAP(0), RAPObject(0) { }

		QHostAddress Address;
		quint16 Port;
		GCF::RemoteAppAccessPoint* RAP;
		QObject* RAPObject;
	};

	struct RemoteAppDiscoveryData
	{
		RemoteAppDiscoveryData() : broadcastTimer(0) { }

		QUdpSocket broadcastSocket;
		QTimer* broadcastTimer;
		QString username;

		QList<QHostAddress> broadcastAddresses;
		QList<QHostAddress> ipAddresses;
		void updateAddresses();
		bool isLocalHostAddress(const QHostAddress &address);

		QList<GCF::RAPInfo> knownRAPList;
	};

}

static const int broadcastTimerInterval = 2000;
static const unsigned broadcastPort = 45000;


/**
Returns a reference to the only instance of the this class.
*/
GCF::RemoteAppDiscovery& GCF::RemoteAppDiscovery::instance()
{
    static GCF::RemoteAppDiscovery theInstance;
    return theInstance;
}

/**
\internal
*/
GCF::RemoteAppDiscovery::RemoteAppDiscovery()
{
    d = new GCF::RemoteAppDiscoveryData;

    // Look for a user name from the environment variables
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();
    foreach (QString string, envVariables)
    {
        int index = environment.indexOf(QRegExp(string));
        if (index != -1)
        {
            QStringList stringList = environment.at(index).split("=");
            if (stringList.size() == 2)
            {
                d->username = stringList.at(1).toUtf8();
                break;
            }
        }
    }
    if(d->username.isEmpty())
        d->username = "unknown";

    // Update known broadcast and ip addresses
    d->updateAddresses();
    connect(&(d->broadcastSocket), SIGNAL(readyRead()), this, SLOT(readBroadcastDatagram()));
}

/**
Destructor
*/
GCF::RemoteAppDiscovery::~RemoteAppDiscovery()
{
    stopDiscovery(true);
    delete d;
}

/**
Starts discovery operation. By-default discovery is off, unless you specifically
start it using this method.
*/
void GCF::RemoteAppDiscovery::startDiscovery()
{
    if( d->broadcastTimer )
        return;

    // Bind the UDP socket to an address
    if( d->broadcastSocket.state() != QAbstractSocket::BoundState )
        d->broadcastSocket.bind(QHostAddress::Any, broadcastPort, QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);

    if( d->broadcastSocket.state() != QAbstractSocket::BoundState )
        qDebug("RemoteAppDiscovery: Cannot bind to %d", broadcastPort);
    else
    {
        // start the broadcast timer
        if( !d->broadcastTimer )
        {
            d->broadcastTimer = new QTimer(this);
            connect(d->broadcastTimer, SIGNAL(timeout()), this, SLOT(sendBroadcastDatagram()));
            d->broadcastTimer->setInterval(broadcastTimerInterval);
        }
        d->broadcastTimer->start();
        qDebug("RemoteAppDiscovery: Bound to %d", broadcastPort);
    }
}

/**
Stops discovering for new applications.
*/
void GCF::RemoteAppDiscovery::stopDiscovery(bool close)
{
    if( !d->broadcastTimer || !d->broadcastTimer->isActive() )
        return;

    d->broadcastTimer->stop();

    if(close)
        d->broadcastSocket.close();
}

/**
\internal
*/
void GCF::RemoteAppDiscovery::sendBroadcastDatagram()
{
    if( !GCF::AppAccessPoint::instance().isAppAccessEnabled() )
        return;

    QByteArray datagram = "GCFRAD@";
    datagram.append(d->username.toAscii());
    datagram.append('@');
    datagram.append(QByteArray::number(GCF::AppAccessPoint::instance().serverPort()));

    bool validBroadcastAddresses = true;
    foreach(QHostAddress address, d->broadcastAddresses)
    {
        if (d->broadcastSocket.writeDatagram(datagram, address, broadcastPort) == -1)
            validBroadcastAddresses = false;
    }

    if(!validBroadcastAddresses)
        d->updateAddresses();
}

/**
\internal
*/
void GCF::RemoteAppDiscovery::readBroadcastDatagram()
{
    while(d->broadcastSocket.hasPendingDatagrams())
    {
        QByteArray datagram;
        GCF::RAPInfo rapInfo;

        datagram.resize(d->broadcastSocket.pendingDatagramSize());
        if (d->broadcastSocket.readDatagram(datagram.data(), datagram.size(), &rapInfo.Address, &rapInfo.Port) == -1)
            continue;

        QList<QByteArray> list = datagram.split('@');
        if (list.size() != 3)
            continue;

        rapInfo.Port = list.at(2).toInt();
        if (d->isLocalHostAddress(rapInfo.Address) && rapInfo.Port == GCF::AppAccessPoint::instance().serverPort())
            continue;

        bool alreadyDiscovered = false;
        for(int i=0; i<d->knownRAPList.count(); i++)
        {
            if( d->knownRAPList[i].Address == rapInfo.Address && d->knownRAPList[i].Port == rapInfo.Port )
            {
                alreadyDiscovered = true;
                break;
            }
        }
        if( alreadyDiscovered )
            continue;

        emit discoveredRemoteApp(rapInfo.Address, rapInfo.Port);

        rapInfo.RAP = GCF::RemoteAppAccessPoint::getRemoteAppAccessPoint(rapInfo.Address, rapInfo.Port);
        rapInfo.RAPObject = rapInfo.RAP;
        connect(rapInfo.RAP, SIGNAL(connectedToApp()), this, SLOT(rapConnected()));
        connect(rapInfo.RAP, SIGNAL(destroyed(QObject*)), this, SLOT(rapDestroyed(QObject*)));
        connect(rapInfo.RAP, SIGNAL(couldNotConnectToApp()), this, SLOT(rapCouldNotConnect()));

        d->knownRAPList.append(rapInfo);
    }
}

/**
\internal
*/
void GCF::RemoteAppDiscovery::rapConnected()
{
    GCF::RemoteAppAccessPoint* rap = qobject_cast<GCF::RemoteAppAccessPoint*>( sender() );
    if( rap )
        emit discoveredRemoteApp(rap);
}

/**
\internal
*/
void GCF::RemoteAppDiscovery::rapCouldNotConnect()
{
    GCF::RemoteAppAccessPoint* rap = qobject_cast<GCF::RemoteAppAccessPoint*>( sender() );
    if( !rap )
        return;

    rap->deleteLater();
}

/**
\internal
*/
void GCF::RemoteAppDiscovery::rapDestroyed(QObject* object)
{
    for(int i=0; i<d->knownRAPList.count(); i++)
    {
        if( d->knownRAPList[i].RAPObject == object )
        {
            d->knownRAPList.removeAt(i);
            break;
        }
    }
}

/**
\internal
*/
void GCF::RemoteAppDiscoveryData::updateAddresses()
{
    broadcastAddresses.clear();
    ipAddresses.clear();

    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        foreach (QNetworkAddressEntry entry, interface.addressEntries())
        {
            QHostAddress broadcastAddress = entry.broadcast();
            if (broadcastAddress != QHostAddress::Null)
            {
                broadcastAddresses << broadcastAddress;
                ipAddresses << entry.ip();
            }
        }
    }
}

bool GCF::RemoteAppDiscoveryData::isLocalHostAddress(const QHostAddress &address)
{
    foreach (QHostAddress localAddress, ipAddresses)
    {
        if (address == localAddress)
            return true;
    }

    return false;
}
