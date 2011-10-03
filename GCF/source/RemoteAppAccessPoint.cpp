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

#include <GCF/RemoteAppAccessPoint.h>
#include <GCF/Message.h>
#include <GCF/RemoteObject.h>
#include <GCF/AppAccessPointComponent.h>
#include "MutexHelper.h"
#include <GCF/AppAccessPoint.h>

#include <QApplication>
#include <QSslSocket>
#include <QHostAddress>
#include <QTimer>

// All instances of GCF::RemoteAppAccessPoint are managed by an
// instance of this class.

namespace GCF
{
	struct RemoteAppAccessPointManager : QList<GCF::RemoteAppAccessPoint*>
	{
		~RemoteAppAccessPointManager() {
			QList<GCF::RemoteAppAccessPoint*> copy = *this;
			this->clear();

			for(int i=0; i<copy.count(); i++)
				copy[i]->deleteLater();
		}
	};

	static RemoteAppAccessPointManager& RAPManager()
	{
		static RemoteAppAccessPointManager theManager;
		return theManager;
	}

}

/**
\class GCF::RemoteAppAccessPoint GCF::RemoteAppAccessPoint.h
\ingroup gcfipc
\brief Manages connection to a remote application

This class helps an appliction to connect to a remote application. All
communication with that remote application will be routed via this class.

You can have only once instance of this class for a remote application.
Use the \ref getRemoteAppAccessPoint() method to get a pointer to an
instance of this class for a remote application.

Use deleteLater() method to destroy instances of this class. The constructor
and destructor of this class is made protected to ensure property object
creation and destruction.
*/

struct GCF::RemoteAppAccessPointData
{
    RemoteAppAccessPointData() : socket(0), accessGranted(false), remoteAppPort(0) { }

    QTcpSocket* socket;
    bool accessGranted;
    QByteArray unprocessedData;
    QMutex sendMessageLock;
    GCF::DataStore customData;
    QMap<QString, GCF::RemoteObject*> remoteObjectMap;

    // Values of parameters passed to connectToApp()
    QHostAddress remoteAppAddress;
    qint16 remoteAppPort;

    QList<GCF::RemoteObject*> remoteObjects;
    QList<QObject*> remoteObjects2;
    GCF::RemoteObject* findObject(qint32 id) {
        for(int i=0; i<remoteObjects.count(); i++)
            if( remoteObjects[i]->id() == id )
                return remoteObjects[i];
        return 0;
    }

    // Manage sent messages
    QList<GCF::Message*> sentMessages;
    GCF::Message* findSentMessage(qint32 msgId) {
        for(int i=0; i<sentMessages.count(); i++)
            if( sentMessages[i]->id() == msgId )
                return sentMessages[i];
        return 0;
    }

    // Managing pending messages
    QMutex pendingMessageLock;
    QList<GCF::Message*> pendingMessages;
    void addPendingMessage(GCF::Message* msg) {
        pendingMessageLock.lock();
        if( !pendingMessages.contains(msg) )
            pendingMessages.append(msg);
        pendingMessageLock.unlock();
    }
};

/**
Returns a pointer to an instance of \ref GCF::RemoteAppAccessPoint based for a given host address and port.
The function ensures that only once instance of \ref GCF::RemoteAppAccessPoint is created for every
(address, port) pair.
*/
GCF::RemoteAppAccessPoint* GCF::RemoteAppAccessPoint::getRemoteAppAccessPoint(const QHostAddress& address, quint16 port, bool useSsl)
{
    for(int i=0; i<RAPManager().count(); i++)
    {
        GCF::RemoteAppAccessPoint* rap = RAPManager()[i];
        if( rap->d->remoteAppAddress == address && rap->d->remoteAppPort == port )
            return rap;
    }

    GCF::RemoteAppAccessPoint* rap = new GCF::RemoteAppAccessPoint;
    rap->connectToApp(address, port, useSsl);
    // RAPManager().append(rap); // done by GCF::RemoteAppAccessPoint constructor
    return rap;
}

