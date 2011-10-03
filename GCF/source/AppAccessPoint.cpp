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

#include <GCF/AppAccessPoint.h>
#include <GCF/AbstractComponent.h>
#include <GCF/ComponentGui.h>
#include <GCF/ComponentFactory.h>
#include <GCF/Message.h>
#include <GCF/AppAccessPointComponent.h>
#include <GCF/IRemoteAccessControl.h>

#include <QCoreApplication>
#include <QTcpSocket>
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QGenericArgument>
#include <QTimer>
#include <QStack>

/**
\class GCF::AppAccessPoint GCF::AppAccessPoint.h
\ingroup gcfipc
\brief Access point for other applications to access components/objects
in this application

This class can be configured to enable access to components and objects
of the host application to outside applications. This class provides the
core functionality to implement inter-component communication across
application boundaries.
*/

struct GCF::AppAccessPointData
{
    AppAccessPointData() : enableFlag(false), serverPort(0) { }

    bool enableFlag;
    quint16 serverPort;
    QList<GCF::RemoteApp*> remoteAppList;
    QString accessPointName;
};

/**
Returns a reference to the only instance of AppAccessPoint object.
*/
GCF::AppAccessPoint & GCF::AppAccessPoint::instance()
{
    static AppAccessPoint theInstance(0);
    return theInstance;
}

