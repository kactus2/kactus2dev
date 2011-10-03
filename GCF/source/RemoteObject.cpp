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

#include <GCF/RemoteObject.h>
#include <GCF/RemoteAppAccessPoint.h>
#include <GCF/Message.h>
#include <GCF/MethodInvocationHelper.h>

#include <QPointer>

/**
\class GCF::RemoteObject GCF::RemoteObject.h
\ingroup gcfipc
\brief Represents an object in the remote application.

Instances of this class represent a single object in the remote application. Using
an instance of this object, you can

\li invoke methods on the remote object using the \ref call() method.
\li set/get properties of the remote object.
\li connect/disconnect to signals and/or slots in the remote object

Usage:
\code
QHostAddress remoteAppHost = ...
qint16 remoteAppPort = ...

// Connect to a remote application
GCF::RemoteAppAccessPoint* remoteApp = GCF::RemoteAppAccessPoint::getRemoteAppAccessPoint(remoteAppHost, remoteAppPort);

// Fetch a pointer to the remote object
GCF::RemoteObject* remoteMainWin = remoteApp->getRemoteObject("CMainWindowComponent.window");

// Place calls on the remote object
qint32 callId = remoteMainWin->call("setWindowTitle", "MainWindow title set remotely");

// remoteMainWin emits a methodInvocationResult() after it gets a confirmation
// from the other application that the method was actually invoked.

\endcode

This class emits \ref activated() and \ref deactivated() signals when it becomes active or inactive.
By active we mean that there is an actual remote object that this class can communicate with.

Method invocations, property set/get, signal connect/disconnect and signal delivery are queued
by this class if it is not active. The moment it becomes active all queued messages are sent to
the other side.

\section Supported data-types
Remote method invocations brings on the table some challenges. For example it is not possible
to use all kinds of data types during message transactions. Currently the following data-types
are supported for use \ref call(), \ref setObjectProperty() and \ref objectProperty().

\li QVariant::Bool
\li QVariant::Char
\li QVariant::Color
\li QVariant::Date
\li QVariant::DateTime
\li QVariant::Double
\li QVariant::Int (int, qint32 and qint64)
\li QVariant::Time
\li QVariant::UInt (uint, quint32 and quint64)
\li QVariant::String
\li QVariant::Pixmap
\li QVariant::ByteArray
\li QVariant::BitArray
\li QVariant::Image
\li QVariant::Url
\li QVariant::StringList
\li QVariant::SizePolicy
\li QVariant::SizeF
\li QVariant::Size
\li QVariant::RegExp
\li QVariant::RectF
\li QVariant::Rect
\li QVariant::Polygon
\li QVariant::PointF
\li QVariant::Point
\li QVariant::Matrix
\li QVariant::LineF
\li QVariant::Line
\li QVariant::Brush
\li QVariant::Bitmap
\li QVariant::Transform

\note You cannot create instances of this class directly. You have to make use of
\ref GCF::RemoteAppAccessPoint::getRemoteObject() for it.
*/

namespace GCF
{
	struct SReceiver
	{
		SReceiver() : receiver(0) { }
		SReceiver(QObject* ptr, const char* slot) {
			receiver = ptr;
			member = QMetaObject::normalizedSignature(slot+1);
		}

		QPointer<QObject> receiver;
		QByteArray member;

		bool operator == (const SReceiver& other) const {
			return ((QObject*)(receiver) == (QObject*)(other.receiver)) &&
				(member == other.member);
		}
	};
}

struct GCF::RemoteObjectData
{
    RemoteObjectData() : id( -1 ), activated(false) { }

    GCF::RemoteObject* remoteObject;
    GCF::RemoteAppAccessPoint* remoteAppAccessPoint;
    qint32 id;
    GCF::DataStore objectProperties;
    bool activated;
    GCF::DataStore customData;

    QList<GCF::Message*> queuedMessages;
    QList<qint32> cachedPropMsgIdList;

    // Signal connection map (remote signal to local slot)
    QMap< QByteArray, QList<GCF::SReceiver> > signalReceiverMap;

    // Signal connection list (local signal to remote slot)
    QList<GCF::SignalSpy2*> signalSpyList;
};