/**
Returns a list of all application access points.
*/
QList<GCF::RemoteAppAccessPoint*> GCF::RemoteAppAccessPoint::getRemoteAppAccessPointList()
{
    return RAPManager();
}

/**
Closes down all remote access points. All instances of CRemoteObjects will be deleted.
All pending messages will be discarded.
*/
void GCF::RemoteAppAccessPoint::closeAllRemoteAccessPoints()
{
    QList<GCF::RemoteAppAccessPoint*> rapList = RAPManager();
    RAPManager().clear();

    for(int i=0; i<rapList.count(); i++)
    {
        rapList[i]->disconnect();
        rapList[i]->deleteLater();
    }
}

/**
\internal
*/
GCF::RemoteAppAccessPoint::RemoteAppAccessPoint(QObject* parent)
:QObject(parent)
{
    d = new GCF::RemoteAppAccessPointData;
    RAPManager().append(this);
}

/**
\internal
*/
GCF::RemoteAppAccessPoint::~RemoteAppAccessPoint()
{
    // Unlock mutexes to release deadlocks if any
    // d->sendMessageLock.unlock();
    // d->pendingMessageLock.unlock();
    qDeleteAll(d->pendingMessages);

    d->remoteObjects2.clear();
    for(int i=0; i<d->remoteObjects.count(); i++)
        d->remoteObjects[i]->setId(-1);
    qDeleteAll( d->remoteObjects );
    delete d;
    RAPManager().removeAll(this);
}

/**
Returns true if this class has a valid connection with a remote application's \ref GCF::AppAccessPoint.
*/
bool GCF::RemoteAppAccessPoint::isValid() const
{
    if( d->socket && d->socket->isValid() )
        return d->accessGranted;

    return false;
}


/**
Disconnects from the remote application. The first sends a \ref GCF::Message::ReleaseAccess message
to the remote application, it then disconnects from the server.

\note Instances of GCF::RemoteObject created from this access point will be deactivated.
*/
void GCF::RemoteAppAccessPoint::disconnect()
{
    if( !isValid() )
        return;

    // Unlock mutexes to release deadlocks if any
    // d->sendMessageLock.unlock();
    // d->pendingMessageLock.unlock();
    qDeleteAll(d->pendingMessages);

    for(int i=0; i<d->remoteObjects.count(); i++)
        d->remoteObjects[i]->setId(-1);

    if( d->socket )
    {
        QObject::disconnect(d->socket, 0, this, 0);
        d->socket->disconnectFromHost();
        d->socket->close();
        delete d->socket;
        d->socket = 0;

        emit disconnectedFromApp();
    }
}

/**
\internal
*/
void GCF::RemoteAppAccessPoint::connectToApp(const QHostAddress& address, quint16 port, bool useSsl)
{
    if( d->socket )
        return;

    d->remoteAppAddress = address;
    d->remoteAppPort = port;

    d->socket = new QTcpSocket(this);

    connect(d->socket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(d->socket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    connect(d->socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError()));
    connect(d->socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotStateChanged()));
    connect(d->socket, SIGNAL(aboutToClose()), this, SLOT(slotAboutToClose()));
    connect(d->socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));

    d->socket->connectToHost(address, port);

    QTimer::singleShot(20000, this, SLOT(checkConnectStatus()));

    Q_UNUSED(useSsl);
}

void GCF::RemoteAppAccessPoint::activateObject(GCF::RemoteObject* rObj)
{
    // Send a request object message only if the object has been validated.
    // Otherwise, queue the message for later despatch.
    if( isValid() && !rObj->isActive() )
    {
        // Create a message to send to the server to request this object.
        GCF::RequestObjectMessage* msg = new GCF::RequestObjectMessage;
        msg->setObjectName(rObj->objectName());
        msg->customData()["GCF::RemoteObject"] = qVariantFromValue<GCF::RemoteObject*>(rObj);

        // Send the message
        sendMessage(msg);
    }
}