/**
Constructor. This is never directly called by the user, it is called by
the \ref instance() method.
*/
GCF::AppAccessPoint::AppAccessPoint(QObject* parent)
:QTcpServer(parent)
{
    d = new GCF::AppAccessPointData;

    //setAppAccessEnabled(true);
    connect(this, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    d->accessPointName = qApp->applicationName();
}

/**
Destructor
*/
GCF::AppAccessPoint::~AppAccessPoint()
{
    qDeleteAll( d->remoteAppList );
    delete d;
}

/**
Sets the application access point name. By default this is equal to the string
returned by \ref QApplication::applicationName(). GCF applications that have
the same access point name can communicate between each other.
*/
void GCF::AppAccessPoint::setAccessPointName(const QString& name)
{
    d->accessPointName = name;
}

/**
Returns the application access point name.
*/
QString GCF::AppAccessPoint::accessPointName() const
{
    return d->accessPointName;
}

/**
Enables/disables the access point. If the access point is enabled other applications
can invoke methods on objects that have been exposed by different components.
*/
void GCF::AppAccessPoint::setAppAccessEnabled(bool val)
{
    if( d->enableFlag == val )
        return;

    if( d->enableFlag )
    {
        QList<GCF::RemoteApp*> apps = d->remoteAppList;
        d->remoteAppList.clear();
        qDeleteAll(apps);
    }

    d->enableFlag = val;
    if( d->enableFlag )
    {
        d->enableFlag &= listen(QHostAddress::Any, d->serverPort);
        if( !d->enableFlag )
            d->enableFlag &= listen();
    }
    else
        close();

    if( d->enableFlag )
    {
        // Print out a frendly message in the debug, so that I dont have to
        // go to the config dialog box to figure out the ip address and port
        qDebug("Application Access opened on %s:%d",
            qPrintable(GCF::AppAccessPoint::instance().serverAddress().toString()),
            GCF::AppAccessPoint::instance().serverPort());
    }
}

/**
Returns if the access point has been enabled or not.
*/
bool GCF::AppAccessPoint::isAppAccessEnabled() const
{
    return d->enableFlag;
}

/**
Sets the port on which this access point should listen for connections. The port
number is when this class calls listen() the next time.
*/
void GCF::AppAccessPoint::setPort(quint16 port)
{
    d->serverPort = port;
}

/**
Returns the port number this class will use the next time setAppAccessEnabled() is called.

By default the value returned by this function is 0, which means that the class
automatically picks up a port number.
*/
quint16 GCF::AppAccessPoint::port() const
{
    return d->serverPort;
}

/**
By default component objects exposed through the GUI XML file and marked with
remoteAccess flag are exposed for outside applications to access. Applications
can use this method to add more objects at runtime for remote access.

\note If the object is deleted, then it is automatically removed as an exposed
object. You need not call \ref removeExposedObject() before deleting the object.
*/
bool GCF::AppAccessPoint::addExposedObject(GCF::AbstractComponent* owner, QObject* object, const QString& objectName, GCF::AppAccessPoint::AllowedRemoteOperations allowedOps)
{
    if( !owner || !object )
        return false;

    if( !GCF::ComponentFactory::instance().components().contains(owner) )
        return false;

    object->setObjectName(objectName);

    // Fetch the component GUI of the owner..
    GCF::ComponentGui gui = owner->componentGui();

    // Now create a node of Other type and register it with the gui
    GCF::ComponentGuiNode guiNode;
    guiNode.setType(GCF::ComponentGuiNode::Other);
    guiNode.setName(objectName);
    guiNode["name"] = objectName;
    guiNode["QObject"] = qVariantFromValue<QObject*>(object);
    guiNode["autoDeleteNode"] = true;
    guiNode["exposedNode"] = true;

	// Hints for remote access validation
	guiNode["allowRemoteAccess"] = true;
	if(allowedOps&AllowRemoteMethodInvocation)
		guiNode["allowRemoteMethodInvocation"] = true;
	if(allowedOps&AllowRemoteGetProperty)
		guiNode["allowRemoteGetProperty"] = true;
	if(allowedOps&AllowRemoteSetProperty)
		guiNode["allowRemoteSetProperty"] = true;
	if(allowedOps&AllowRemoteSignalConnect)
		guiNode["allowRemoteSignalConnect"] = true;
	if(allowedOps&AllowRemoteSignalDelivery)
		guiNode["allowRemoteSignalDelivery"] = true;

    // Store the node as a property value in QObject
    object->setProperty("_node_", qVariantFromValue<GCF::ComponentGuiNode>(guiNode));
    connect(object, SIGNAL(destroyed(QObject*)), owner, SLOT(objectDestroyed(QObject*)));

    // Now add the node to the component gui
    gui.addNode(guiNode);

    return true;
}

/**
Removes an exposed object from the component. Note that the object is not deleted, however
it will not be remotely accessible anymore.
*/
bool GCF::AppAccessPoint::removeExposedObject(GCF::AbstractComponent* owner, QObject* object)
{
    if( !owner || !object )
        return false;

    if( !GCF::ComponentFactory::instance().components().contains(owner) )
        return false;

    // Fetch the component GUI of the owner..
    GCF::ComponentGui gui = owner->componentGui();

    // Find out the node which describes the object
    GCF::ComponentGuiNode guiNode;

    // Loop through GUI nodes of Other type and look for \c object.
    for(int i=0; i<gui.nodeCount(); i++)
    {
        GCF::ComponentGuiNode node = gui.node(i);
        if( node.type() != GCF::ComponentGuiNode::Other )
            continue;

        if( node.hasKey("exposedNode") && node["exposedNode"].toBool() )
        {
            QObject* nodeObjPtr = node.hasKey("QObject") ? node["QObject"].value<QObject*>() : 0;
            if( nodeObjPtr == object )
            {
                guiNode = node;
                break;
            }
        }
    }

    // Now unregister the node.
    gui.removeNode( guiNode );

    return true;
}

/**
Returns pointer to a QObject whose complete name is as specified in the parameter.

If the \c forRemoteAccess parameter is true, then only those objects that dont have
a disallowRemoteAccess="false" attribute associated with them in their component GUI XML
files will be searched. If unspecified, disallowRemoteAccess is assumed to be false.

The returned object's owner is returned via the \c owner parameter, if a non-null pointer
is passed.
*/
QObject* GCF::AppAccessPoint::findObject(const QString& completeName, GCF::AbstractComponent** owner, bool forRemoteAccess) const
{
    if(completeName.isEmpty())
        return 0;

    QString componentName = completeName.split('.').first();
    GCF::AbstractComponent* comp = GCF::ComponentFactory::instance().findComponent(componentName);
    if( !comp )
        return 0;

    if( forRemoteAccess )
    {
        GCF::ComponentGuiNode node = comp->componentGui().findNode(completeName);
        if( node.hasKey("disallowRemoteAccess") && node["disallowRemoteAccess"].toBool() )
            return 0;
    }

    QObject* retObj = comp->findObject(completeName);
    if( retObj && owner )
        *owner = comp;

    return retObj;
}

/**
Returns a list of sockets corresponding to remote applications
*/
QList<GCF::RemoteApp*> GCF::AppAccessPoint::connectedRemoteAppList() const
{
    return d->remoteAppList;
}

void GCF::AppAccessPoint::slotNewConnection()
{
    while( this->hasPendingConnections() )
    {
        QTcpSocket* socket = this->nextPendingConnection();
        new GCF::RemoteApp(socket, this);
    }
}

void GCF::AppAccessPoint::addRemoteApp(GCF::RemoteApp* app)
{
    d->remoteAppList.append(app);
}

void GCF::AppAccessPoint::removeRemoteApp(GCF::RemoteApp* app)
{
    d->remoteAppList.removeAll(app);
}

////////////////////////////////////////////////////////////////////////////////////
// GCF::RemoteApp
////////////////////////////////////////////////////////////////////////////////////

#include <GCF/MethodInvocationHelper.h>

#include <QColor>
#include <QPixmap>
#include <QTime>
#include <QDate>
#include <QDateTime>
#include <QString>
#include <QByteArray>
#include <QBitArray>
#include <QImage>
#include <QBitmap>
#include <QPointF>
#include <QPoint>
#include <QLineF>
#include <QLine>
#include <QSizeF>
#include <QSize>
#include <QRectF>
#include <QRect>
#include <QMatrix>
#include <QTransform>
#include <QUrl>
#include <QSizePolicy>
#include <QBrush>

#include "MutexHelper.h"

/**
\class GCF::RemoteApp GCF::RemoteApp.h
\ingroup gcfipc
\brief Handles messages sent from a remote application

This class receives messages sent from a remote application. Its responsibility is
to interpret those messages and then respond to them. The class should only be
constructed by AppAccessPoint, and not by anyone else.
*/

static qint32 GenObjectID()
{
    static qint32 id = 0;
    return ++id;
}

struct GCF::RemoteAppData
{
    QMap<qint32, QObject*> objectIdMap;
    QMap<QObject*, GCF::AbstractComponent*> objectComponentMap;
    QAbstractSocket* socket;
    QByteArray unprocessedData;
    QMutex sendMessageLock;

    // Managing pending messages
    QMutex pendingMessageLock;
    QList<GCF::Message*> pendingMessages;
    void addPendingMessage(GCF::Message* msg) {
        pendingMessageLock.lock();
        if( !pendingMessages.contains(msg) )
            pendingMessages.append(msg);
        pendingMessageLock.unlock();
    }

    // List of signal spies maintained for signal delivery.
    QList<GCF::SignalSpy1*> signalSpyList;
    GCF::SignalSpy1* findSignalSpy(QObject* sender, const char* signal) {
        for(int i=0; i<signalSpyList.count(); i++) {
            GCF::SignalSpy1* spy = signalSpyList[i];
            if( spy->sender() == sender && !qstrcmp(spy->signal2(),signal) )
                return spy;
        }
        return 0;
    }
};

static QStack<GCF::RemoteApp*> & CurrentRemoteAppStack()
{
    static QStack<GCF::RemoteApp*> theStack;
    return theStack;
}

/**
When a method is called from within a RMI context, then this function
will return a pointer to the \ref GCF::RemoteApp from which the method
was called. At all other times, this function returns 0.

\note It is extremely important that you do not delete the object returned
by this function.
*/
GCF::RemoteApp* GCF::RemoteApp::currentRemoteApp()
{
    if( CurrentRemoteAppStack().count() )
        return CurrentRemoteAppStack().top();

    return 0;
}

/**
\internal
*/
GCF::RemoteApp::RemoteApp(QAbstractSocket* socket, QObject* parent)
:QObject(parent)
{
    d = new GCF::RemoteAppData;
    d->socket = socket;
    d->socket->setParent(this);

    GCF::AppAccessPoint::instance().addRemoteApp(this);

    // Make connections
    connect(d->socket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    connect(d->socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError()));
    connect(d->socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotStateChanged()));
    connect(d->socket, SIGNAL(aboutToClose()), this, SLOT(slotAboutToClose()));
    connect(d->socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));

    // startTimer(2000);
}