/**
Constructor
*/
GCF::RemoteObject::RemoteObject(GCF::RemoteAppAccessPoint* remotePt)
:QObject(remotePt)
{
    d = new GCF::RemoteObjectData;
    d->remoteObject = this;
    d->remoteAppAccessPoint = remotePt;
}

/**
Destructor. Sends a \ref GCF::Message::ReleaseObject message to the other side
before destroying self.
*/
GCF::RemoteObject::~RemoteObject()
{
    qDeleteAll(d->signalSpyList);
    d->signalSpyList.clear();

    if( d->id >= 0 )
    {
        GCF::ReleaseObjectMessage* msg = new GCF::ReleaseObjectMessage;
        msg->setObjectId(d->id);
        // d->remoteAppAccessPoint->sendMessage(msg);
        sendMessage(msg);
    }
    delete d;
}

/**
\internal
*/
void GCF::RemoteObject::setId(qint32 id)
{
    if( d->id < 0 )
    {
        d->id = id;

        if( d->id >= 0 )
        {
            d->activated = true;
            emit activated();

            QMetaObject::invokeMethod(this, "sendQueuedMessages", Qt::QueuedConnection);
        }
    }
    else if( d->id >= 0 && id < 0 )
    {
        d->id = id;
        emit deactivated();

        qDeleteAll(d->signalSpyList);
        d->signalSpyList.clear();
    }
}

/**
Returns a pointer to an instance of \ref GCF::RemoteAppAccessPoint using which this object
was created. All messages from this object are sent to the other side via this remote
application access point.
*/
GCF::RemoteAppAccessPoint* GCF::RemoteObject::remoteAppAccessPoint() const
{
    return d->remoteAppAccessPoint;
}

/**
Returns if the object is active or not. Same as calling id() >= 0.
*/
bool GCF::RemoteObject::isActive() const
{
    return d->id >= 0;
}

/**
Returns an object ID. This ID is used while communicating messages between
this object and its peer object on the other side.
*/
qint32 GCF::RemoteObject::id() const
{
    return d->id;
}

/**
Custom information specific to this remote object can be stored in the data
store returned by this function.
*/
GCF::DataStore GCF::RemoteObject::customData() const
{
    return d->customData;
}

/**
Places a call to the remote object. The function returns a CallID, which you can use in
conjunction with \ref methodInvocationResult() signal to figure out if the method
invocation was successful or not.
*/
qint32 GCF::RemoteObject::call(const QString& method, const QVariant& arg1, const QVariant& arg2,
                         const QVariant& arg3, const QVariant& arg4,
                         const QVariant& arg5, const QVariant& arg6,
                         const QVariant& arg7, const QVariant& arg8)
{
    QVariantList args;
    if( arg1.isValid() )
        args << arg1;
    if( arg2.isValid() )
        args << arg2;
    if( arg3.isValid() )
        args << arg3;
    if( arg4.isValid() )
        args << arg4;
    if( arg5.isValid() )
        args << arg5;
    if( arg6.isValid() )
        args << arg6;
    if( arg7.isValid() )
        args << arg7;
    if( arg8.isValid() )
        args << arg8;

    return this->call(method, args);
}

/**
Places a call to the remote object. The function returns a CallID, which you can use in
conjunction with \ref methodInvocationResult() signal to figure out if the method
invocation was successful or not.
*/
qint32 GCF::RemoteObject::call(const QString& method, const QVariantList& args)
{
    // Prepare an InvokeMethod message
    GCF::InvokeMethodMessage* msg = new GCF::InvokeMethodMessage;
    msg->setObjectId(d->id);
    msg->setMethodName(method);
    msg->setArgList(args);

    // Note the message id
    qint32 msgId = msg->id();

    // send the message to the remote application.
    sendMessage(msg);

    // Return the message id
    return msgId;
}