/**
Returns a pointer to a remote object whose name is \c completeName. The returned object can be used
if and only if GCF::RemoteObject::isActive() returns true. The returned object emits a activated() signal
once it becomes active.

The function returns a valid remote object, even if the access point is invalid. The returned
objects emit a activated() signal when it becomes active. (And that would happen ofcourse only
after the access point has become active).

\note Returned remote objects are destroyed when this object is destroyed, unless they are
specifically destroyed before hand.

\note Even if you parent the object under-some other object. They will still be destroyed.
*/
GCF::RemoteObject* GCF::RemoteAppAccessPoint::getRemoteObject(const QString& completeName)
{
    GCF::RemoteObject* rObj = 0;

    // First check to see if an instance of the remote object already exists
    if(d->remoteObjectMap.contains(completeName))
        return d->remoteObjectMap[completeName];

    if( !rObj )
    {
        // Create an instance of GCF::RemoteObject for completeName.
        rObj = new GCF::RemoteObject(this);
        rObj->setObjectName(completeName);
        d->remoteObjects.append(rObj);
        d->remoteObjects2.append(rObj);
        connect(rObj, SIGNAL(destroyed(QObject*)), this, SLOT(remoteObjectDestroyed(QObject*)));
        d->remoteObjectMap[completeName] = rObj;
    }

    this->activateObject(rObj);

    return rObj;
}

/**
Returns the local socket information
*/
QString GCF::RemoteAppAccessPoint::localInfo() const
{
    return QString("%1:%2").arg(d->socket->localAddress().toString()).arg(d->socket->localPort());
}

/**
Returns the remote socket information
*/
QString GCF::RemoteAppAccessPoint::peerInfo() const
{
    return QString("%1:%2").arg(d->socket->peerAddress().toString()).arg(d->socket->peerPort());
}

/**
Custom data specific to this remote application access point can be stored in the object
returned by this function.
*/
GCF::DataStore GCF::RemoteAppAccessPoint::customData() const
{
    return d->customData;
}

/**
Returns the socket through which all the messages are
*/
QAbstractSocket* GCF::RemoteAppAccessPoint::socket() const
{
    return d->socket;
}

/**
All message sending is done via this function. After sending the message, the message
object will be deleted.

\note Do not delete the message after calling this function. The message will automatically
be deleted at an appropriate time.
*/
void GCF::RemoteAppAccessPoint::sendMessage(GCF::Message* message)
{
    if( !message )
        return;

    if( !d->socket )
    {
        delete message;
        return;
    }

    if( !isValid() && message->messageType() != GCF::Message::RequestAccess )
    {
        delete message;
        return;
    }

    if( message->messageDirection() != GCF::Message::ClientToServer &&
        message->messageDirection() != GCF::Message::Both )
    {
        delete message;
        return;
    }

    // Try to acquire the send message lock. If thats not possible then
    // queue the message for sending at a later stage and quit.
    if( !d->sendMessageLock.tryLock() )
    {
        d->addPendingMessage(message);
        // I wouldnt use this because, queued connections are scheduled really quick.
        // I would much rather use a QTimer for this
        // QMetaObject::invokeMethod(this, "sendPendingMessages", Qt::QueuedConnection);
        QTimer::singleShot(500, this, SLOT("sendPendingMessages()"));
        return;
    }

    // Prepare a GCF::MutexUnlocker to unlock when done
    GCF::MutexUnlocker unlocker(&(d->sendMessageLock));

    if( d->sentMessages.contains(message) )
        return;

    QByteArray ba = message->toByteArray();
    if( d->socket->write(ba) < 0 )
    {
        qDebug("Error while sending message: %s", qPrintable(message->toString()));
        delete message;
        return;
    }
    // d->socket->flush();

    d->sentMessages.append(message);

    // Log message if possible
    if( GCF::AppAccessPointComponent::instance().messageLog() )
    {
        QString log = QString("%1: Sent to %2 - ").arg(localInfo()).arg(peerInfo());
        log += message->toString();
        GCF::AppAccessPointComponent::instance().appendMessageLog( log );
    }
}

