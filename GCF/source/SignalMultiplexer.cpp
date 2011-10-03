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

#include <GCF/SignalMultiplexer.h>
#include <QMap>
#include <QList>
#include <QAction>
#include <QWidget>
#include <QWidgetAction>
#include <QApplication>

/**
\class GCF::SignalMultiplexer GCF::SignalMultiplexer.h
\brief Multiplexes signal(s) from a object to many slots.

Using GCF::SignalMultiplexer you can connect one or more signals from a object to
one or more signals/slots to another object. One or more receivers can be
marked active for receiving the signal.

The following code example can help understand this class better
\code
QAction* action = new QAction("My Action");
GCF::SignalMultiplexer* actionMx = new GCF::SignalMultiplexer(action);

QWidget* widget1 = new QWidget;
QWidget* widget2 = new QWidget;

actionMx->makeConnection(SIGNAL(triggered(bool)), widget1, SLOT(showMaximized()));
actionMx->makeConnection(SIGNAL(triggered(bool)), widget2, SLOT(showMinimized()));

action->trigger(); // triggered() signal sent to both widget1 and widget2

actionMx->setActiveReceiver(widget1, false);
action->trigger(); // triggered() signal sent to widget2 only

actionMx->setActiveReceiver(widget1, false);
actionMx->setActiveReceiver(widget2, true);
action->trigger(); // triggered() signal sent to widget1 only

actionMx->setActiveReceiver(widget2, false);
action->trigger(); // triggered() signal sent to none
\endcode
*/

namespace GCF
{
	struct SConnectionInfo
	{
		QString signal;
		QString slot;
	};
}

struct GCF::SignalMultiplexerData
{
    SignalMultiplexerData() : sender(0) { }

    QObject* sender;
    QMap< QObject*, QList<GCF::SConnectionInfo> > connectionMap;
    QMap< QObject*, bool > receiverMap;

    void enableSender(bool enable=true)
    {
        if(!sender)
            return;

        if(sender->isWidgetType())
            ((QWidget*)(sender))->setEnabled(enable);
        else
        {
            QAction* action = qobject_cast<QAction*>(sender);
			QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action);
			if(widgetAction && !enable)
				return;

            if(action)
            {
                action->setEnabled(enable);
                if(action->property("hideWhenNotInUse").toBool())
                    action->setVisible(enable);
            }
        }
    }
};

/**
Constructs a signal multiplexer for multiplexing signals originating from \obj. The
multiplexer is installed as a child of \c parent. If \c parent is a NULL pointer then
the multiplexer is added as a child to \c qApp.
*/
GCF::SignalMultiplexer::SignalMultiplexer(QObject* obj, QObject* parent)
:QObject(parent ? parent : qApp)
{
    d = new GCF::SignalMultiplexerData;
    d->sender = obj;
	d->enableSender(false);
}

/**
Destroys the multiplexer. Before deleting itself, all receivers are made active.
*/
GCF::SignalMultiplexer::~SignalMultiplexer()
{
    QList<QObject*> rxList = d->receiverMap.keys();
    for(int i=0; i<rxList.count(); i++)
        setActiveReceiver(rxList[i], true);
    delete d;
    d = 0;
}

/**
Returns the object whose signals are being multiplexed
*/
QObject* GCF::SignalMultiplexer::object() const
{
    return d->sender;
}

/**
Makes a connection between object(), \c signal, \c receiver, \c member. The receiver will
automatically become a active receiver. Some points worth noting here

\li If a receiver is being added for the first time, then that receiver will be assumed
to be an active receiver.
\li If the receiver was already added, then the connection is established only if the
receiver is an active receiver. Otherwise the connection is made a note of for later use.
*/
void GCF::SignalMultiplexer::makeConnection(const char* signal, QObject* receiver, const char* member)
{
    if(!signal || !receiver || !member)
        return;

    if(!d->connectionMap.contains(receiver))
    {
        QList<GCF::SConnectionInfo> infoList;
        d->connectionMap[receiver] = infoList;
        d->receiverMap[receiver] = true;
        connect(receiver, SIGNAL(destroyed(QObject*)), this, SLOT(receiver_destroyed(QObject*)));
    }

    GCF::SConnectionInfo info;
    if(signal[0] == '2')
        info.signal = QString(signal);
    else
        info.signal = QString("2%1").arg(signal);
    if(member[0] == '1')
        info.slot = QString(member);
    else
        info.slot = QString("1%1").arg(member);
    d->connectionMap[receiver].append(info);

    if(d->receiverMap[receiver])
    {
        connect(d->sender, info.signal.toAscii().data(), receiver, info.slot.toAscii().data());
        d->enableSender();
    }
}

/**
Breaks all connections with \c receiver.
*/
void GCF::SignalMultiplexer::breakConnection(QObject* receiver)
{
    if(!receiver || !d->receiverMap.contains(receiver))
        return;

    setActiveReceiver(receiver, false);
    d->connectionMap.remove(receiver);
    d->receiverMap.remove(receiver);
    disconnect(receiver, 0, this, 0);
}

/**
Sets \c receiver as an active receiver if \c active is true. Active receiver gets signal emission
notifications, while inactive receivers dont get them.
*/
void GCF::SignalMultiplexer::setActiveReceiver(QObject* receiver, bool active)
{
    if(!d->receiverMap.contains(receiver))
        return;

    if(d->receiverMap[receiver] == active)
        return;

    QList<GCF::SConnectionInfo> & infoList = d->connectionMap[receiver];
    for(int i=0; i<infoList.count(); i++)
    {
        GCF::SConnectionInfo info = infoList[i];
        if(active)
            connect(d->sender, info.signal.toAscii().data(), receiver, info.slot.toAscii().data());
        else
            disconnect(d->sender, info.signal.toAscii().data(), receiver, info.slot.toAscii().data());
    }

    d->receiverMap[receiver] = active;

    bool enable = false;
    QMap<QObject*, bool>::const_iterator it = d->receiverMap.begin();
    QMap<QObject*, bool>::const_iterator end = d->receiverMap.end();
    while(it != end)
    {
        if(it.value())
        {
            enable = true;
            break;
        }
        ++it;
    }

    d->enableSender(enable);
}

/**
Returns true if \c receiver is an active receiver.
*/
bool GCF::SignalMultiplexer::isActiveReceiver(QObject* receiver)
{
    return d->receiverMap.contains(receiver) && d->receiverMap[receiver];
}

/**
Returns a list of all receivers.
*/
QList<QObject*> GCF::SignalMultiplexer::receivers() const
{
    return d->receiverMap.keys();
}

/**
Returns ture if this object has any receivers.
*/
bool GCF::SignalMultiplexer::hasReceivers() const
{
    if(!d)
        return false;
    return d->receiverMap.count() > 0;
}

void GCF::SignalMultiplexer::receiver_destroyed(QObject* obj)
{
    breakConnection(obj);
}

void GCF::SignalMultiplexer::sender_destroyed(QObject* obj)
{
    d->sender = 0;
    d->connectionMap.clear();
    d->receiverMap.clear();
    Q_UNUSED(obj);
}