/**
Makes a connection. (Remote Signal to Local Slot)

\param signal Name of the signal in the remote object.
\param receiver Pointer to a local receiver object
\paral slot A member (may be a signal or a slot member) in the receiver object.

The function returns an ID, which you can use along with the makeConnectionResult()
signal to figure out whether the signal connection was actually made or not.

\note Signal parameters are passed from the remote signal to the local slots. Standard
Qt's connection logic applies here.
\note The first parameter must be constructed using the SIGNAL() macro
\note The last parameter must be constructed using the SIGNAL() or SLOT() macro.
\note Multiple slot connections to the same signal in the remote object will not
result in multiple message transactions. Optimized mesage transactions are handled
by GCF.

The function will return -1 on error.
*/
qint32 GCF::RemoteObject::makeConnection(const char* signal, QObject* receiver, const char* slot)
{
    if( !signal || !receiver || !slot )
        return -1;

    if( *signal != '2' )
        return -1;

    if( !(*slot >= '0' && *slot <= '2') )
        return -1;

    // Prepare a RequestConnection message
    GCF::RequestConnectionMessage* msg = new GCF::RequestConnectionMessage;

    // Note the mesasage id
    qint32 msgId = msg->id();

    // First figure out if the connection has already been made
    QByteArray signature = QMetaObject::normalizedSignature(signal+1);
    if( d->signalReceiverMap.contains(signature) )
    {
        // We don't need to send any message, so let's delete the message
        delete msg;

        // Just add the receiver and the slot to the list of receivers
        d->signalReceiverMap[signature].append( GCF::SReceiver(receiver, slot) );

        // Return a dummy connection id, but before that schedule a signal
        // emission so that the receiver can expect a signal emission
        QMetaObject::invokeMethod(this, "makeConnectionResult", Qt::QueuedConnection,
            Q_ARG(qint32, msgId), Q_ARG(bool, true), Q_ARG(QString, QString(signature)));

        // Return the message id
        return msgId;
    }

    // prepend the signal code
    signature.insert(0, '2');

    msg->setObjectId(d->id);
    msg->setSignal(signature);
    msg->customData()["QObject*"] = qVariantFromValue<QObject*>(receiver);
    msg->customData()["slot"] = QByteArray(slot);

    // send the message to the other end
    sendMessage(msg);

    // return message id
    return msgId;
}

/**
Makes a connection. (Local Signal to Remote Slot)

\param sender Pointer to a local object.
\param signal Signal emitted by the local object
\param member The slot/signal to invoke in the remote object.

\note Signal/slot connections made using this function are realized by making use
of a call() method internally. The \ref GCF::GenericSignalSpy class is used to spy for
signals, and when the signal emission occors this class places a call to \c member.

\note Signal parameters are passed from the remote signal to the local slots. Standard
Qt's connection logic applies here.

The function returns true/false to indicate whether the connection was made or not.
*/
bool GCF::RemoteObject::makeConnection(QObject* sender, const char* signal, const char* member)
{
    if( !sender || !signal || !member )
        return false;

    if( *signal != '2' )
        return false;

    if( !(*member >= '0' && *member <= '2') )
        return false;

    QByteArray signalSig = QMetaObject::normalizedSignature(signal);
    for(int i=0; i<d->signalSpyList.count(); i++)
    {
        GCF::SignalSpy2* spy = d->signalSpyList[i];
        if( spy->sender() == sender && spy->signal2() == signalSig )
            return true;
    }

    GCF::SignalSpy2* spy = new GCF::SignalSpy2(this, sender, signal, member);
    d->signalSpyList.append(spy);
    return true;
}

/**
Breaks a connection (Remote Signal to Local Slot)

\param signal Name of the signal in the remote object.
\param receiver Pointer to a local receiver object
\paral slot A member (may be a signal or a slot member) in the receiver object.

If multiple slots are connected to the same signal, then this class waits until all
those slots are disconnected before sending a disconnect signal message to the remote
object.

The function returns true/false to indicate whether the connection was broken or not.
*/
bool GCF::RemoteObject::breakConnection(const char* signal, QObject* receiver, const char* slot)
{
    if( !signal || !receiver || !slot )
        return false;

    if( *signal != '2' )
        return false;

    if( !(*slot >= '0' && *slot <= '2') )
        return false;

    QByteArray signature = QMetaObject::normalizedSignature(signal+1);
    if( d->signalReceiverMap.contains(signature) )
    {
        QList<GCF::SReceiver>& receiverList = d->signalReceiverMap[signature];
        GCF::SReceiver r(receiver, slot);
        int nr = receiverList.removeAll(r) > 0;

        if( !receiverList.count() )
        {
            d->signalReceiverMap.remove(signature);

            // All connections to this signal is now over, so let's
            // send a message to the other end that we no longer need
            // signal delivery messages for this signal.
            GCF::ReleaseConnectionMessage* msg = new GCF::ReleaseConnectionMessage;
            msg->setObjectId(d->id);
            msg->setSignal( "2" + QString(signature) );
            sendMessage(msg);
        }

        return nr > 0;
    }

    return false;
}