/**
Sends a \ref GCF::Message::ReleaseAccess message to the peer socket and destroys self.
*/
GCF::RemoteApp::~RemoteApp()
{
    qDeleteAll(d->signalSpyList);
    d->signalSpyList.clear();

    // sendMessage( new CReleaseAccessMessage );
    if( d->socket )
        d->socket->close();

    // Unlock mutexes to release deadlocks if any
    // d->sendMessageLock.unlock();
    // d->pendingMessageLock.unlock();
    qDeleteAll(d->pendingMessages);

    GCF::AppAccessPoint::instance().removeRemoteApp(this);

    delete d;
}

/**
Returns the socket via which messages to the remote app are sent and
received.
*/
QAbstractSocket* GCF::RemoteApp::socket() const
{
    return d->socket;
}

/**
Returns the local socket information
*/
QString GCF::RemoteApp::localInfo() const
{
    return QString("%1:%2").arg(d->socket->localAddress().toString()).arg(d->socket->localPort());
}

/**
Returns the remote socket information
*/
QString GCF::RemoteApp::peerInfo() const
{
    return QString("%1:%2").arg(d->socket->peerAddress().toString()).arg(d->socket->peerPort());
}

/**
Sends a message to the peer. After sending, the message is deleted. Ensure that
the messages passed to this function are created on the heap.
*/
void GCF::RemoteApp::sendMessage(GCF::Message* message)
{
    if( !message )
        return;

    if( !d->socket || !d->socket->isValid() || !d->socket->isOpen() )
    {
        delete message;
        return;
    }

    if( message->messageDirection() != GCF::Message::ServerToClient &&
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

    // Prepare a MutexUnlocker to unlock when done
    MutexUnlocker unlocker(&(d->sendMessageLock));

    QByteArray ba = message->toByteArray();
    if( d->socket->write(ba) < 0 )
    {
        qDebug("Error while sending message: %s", qPrintable(message->toString()));
        delete message;
        return;
    }
    // d->socket->flush();

    // Log message if possible
    if( GCF::AppAccessPointComponent::instance().messageLog() )
    {
        QString log = QString("%1: Sent to %2 - ").arg(localInfo()).arg(peerInfo());
        log += message->toString();
        GCF::AppAccessPointComponent::instance().appendMessageLog( log );
    }

    delete message;
}

/**
\internal
*/
void GCF::RemoteApp::slotDisconnected()
{
    deleteLater();
}

/**
\internal
*/
void GCF::RemoteApp::slotError()
{
    qDebug("Socket Error: %s", qPrintable(d->socket->errorString()));
}

/**
\internal
*/
void GCF::RemoteApp::slotAboutToClose()
{
    // Do nothing
}

/**
\internal
*/
void GCF::RemoteApp::slotReadyRead()
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

        // Log message if possible
        if( GCF::AppAccessPointComponent::instance().messageLog() )
        {
            QString log = QString("%1: Received from %2 - ").arg(localInfo()).arg(peerInfo());
            log += msg->toString();
            GCF::AppAccessPointComponent::instance().appendMessageLog( log );
        }

        // Handle the incoming message
        bool success = handleMessage(msg);
        if( !success )
        {
            qDebug("Message ID %d of type %s was not handled", msg->id(),
                qPrintable(GCF::Message::messageTypeString(msg->messageType())));
        }

        // Delete the received message
        delete msg;
    }

    // If there are any unprocessed bytes left, then lets keep it for the next batch
    if( ba.size() )
        d->unprocessedData = ba;

    // Lets just check the socket one more time if more bytes are available for us
    // to read. If yes, lets schedule a call to this function again
    if( d->socket->bytesAvailable() )
        QMetaObject::invokeMethod(this, "slotReadyRead", Qt::QueuedConnection);
}