/**
\internal
*/
void GCF::RemoteAppAccessPoint::slotConnected()
{
    QMetaObject::invokeMethod(this, "sendRequestAccessMessage", Qt::QueuedConnection);
    qDebug("Connection %s <-> %s", qPrintable(localInfo()), qPrintable(peerInfo()));
}

/**
\internal
*/
void GCF::RemoteAppAccessPoint::slotDisconnected()
{
    // Delete the socket
    d->socket->deleteLater();
    d->socket = 0;
    d->accessGranted = false;

    // Delete all sent messages
    qDeleteAll( d->sentMessages );
    d->sentMessages.clear();

    // Inform the world
    emit disconnectedFromApp();
}

/**
\internal
*/
void GCF::RemoteAppAccessPoint::slotError()
{
    qDebug("Socket Error: %s", qPrintable(d->socket->errorString()));
}

/**
\internal
*/
void GCF::RemoteAppAccessPoint::slotStateChanged()
{

}

/**
\internal
*/
void GCF::RemoteAppAccessPoint::slotAboutToClose()
{

}

/**
\internal
*/
void GCF::RemoteAppAccessPoint::slotReadyRead()
{
    if( !d->socket->bytesAvailable() )
        return;

    // Lets first take unprocessed data from the last slotReadyRead()
    QByteArray ba;
    if( d->unprocessedData.size() )
    {
        ba = d->unprocessedData;
        d->unprocessedData.clear();
    }

    // Now add the new data to it.
    if( ba.size() )
        ba.append( d->socket->readAll() );
    else
        ba = d->socket->readAll();

    // Now unpack a message from the byte array
    while(1)
    {
        GCF::Message* msg = GCF::Message::fromByteArray(ba);
        if( !msg )
            break;

        GCF::Message* sent = d->findSentMessage(msg->id());

        // Log message if possible
        if( GCF::AppAccessPointComponent::instance().messageLog() )
        {
            QString log = QString("%1: Received from %2 - ").arg(localInfo()).arg(peerInfo());
            log += msg->toString();
            GCF::AppAccessPointComponent::instance().appendMessageLog( log );
        }

        // Handle the incoming message
        bool success = handleMessage(msg, sent);
        if( !success )
        {
            qDebug("Message ID %d of type %s was not handled", msg->id(),
                qPrintable(GCF::Message::messageTypeString(msg->messageType())));
        }

        // Delete both sent and received messages
        delete msg;
        d->sentMessages.removeAll(sent);
        delete sent;
    }

    // If there are any unprocessed bytes left, then lets keep it for the next batch
    if( ba.size() )
        d->unprocessedData = ba;

    // Lets just check the socket one more time if more bytes are available for us
    // to read. If yes, lets schedule a call to this function again
    if( d->socket->bytesAvailable() )
        QMetaObject::invokeMethod(this, "slotReadyRead", Qt::QueuedConnection);
}

void GCF::RemoteAppAccessPoint::remoteObjectDestroyed(QObject* obj)
{
    if( !d->remoteObjects2.contains(obj) )
        return;

    int index = d->remoteObjects2.indexOf(obj);
    GCF::RemoteObject* rObj = d->remoteObjects[index];

    d->remoteObjects2.removeAt(index);
    d->remoteObjects.removeAt(index);

    QString key = d->remoteObjectMap.key(rObj);
    d->remoteObjectMap.remove(key);
}

/**
\internal
*/
void GCF::RemoteAppAccessPoint::sendRequestAccessMessage()
{
    // Send a RequestAccess message to the server
    GCF::RequestAccessMessage* msg = new GCF::RequestAccessMessage;
    msg->setApplicationName( GCF::AppAccessPoint::instance().accessPointName() );
    sendMessage( msg );
}

/**
\internal
*/
void GCF::RemoteAppAccessPoint::activateObjects()
{
    for(int i=0; i<d->remoteObjects.count(); i++)
        this->activateObject( d->remoteObjects[i] );
}