/**
Break all connections between the remote object's \c signal and any slot connected
in the local object \c receiver.

The function returns true/false to indicate whether the connection was broken or not.
*/
bool GCF::RemoteObject::breakConnection(const char* signal, QObject* receiver)
{
    if( !signal || !receiver )
        return false;

    if( *signal != '2' )
        return false;

    QByteArray signature = QMetaObject::normalizedSignature(signal+1);
    if( d->signalReceiverMap.contains(signature) )
    {
        QList<GCF::SReceiver>& receiverList = d->signalReceiverMap[signature];
        bool success = false;
        for(int i=0; i<receiverList.count(); i++)
        {
            if( receiverList[i].receiver == receiver )
            {
                receiverList.removeAt(i);
                --i;
                success = true;
            }
        }

        if( !receiverList.count() )
        {
            d->signalReceiverMap.remove(signature);

            // All connections to this signal is now over, so let's
            // send a message to the other end that we no longer need
            // signal delivery messages for this signal.
            GCF::ReleaseConnectionMessage* msg = new GCF::ReleaseConnectionMessage;
            msg->setObjectId(d->id);
            msg->setSignal( "2" + QString(signature) );
            sendMessage(msg);
        }

        return success;
    }

    return false;
}

/**
Breaks a connection made between local object \c sender 's signal (\c signal) to the remote member.

The function returns true/false to indicate whether the connection was broken or not.
*/
bool GCF::RemoteObject::breakConnection(QObject* sender, const char* signal, const char* member)
{
    if( !sender || !signal || !member )
        return false;

    if( *signal != '2' )
        return false;

    if( !(*member >= '0' && *member <= '2') )
        return false;

    QByteArray signalSig = QMetaObject::normalizedSignature(signal);
    bool success = false;
    for(int i=0; i<d->signalSpyList.count(); i++)
    {
        GCF::SignalSpy2* spy = d->signalSpyList[i];
        if( spy->sender() == sender && spy->signal2() == signalSig )
        {
            d->signalSpyList.removeAt(i);
            success = true;
            --i;
        }
    }

    return success;
}

/**
This function breaks all connections from any remote signal to any slot in \c sender.

The function returns true/false to indicate whether the connection was broken or not.
*/
bool GCF::RemoteObject::breakConnection(QObject* sender)
{
    if( !sender )
        return false;

    bool success = false;
    for(int i=0; i<d->signalSpyList.count(); i++)
    {
        GCF::SignalSpy2* spy = d->signalSpyList[i];
        if( spy->sender() == sender )
        {
            d->signalSpyList.removeAt(i);
            success = true;
            --i;
        }
    }

    return success;
}

/**
This function breaks all connections with the remote signal with any object/slot.

The function returns true/false to indicate whether the connection was broken or not.
*/
bool GCF::RemoteObject::breakConnection(const char* signal)
{
    if( !signal )
        return false;

    QByteArray signature = QMetaObject::normalizedSignature(signal+1);
    if( d->signalReceiverMap.contains(signature) )
    {
        d->signalReceiverMap.remove(signature);

        // All connections to this signal is now over, so let's
        // send a message to the other end that we no longer need
        // signal delivery messages for this signal.
        GCF::ReleaseConnectionMessage* msg = new GCF::ReleaseConnectionMessage;
        msg->setObjectId(d->id);
        msg->setSignal( "2" + QString(signature) );
        sendMessage(msg);

        return true;
    }

    return false;
}

/**
Sets a property in the remote object. The name and value of the property are passed
as parameters.

The function returns an ID, which you can use in conjunction with the setPropertyResult()
method to figure out whether the operation was successful or not.
*/
qint32 GCF::RemoteObject::setObjectProperty(const QString& propName, const QVariant& value)
{
    // Prepare a SetProperty message
    GCF::SetPropertyMessage* msg = new GCF::SetPropertyMessage;
    msg->setObjectId(d->id);
    msg->setPropertyName(propName);
    msg->setPropertyValue(value);

    // note down the message id
    qint32 msgId = msg->id();

    // send the message
    sendMessage(msg);

    // return the message id
    return msgId;
}