/**
\internal
*/
bool GCF::RemoteApp::handleMessage(GCF::Message* received)
{
    switch(received->messageType())
    {
    case GCF::Message::RequestAccess: {
        GCF::RequestAccessMessage* msg = (GCF::RequestAccessMessage*)received;
        GCF::RequestAccessResMessage* reply = new GCF::RequestAccessResMessage(received->id());
        if( msg->applicationName() == GCF::AppAccessPoint::instance().accessPointName() )
            reply->setAccessGranted( GCF::AppAccessPoint::instance().isAppAccessEnabled() );
        else
            reply->setAccessGranted( false );
        sendMessage( reply );

        } break;

    case GCF::Message::RequestObject: {

        GCF::RequestObjectMessage* msg = (GCF::RequestObjectMessage*)received;
        GCF::AbstractComponent* owner = 0;
        QObject* reqObjectPtr = GCF::AppAccessPoint::instance().findObject( msg->objectName(), &owner, true );
        qint32 id = -1;

        if( reqObjectPtr && isAllowObjectAccess(owner, reqObjectPtr, this) )
        {
            QList<QObject*> objects = d->objectIdMap.values();
            if( objects.contains(reqObjectPtr) )
                id = d->objectIdMap.key(reqObjectPtr);
            else
            {
                id = GenObjectID();
                d->objectIdMap[id] = reqObjectPtr;
                d->objectComponentMap[reqObjectPtr] = owner;

                // so we can send ReleaseObject signal when the object
                // gets destroyed
                connect(reqObjectPtr, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)));
            }
        }

        GCF::RequestObjectResMessage* reply = new GCF::RequestObjectResMessage(received->id());
        reply->setObjectId(id);
        sendMessage( reply );

        } break;

    case GCF::Message::InvokeMethod:
    case GCF::Message::SetProperty:
    case GCF::Message::GetProperty:
    case GCF::Message::GetPropertyNames:
    case GCF::Message::RequestConnection:
    case GCF::Message::ReleaseConnection:
    case GCF::Message::ReleaseObject: {

        CurrentRemoteAppStack().push(this);

        qint32 objectId = received->objectId();
        QObject* objectPtr = d->objectIdMap.contains(objectId) ? d->objectIdMap[objectId] : 0;
        GCF::AbstractComponent* objectOwner = d->objectComponentMap.contains(objectPtr) ? d->objectComponentMap[objectPtr] : 0;
        bool success = handleObjectMessage(objectId, objectOwner, objectPtr, received);

        CurrentRemoteAppStack().pop();

        return success;

        } break;

    default:
        return false;
    }

    return true;
}