/**
\internal
*/
void GCF::RemoteAppAccessPoint::sendPendingMessages()
{
    if( !d->pendingMessageLock.tryLock() )
    {
        // I wouldnt use this because, queued connections are scheduled really quick.
        // I would much rather use a QTimer for this
        // QMetaObject::invokeMethod(this, "sendPendingMessages", Qt::QueuedConnection);
        QTimer::singleShot(500, this, SLOT("sendPendingMessages()"));
        return;
    }
    QList<GCF::Message*> msgList = d->pendingMessages;
    d->pendingMessages.clear();
    d->pendingMessageLock.unlock();

    for(int i=0; i<msgList.count(); i++)
    {
        GCF::Message* msg = msgList[i];
        sendMessage(msg);
    }
}

/**
\internal
*/
void GCF::RemoteAppAccessPoint::checkConnectStatus()
{
    if( this->isValid() )
        return;

    emit couldNotConnectToApp();
    for(int i=0; i<d->remoteObjects.count(); i++)
        emit d->remoteObjects[i]->couldNotActivate();
}

/**
\internal
*/
void GCF::RemoteAppAccessPoint::closeSocket()
{
    if( d->socket )
        d->socket->close();
}

/**
\internal
*/
bool GCF::RemoteAppAccessPoint::event(QEvent* e)
{
    if( e->type() == QEvent::DeferredDelete )
        disconnect();

    return QObject::event(e);
}

/**
\internal

This function handles an incoming message.

\li received is a pointer to the incoming message.
\li sent is a pointer to the message that was sent, which caused the \c received
message to arrive here.

Basically

if( sent is a valid pointer) then
    received->id() == sent->id()
Also received->incoming() == true, and
     sent->incoming() == false
*/
bool GCF::RemoteAppAccessPoint::handleMessage(GCF::Message* received, GCF::Message* sent)
{
    switch(received->messageType())
    {
    case GCF::Message::RequestAccessRes: {
        GCF::RequestAccessResMessage* msg = (GCF::RequestAccessResMessage*)received;
        if( msg->isAccessGranted() )
        {
            d->accessGranted = true;
            emit connectedToApp();
            qDebug("Connected to %s", qPrintable(peerInfo()));

            QMetaObject::invokeMethod(this, "activateObjects", Qt::QueuedConnection);
        }
        else
            QMetaObject::invokeMethod(this, "closeSocket", Qt::QueuedConnection);
            // d->socket->close();
        } break;

    case GCF::Message::RequestObjectRes: {
        GCF::RequestObjectResMessage* msg = (GCF::RequestObjectResMessage*)received;

        // from the sent message, figure out the GCF::RemoteObject pointer.
        GCF::RemoteObject* rObj = sent->customData()["GCF::RemoteObject"].value<GCF::RemoteObject*>();

        // Its is quite possible that the remote object was deleted, before
        // access grant had come to us. So, it is important to check for that
        // condition.
        if( d->remoteObjects.contains(rObj) )
            rObj->setId( msg->objectId() );
        else
        {
            GCF::ReleaseObjectMessage* msg2 = new GCF::ReleaseObjectMessage;
            msg2->setObjectId( msg->objectId() );
            sendMessage(msg2);
        }
        } break;

    // For all object based messages, ask the respective object to deal with it.
    case GCF::Message::GetPropertyNamesRes:
    case GCF::Message::SetPropertyRes:
    case GCF::Message::GetPropertyRes:
    case GCF::Message::InvokeMethodRes:
    case GCF::Message::RequestConnectionRes:
    case GCF::Message::SignalDelivery:
    case GCF::Message::ReleaseObject:
    case GCF::Message::ReleaseObjectRes:
    case GCF::Message::ReleaseConnection:
    case GCF::Message::ReleaseConnectionRes: {
        GCF::RemoteObject* rObj = d->findObject( received->objectId() );
        if( !rObj )
            return false;

        return rObj->handleMessage(received, sent);
        } break;
    default:
        return false;
    }

    return true;
}