/**
Gets a property in the remote object. The function returns the last known value of
the property via the \c value parameter, and sends a GetProperty message to the other end.

Object properties are cached as and when their information is received from the other end.
This method returns the property value from the cache for immediate usage, while at the
same time it sends a GetProperty message to the other end to update its cache. If you just
want to fetch the cached property, then you can make use of the \ref getCachedObjectProperty()
method.

The function returns an ID, which you can use in conjunction with the getPropertyResult()
method to figure out whether the operation was successful or not.
*/
qint32 GCF::RemoteObject::getObjectProperty(const QString& propName, QVariant& value) const
{
    // Prepare a GetProperty message
    GCF::GetPropertyMessage* msg = new GCF::GetPropertyMessage;
    msg->setObjectId(d->id);
    msg->setPropertyName(propName);

    // note down the message id
    qint32 msgId = msg->id();

    // send the message
    d->remoteObject->sendMessage(msg);

    // Through value, return the currently known property value.
    value = d->objectProperties[propName];

    // return the message id
    return msgId;
}

/**
This function internally calls getObjectProperty(QString, QVariant). But it returns the
last known value of the property.
*/
QVariant GCF::RemoteObject::getObjectProperty(const QString& propName) const
{
    QVariant retVal;
    getObjectProperty(propName, retVal);
    return retVal;
}

/**
This function caches all properties of the remote object.

If the number of properties in the remote object is huge, then a large number
of message transactions might get triggered because of this function call. The
GUI wont freeze, because all message transactions happen via the standard event loop.
*/
void GCF::RemoteObject::cacheProperties()
{
    // If caching is already underway then forget about it
    if( d->cachedPropMsgIdList.count() )
        return;

    // prepare a GetPropertyNames message
    GCF::GetPropertyNamesMessage* msg = new GCF::GetPropertyNamesMessage;
    msg->setObjectId(d->id);

    // send message to the other end.
    sendMessage(msg);
}

/**
Returns the cached value of a property. If a cached value doesnt exist, then
an invalid value is returned.
*/
QVariant GCF::RemoteObject::getCachedObjectProperty(const QString& propName) const
{
    if( !d->objectProperties.hasKey(propName) )
        return QVariant();

    return d->objectProperties[propName];
}


/**
\internal
*/
void GCF::RemoteObject::sendMessage(GCF::Message* msg)
{
    if( d->id < 0 )
    {
        if(d->activated)
        {
            // this object was activated once, but has now gotten deactivated.
            // so, we should not queue the message. Because queued messages will
            // never get sent anyway.
            delete msg;
        }
        else
            d->queuedMessages.append(msg);
    }
    else
        d->remoteAppAccessPoint->sendMessage(msg);
}