bool GCF::RemoteApp::handleObjectMessage(qint32 objectId, GCF::AbstractComponent* objectOwner, QObject* objectPtr, GCF::Message* received)
{
    switch(received->messageType())
    {
    case GCF::Message::InvokeMethod: {

        GCF::InvokeMethodMessage* msg = (GCF::InvokeMethodMessage*)received;
        bool success = false;
        QVariant retVal;

        if( objectPtr )
        {
			//Todo
			/*int methodIdx = GCF::MethodInvocationHelper::findMethod(objectPtr, msg->methodName(), msg->argList());
            if( this->isAllowObjectMethodInvoke(objectOwner, objectPtr, methodIdx, this) )
                success = GCF::MethodInvocationHelper::callMethod(objectPtr, msg->methodName(), msg->argList(), retVal);*/
			int methodIdx = GCF::findMethod(objectPtr, msg->methodName(), msg->argList());
            if( this->isAllowObjectMethodInvoke(objectOwner, objectPtr, methodIdx, this) )
                success = GCF::callMethod(objectPtr, msg->methodName(), msg->argList(), retVal);
        }

        GCF::InvokeMethodResMessage* reply = new GCF::InvokeMethodResMessage(msg->id());
        reply->setObjectId(msg->objectId());
        reply->setMethodName(msg->methodName());
        reply->setRetValue(retVal);
        reply->setSuccess(success);
        sendMessage( reply );

        } break;

    case GCF::Message::SetProperty: {

        GCF::SetPropertyMessage* msg = (GCF::SetPropertyMessage*)received;
        bool success = false;
        QVariant retVal;

        if( objectPtr )
        {
            QByteArray propName = msg->propertyName().toAscii();
            int propIdx = objectPtr->metaObject()->indexOfProperty( propName.constData() );
            if( this->isAllowObjectSetProperty(objectOwner, objectPtr, propIdx, this) )
                success = objectPtr->setProperty( qPrintable(msg->propertyName()), msg->propertyValue() );

            if( success )
                retVal = msg->propertyValue();
            else
                retVal = objectPtr->property( qPrintable(msg->propertyName()) );
        }

        GCF::SetPropertyResMessage* reply = new GCF::SetPropertyResMessage(msg->id());
        reply->setObjectId(msg->objectId());
        reply->setPropertyName(msg->propertyName());
        reply->setPropertyValue(retVal);
        reply->setSuccess(success);
        sendMessage(reply);

        } break;

    case GCF::Message::GetProperty: {

        GCF::GetPropertyMessage* msg = (GCF::GetPropertyMessage*)received;
        bool success = false;
        QVariant retVal;

        if( objectPtr )
        {
            const QMetaObject* mo = objectPtr->metaObject();
            QByteArray propName = msg->propertyName().toAscii();
            int propIdx = objectPtr->metaObject()->indexOfProperty( propName.constData() );
            if( this->isAllowObjectSetProperty(objectOwner, objectPtr, propIdx, this) )
            {
                retVal = objectPtr->property( qPrintable(msg->propertyName()) );
                success = true;
            }
        }

        GCF::GetPropertyResMessage* reply = new GCF::GetPropertyResMessage(msg->id());
        reply->setObjectId(msg->objectId());
        reply->setPropertyName(msg->propertyName());
        reply->setPropertyValue(retVal);
        reply->setSuccess(success);
        sendMessage(reply);

        } break;

    case GCF::Message::GetPropertyNames: {

        GCF::GetPropertyMessage* msg = (GCF::GetPropertyMessage*)received;
        QStringList propNames;

        if( objectPtr && this->isAllowObjectAccess(objectOwner, objectPtr, this) )
        {
            const QMetaObject* mo = objectPtr->metaObject();
            for(int i=0; i<mo->propertyCount(); i++)
                propNames << QString( mo->property(i).name() );
        }

        GCF::GetPropertyNamesResMessage* reply = new GCF::GetPropertyNamesResMessage(msg->id());
        reply->setObjectId(msg->objectId());
        reply->setPropertyNames(propNames);
        sendMessage(reply);

        } break;

    case GCF::Message::RequestConnection:{

        GCF::RequestConnectionMessage* msg = (GCF::RequestConnectionMessage*)received;

        QByteArray signal = msg->signal().toAscii();
        QByteArray signalSignature = QMetaObject::normalizedSignature( signal.constData()+1 );
        int signalIdx = objectPtr->metaObject()->indexOfSignal( signalSignature.constData() );
        GCF::SignalSpy1* spy = 0;

        if( this->isAllowObjectSignalConnect(objectOwner, objectPtr, signalIdx, this) )
        {
            spy = d->findSignalSpy(objectPtr, signal.constData());
            if( !spy )
            {
                spy = new GCF::SignalSpy1(this, objectOwner, objectId, objectPtr, signal.constData());
                if( !spy->isValid() )
                {
                    delete spy;
                    spy = 0;
                }
                else
                    d->signalSpyList.append(spy);
            }
        }

        GCF::RequestConnectionResMessage* reply = new GCF::RequestConnectionResMessage(msg->id());
        reply->setObjectId(objectId);
        if( spy )
            reply->setSignal(spy->signal());
        else
            reply->setSignal(msg->signal());
        reply->setSuccess(spy != 0);
        sendMessage(reply);

        } break;

    case GCF::Message::ReleaseConnection: {

        GCF::ReleaseConnectionMessage* msg = (GCF::ReleaseConnectionMessage*)received;
        QByteArray signal = msg->signal().toAscii();
        GCF::SignalSpy1* spy = d->findSignalSpy(objectPtr, signal.constData());
        if(spy)
        {
            d->signalSpyList.removeAll(spy);
            delete spy;
        }

        GCF::ReleaseConnectionResMessage* reply = new GCF::ReleaseConnectionResMessage(msg->id());
        reply->setObjectId(objectId);
        reply->setSignal(msg->signal());
        reply->setSuccess(spy != 0);
        sendMessage(reply);

        } break;

    case GCF::Message::ReleaseObject: {

        GCF::ReleaseObjectMessage* msg = (GCF::ReleaseObjectMessage*)received;

        // Remove from the object ID map
        d->objectIdMap.remove(objectId);
        d->objectComponentMap.remove(objectPtr);

        // Remove all signal spies, spying on this object
        for(int i=0; i<d->signalSpyList.count(); i++)
        {
            GCF::SignalSpy1* spy = d->signalSpyList[i];
            if( spy->sender() == objectPtr )
            {
                d->signalSpyList.removeAt(i);
                --i;
                delete spy;
            }
        }

        // Send a confirmation
        GCF::ReleaseObjectResMessage* reply = new GCF::ReleaseObjectResMessage(msg->id());
        reply->setObjectId(objectId);
        reply->setSuccess(true);

        } break;

    default:
        return false;
    }

    return true;
}