/**
\internal
*/
bool GCF::RemoteObject::handleMessage(GCF::Message* received, GCF::Message* sent)
{
    switch(received->messageType())
    {
    case GCF::Message::InvokeMethodRes: {
        GCF::InvokeMethodResMessage* msg = (GCF::InvokeMethodResMessage*)received;
        if(sent)
            emit methodInvocationResult(sent->id(), msg->isSuccess(), msg->retValue(), msg->methodName());
        } break;

    case GCF::Message::SetPropertyRes: {
        GCF::SetPropertyResMessage* msg = (GCF::SetPropertyResMessage*)received;
        if( msg->isSuccess() )
            d->objectProperties[msg->propertyName()] = msg->propertyValue();

        emit setPropertyResult(msg->id(), msg->isSuccess(), msg->propertyValue(), msg->propertyName());
        } break;

    case GCF::Message::GetPropertyRes: {
        GCF::GetPropertyResMessage* msg = (GCF::GetPropertyResMessage*)received;
        if( msg->isSuccess() )
            d->objectProperties[msg->propertyName()] = msg->propertyValue();
        else
            d->objectProperties.removeKey(msg->propertyName());

        if( d->cachedPropMsgIdList.contains(msg->id()) )
        {
            d->cachedPropMsgIdList.removeAll(msg->id());
            if( !d->cachedPropMsgIdList.count() )
                emit propertiesCached();
        }

        emit getPropertyResult(msg->id(), msg->isSuccess(), msg->propertyValue(), msg->propertyName());
        } break;

    case GCF::Message::GetPropertyNamesRes: {
        GCF::GetPropertyNamesResMessage* msg = (GCF::GetPropertyNamesResMessage*)received;
        cacheProperties( msg->propertyNames() );
        } break;

    case GCF::Message::RequestConnectionRes: {
        if( !sent )
            return false;

        GCF::RequestConnectionResMessage* msg = (GCF::RequestConnectionResMessage*)received;
        if( msg->isSuccess() )
        {
            QByteArray signal = msg->signal().toAscii();
            QObject* receiver = sent->customData()["QObject*"].value<QObject*>();
            QByteArray slot = sent->customData()["slot"].toByteArray();
            d->signalReceiverMap[signal].append( GCF::SReceiver(receiver, slot.constData()) );
        }

        emit makeConnectionResult(msg->id(), msg->isSuccess(), msg->signal());

        } break;

    case GCF::Message::SignalDelivery: {
        GCF::SignalDeliveryMessage* msg = (GCF::SignalDeliveryMessage*)received;

        QByteArray signal = msg->signal().toAscii();
        QVariantList args = msg->argList();
        if( !d->signalReceiverMap.contains(signal) )
            return true;

        // Deliver the signal to the receivers
        QList<GCF::SReceiver>& receivers = d->signalReceiverMap[signal];
        for(int i=0; i<receivers.count(); i++)
        {
            GCF::SReceiver r = receivers[i];
            if( r.receiver.isNull() )
                continue;

            QVariant returnValue; // not needed anyway
            QString method = QString(r.member).section('(', 0, 0);
			//if( !(GCF::MethodInvokationHelper::callMethod(r.receiver, method, args, returnValue) )) //todo
			if( !(GCF::callMethod(r.receiver, method, args, returnValue) ))
            {
                receivers.removeAt(i);
                --i;
            }
        }

        } break;

    default:
        return false;
    }

    return true;
}

/**
\internal
*/
void GCF::RemoteObject::sendQueuedMessages()
{
    // send all queued messages
    for(int i=0; i<d->queuedMessages.count(); i++)
    {
        GCF::Message* msg = d->queuedMessages[i];
        msg->setObjectId( d->id );
        d->remoteAppAccessPoint->sendMessage( msg );
    }
    d->queuedMessages.clear();
}

/**
\internal
*/
void GCF::RemoteObject::cacheProperties(const QStringList& propNames)
{
    // If caching is already underway then forget about it
    if( d->cachedPropMsgIdList.count() )
        return;

    // Trigger the caching of properties.
    for(int i=0; i<propNames.count(); i++)
    {
        QVariant val;
        qint32 msgId = getObjectProperty(propNames[i], val);
        d->cachedPropMsgIdList.append(msgId);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// GCF::SignalSpy2
//////////////////////////////////////////////////////////////////////////////////////////

struct GCF::SignalSpy2Data
{
    GCF::RemoteObject* remoteObject;
    QByteArray signal;
    QByteArray method;
    QString methodName;
};

GCF::SignalSpy2::SignalSpy2(GCF::RemoteObject* remoteObj, QObject* sender, const char* signal, const char* method)
:GCF::GenericSignalSpy(remoteObj, sender, signal)
{
    d = new GCF::SignalSpy2Data;
    d->remoteObject = remoteObj;
    d->signal = QMetaObject::normalizedSignature(signal);
    d->method = QMetaObject::normalizedSignature(method);

    if( !d->method.isEmpty() )
        d->methodName = QString(d->method).section('(', 0, 0);

    if( d->methodName[0] >= '0' && d->methodName[0] <= '2' )
        d->methodName.remove(0, 1);
}

GCF::SignalSpy2::~SignalSpy2()
{
    delete d;
}


QByteArray GCF::SignalSpy2::signal2() const
{
    return d->signal;
}

QByteArray GCF::SignalSpy2::method() const
{
    return d->method;
}

void GCF::SignalSpy2::handleSignalEmission(const QVariantList& args)
{
    if( d->methodName.isEmpty() )
        return;

    d->remoteObject->call(d->methodName, args);
}