void GCF::RemoteApp::objectDestroyed(QObject* obj)
{
    qint32 id = -1;

    // Find out if this slot was called by one of our objects
    QList<QObject*> objects = d->objectIdMap.values();
    if( objects.contains(obj) )
        id = d->objectIdMap.key(obj);

    // Discard the call if the object is not ours
    if(id < 0)
        return;

    // Unregister the object
    d->objectIdMap.remove(id);

    // Send a ReleaseObject message to the other end.
    GCF::ReleaseObjectMessage* msg = new GCF::ReleaseObjectMessage;
    msg->setObjectId(id);
    sendMessage(msg);
}

void GCF::RemoteApp::sendPendingMessages()
{
    if( !d->pendingMessageLock.tryLock() )
    {
        // schedule self again
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
Returns true/false on whether the \c object belonging to \c comp component can be accessed remotely by
the remote application handler \c remoteApp.

The function makes use of IComponentRemoteAccessControl interface in component (if-found) and
IObjectRemoteAccessControl interface in object (if-found) for making the decision. The former interface's
decision is given priority over the latters.
*/
bool GCF::RemoteApp::isAllowObjectAccess(GCF::AbstractComponent* comp, QObject* object, GCF::RemoteApp* remoteApp)
{
    if( !comp || !object || !remoteApp )
        return false;

	// Find out from code if remote access is allowed
    IComponentRemoteAccessControl* compRAC = qobject_cast<IComponentRemoteAccessControl*>(comp);
    if( compRAC )
    {
        if( !compRAC->isAllowComponentAccess(remoteApp) )
            return false;

        if( !compRAC->isAllowObjectAccess(object, remoteApp) )
            return false;
    }
	else 
	{
		// Check what the GUIXML has to say.
		GCF::ComponentGui gui = comp->componentGui();
		bool allowComponentRemoteAccess = gui.hasKey("allowRemoteAccess") && gui["allowRemoteAccess"].toBool();

		if(!allowComponentRemoteAccess)
			return false;
	}

    IObjectRemoteAccessControl* objectRAC = qobject_cast<IObjectRemoteAccessControl*>(object);
    if( objectRAC )
    {
        if( !objectRAC->isAllowObjectAccess(remoteApp) )
            return false;
    }
	else
	{
		// Check what the GUIXML has to say.
		GCF::ComponentGuiNode node = object->property("_node_").value<GCF::ComponentGuiNode>();
		bool allowObjectRemoteAccess = node.isValid() && node.hasKey("allowRemoteAccess") && node["allowRemoteAccess"].toBool();

		if(!allowObjectRemoteAccess)
			return false;
	}

    return true;
}

/**
Returns true/false on whether the method in \c object belonging to \c comp component can be accessed
remotely by the remote application handler \c remoteApp.

The function makes use of IComponentRemoteAccessControl interface in component (if-found) and
IObjectRemoteAccessControl interface in object (if-found) for making the decision. The former interface's
decision is given priority over the latters.
*/
bool GCF::RemoteApp::isAllowObjectMethodInvoke(GCF::AbstractComponent* comp, QObject* object, int methodIdx, GCF::RemoteApp* remoteApp)
{
    if( !object || !remoteApp || methodIdx < 0 || methodIdx >= object->metaObject()->methodCount())
        return false;

	if(!isAllowObjectAccess(comp, object, remoteApp))
		return false;

    IObjectRemoteAccessControl* objectRAC = qobject_cast<IObjectRemoteAccessControl*>(object);
    if( objectRAC )
    {
        if( !objectRAC->isAllowObjectMethodInvoke(methodIdx, remoteApp) )
            return false;
    }
	else
	{
		// Check what the GUIXML has to say.
		GCF::ComponentGuiNode node = object->property("_node_").value<GCF::ComponentGuiNode>();
		bool allowRMI = node.isValid() && node.hasKey("allowRemoteMethodInvocation") && node["allowRemoteMethodInvocation"].toBool();

		if(!allowRMI)
			return false;
	}

    return true;
}

/**
Returns true/false on whether the property in \c object belonging to \c comp component can be accessed
remotely by the remote application handler \c remoteApp.

The function makes use of IComponentRemoteAccessControl interface in component (if-found) and
IObjectRemoteAccessControl interface in object (if-found) for making the decision. The former interface's
decision is given priority over the latters.
*/
bool GCF::RemoteApp::isAllowObjectSetProperty(GCF::AbstractComponent* comp, QObject* object, int propIdx, GCF::RemoteApp* remoteApp)
{
    if( !object || !remoteApp || propIdx < 0 || propIdx >= object->metaObject()->propertyCount())
        return false;

	if(!isAllowObjectAccess(comp, object, remoteApp))
		return false;

    IObjectRemoteAccessControl* objectRAC = qobject_cast<IObjectRemoteAccessControl*>(object);
    if( objectRAC )
    {
        if( !objectRAC->isAllowObjectSetProperty(propIdx, remoteApp) )
            return false;
    }
	else
	{
		// Check what the GUIXML has to say.
		GCF::ComponentGuiNode node = object->property("_node_").value<GCF::ComponentGuiNode>();
		bool allowRSP = node.isValid() && node.hasKey("allowRemoteSetProperty") && node["allowRemoteSetProperty"].toBool();

		if(!allowRSP)
			return false;
	}

    return true;
}

/**
Returns true/false on whether the property in \c object belonging to \c comp component can be accessed
remotely by the remote application handler \c remoteApp.

The function makes use of IComponentRemoteAccessControl interface in component (if-found) and
IObjectRemoteAccessControl interface in object (if-found) for making the decision. The former interface's
decision is given priority over the latters.
*/
bool GCF::RemoteApp::isAllowObjectGetProperty(GCF::AbstractComponent* comp, QObject* object, int propIdx, GCF::RemoteApp* remoteApp)
{
    if( !object || !remoteApp || propIdx < 0 || propIdx >= object->metaObject()->propertyCount())
        return false;

	if(!isAllowObjectAccess(comp, object, remoteApp))
		return false;

    IObjectRemoteAccessControl* objectRAC = qobject_cast<IObjectRemoteAccessControl*>(object);
    if( objectRAC )
    {
        if( !objectRAC->isAllowObjectGetProperty(propIdx, remoteApp) )
            return false;
    }
	else
	{
		// Check what the GUIXML has to say.
		GCF::ComponentGuiNode node = object->property("_node_").value<GCF::ComponentGuiNode>();
		bool allowRGP = node.isValid() && node.hasKey("allowRemoteGetProperty") && node["allowRemoteGetProperty"].toBool();

		if(!allowRGP)
			return false;
	}

    return true;
}

/**
Returns true/false on whether the signal in \c object belonging to \c comp component can be accessed
remotely by the remote application handler \c remoteApp.

The function makes use of IComponentRemoteAccessControl interface in component (if-found) and
IObjectRemoteAccessControl interface in object (if-found) for making the decision. The former interface's
decision is given priority over the latters.
*/
bool GCF::RemoteApp::isAllowObjectSignalConnect(GCF::AbstractComponent* comp, QObject* object, int signalIdx, GCF::RemoteApp* remoteApp)
{
    if( !object || !remoteApp || signalIdx < 0 || signalIdx >= object->metaObject()->methodCount())
        return false;

	if(!isAllowObjectAccess(comp, object, remoteApp))
		return false;

    IObjectRemoteAccessControl* objectRAC = qobject_cast<IObjectRemoteAccessControl*>(object);
    if( objectRAC )
    {
        if( !objectRAC->isAllowObjectSignalConnect(signalIdx, remoteApp) )
            return false;
    }
	else
	{
		// Check what the GUIXML has to say.
		GCF::ComponentGuiNode node = object->property("_node_").value<GCF::ComponentGuiNode>();
		bool allowRSC = node.isValid() && node.hasKey("allowRemoteSignalConnect") && node["allowRemoteSignalConnect"].toBool();

		if(!allowRSC)
			return false;
	}

    return true;
}

/**
Returns true/false on whether the signal in \c object belonging to \c comp component can be accessed
remotely by the remote application handler \c remoteApp.

The function makes use of IComponentRemoteAccessControl interface in component (if-found) and
IObjectRemoteAccessControl interface in object (if-found) for making the decision. The former interface's
decision is given priority over the latters.
*/
bool GCF::RemoteApp::isAllowObjectSignalDelivery(GCF::AbstractComponent* comp, QObject* object, int signalIdx, GCF::RemoteApp* remoteApp)
{
    if( !object || !remoteApp || signalIdx < 0 || signalIdx >= object->metaObject()->methodCount())
        return false;

    IObjectRemoteAccessControl* objectRAC = qobject_cast<IObjectRemoteAccessControl*>(object);
    if( objectRAC )
    {
        if( !objectRAC->isAllowObjectSignalDelivery(signalIdx, remoteApp) )
            return false;
    }
	else
	{
		// Check what the GUIXML has to say.
		GCF::ComponentGuiNode node = object->property("_node_").value<GCF::ComponentGuiNode>();
		bool allowRSD = node.isValid() && node.hasKey("allowRemoteSignalDelivery") && node["allowRemoteSignalDelivery"].toBool();

		if(!allowRSD)
			return false;
	}

    return true;
}


////////////////////////////////////////////////////////////////////////////////////
// GCF::RemoteApp
////////////////////////////////////////////////////////////////////////////////////

struct GCF::SignalSpy1Data
{
    SignalSpy1Data() : remoteApp(0), objectOwner(0), objectId(-1) { }

    GCF::RemoteApp* remoteApp;
    GCF::AbstractComponent* objectOwner;
    qint32 objectId;
    QByteArray signal;
};

GCF::SignalSpy1::SignalSpy1(GCF::RemoteApp* parent, GCF::AbstractComponent* objectOwner, qint32 objectId, QObject* sender, const char* signal)
:GCF::GenericSignalSpy(parent, sender, signal)
{
    d = new GCF::SignalSpy1Data;
    d->remoteApp = parent;
    d->objectId = objectId;
    d->objectOwner = objectOwner;
    d->signal = QMetaObject::normalizedSignature(signal);
}

GCF::SignalSpy1::~SignalSpy1()
{
    // do nothing
}

QByteArray GCF::SignalSpy1::signal2() const
{
    return d->signal;
}

void GCF::SignalSpy1::handleSignalEmission(const QVariantList& args)
{
    if( !d->remoteApp->isAllowObjectSignalDelivery(d->objectOwner, sender(), signalMethodIndex(), d->remoteApp) )
        return;

    // Create a signal delivery message
    GCF::SignalDeliveryMessage* msg = new GCF::SignalDeliveryMessage;
    msg->setObjectId(d->objectId);
    msg->setSignal( QString(this->signal()) );
    msg->setArgList( args );

    // Send the signal delivery message via my remote application
    d->remoteApp->sendMessage(msg);
}


