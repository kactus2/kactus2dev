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

#include "MessageLogComponent.h"
#include <GCF/ComponentFactory.h>
#include <GCF/IComponentPlugin.h>
#include <QTextEdit>
#include <QApplication>
#include <QScrollBar>

GCF_DEFINE_STD_COMPONENT(MessageLogComponent)

/**
\defgroup messagelogcomponent Message Log Component
\ingroup gcfcomponents

The message log component provides 4 text edit widgets on to which messages from
qDebug(), qWarning(), qCritical() and qFatal() will be forwarded to.

*/

struct GCF::Components::MessageLogComponentData
{
    MessageLogComponentData() :
        debug(0), warning(0), critical(0), fatal(0) { }

    QTextEdit* debug;
    QTextEdit* warning;
    QTextEdit* critical;
    QTextEdit* fatal;

    void scrollToEnd(QTextEdit* textEdit);
};

void MessageLogComponent_MsgHandler(QtMsgType type, const char *msg)
{
    GCF::Components::MessageLogComponent& log = GCF::Components::MessageLogComponent::instance();
    QString msgStr(msg);
    switch (type)
    {
    case QtDebugMsg:
        log.debugMessage(msgStr);
        break;
    case QtWarningMsg:
        log.warningMessage(msgStr);
        break;
    case QtCriticalMsg:
        log.criticalMessage(msgStr);
        break;
    case QtFatalMsg:
        log.fatalMessage(msgStr);
        abort();
    }
}

/**
\ingroup messagelogcomponent
\class GCF::Components::MessageLogComponent GCF::Components::MessageLogComponent.h
\brief Provides widgets and functionality for the message log

This component class provides a four text areas namely

\li debugWindow (QTextEdit instance)
\li warningWindow (QTextEdit instance)
\li criticalWindow (QTextEdit instance)
\li fatalWindow (QTextEdit instance)

Each of the above text areas are created only if the corresponding <widget> tag is
provided in GCF::Components::MessageLogComponent.xml file. By default all the text areas are created
and placed in the standard mainwindow's bottom dock widget area, organized into a tab
widget.

\li The debugWindow, shows debug messages (from qDebug).
\li The warningWindow, shows warning messages (from qWarning).
\li The criticalWindow, shows critical messages (from qCritical).
\li The fatalWindow, shows fatal messages (from qFatal).

By default this component object is available within the scripting environment
under the name "MessageLog".

Shown below is a screenshot of the message log component's widgets.

\image html messagelog.png
*/

GCF::Components::MessageLogComponent & GCF::Components::MessageLogComponent::instance()
{
    static GCF::Components::MessageLogComponent* theInstance = GCF_CREATE_COMPONENT(MessageLogComponent);
    return *theInstance;
}

/**
Constructor
*/
GCF::Components::MessageLogComponent::MessageLogComponent()
{
    d = new GCF::Components::MessageLogComponentData;
}

/**
Destructor
*/
GCF::Components::MessageLogComponent::~MessageLogComponent()
{
    delete d;
}

/**
Forwards messages to the debug window. Each message will appear on a new line.

\param msg The message to show in the debug window.

\note If debugWindow was not created, this function does nothing
*/
void GCF::Components::MessageLogComponent::debugMessage(const QString& msg)
{
    if(d->debug)
    {
        d->debug->append(msg);
        d->scrollToEnd( d->debug );
        qApp->sendPostedEvents(d->debug, QEvent::Paint);
    }
}

/**
Forwards messages to the warning window. Each message will appear on a new line.

\param msg The message to show in the warning window.

\note If warningWindow was not created, this function does nothing
*/
void GCF::Components::MessageLogComponent::warningMessage(const QString& msg)
{
    if(d->warning)
    {
        d->warning->append(msg);
        d->scrollToEnd( d->warning );
        qApp->sendPostedEvents(d->warning, QEvent::Paint);
    }
}

/**
Forwards messages to the critical window. Each message will appear on a new line.

\param msg The message to show in the critical window.

\note If criticalWindow was not created, this function does nothing
*/
void GCF::Components::MessageLogComponent::criticalMessage(const QString& msg)
{
    if(d->critical)
    {
        d->critical->append(msg);
        d->scrollToEnd( d->critical );
        qApp->sendPostedEvents(d->critical, QEvent::Paint);
    }
}

/**
Forwards messages to the fatal window. Each message will appear on a new line.

\param msg The message to show in the fatal window.

\note If fatalWindow was not created, this function does nothing
*/
void GCF::Components::MessageLogComponent::fatalMessage(const QString& msg)
{
    if(d->fatal)
    {
        d->fatal->append(msg);
        d->scrollToEnd( d->fatal );
        qApp->sendPostedEvents(d->fatal, QEvent::Paint);
    }
}

QObject* GCF::Components::MessageLogComponent::fetchObject(const QString& completeName)const
{
    if(completeName.split('.').last() == "controller")
	{
		GCF::Components::MessageLogComponent* that = const_cast<GCF::Components::MessageLogComponent*>(this);
        return that;
	}

    return 0;
}

QWidget* GCF::Components::MessageLogComponent::fetchWidget(const QString& completeName)const
{
    QStringList comps = completeName.split('.');

    if(comps.last() == "debugWindow")
    {
        if(d->debug)
            return d->debug;
        d->debug = new QTextEdit;
        d->debug->setReadOnly(true);
        return d->debug;
    }

    if(comps.last() == "warningWindow")
    {
        if(d->warning)
            return d->warning;
        d->warning = new QTextEdit;
        d->warning->setReadOnly(true);
        return d->warning;
    }

    if(comps.last() == "criticalWindow")
    {
        if(d->critical)
            return d->critical;
        d->critical = new QTextEdit;
        d->critical->setReadOnly(true);
        return d->critical;
    }

    if(comps.last() == "fatalWindow")
    {
        if(d->fatal)
            return d->fatal;
        d->fatal = new QTextEdit;
        d->fatal->setReadOnly(true);
        return d->fatal;
    }

    return 0;
}

void GCF::Components::MessageLogComponent::addChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
    Q_UNUSED(parent);
    Q_UNUSED(hint);
    child->setParent(0); // placing child widgets on text edits is not possible.
}

void GCF::Components::MessageLogComponent::justActivated()
{
    qInstallMsgHandler(MessageLogComponent_MsgHandler);
}

void GCF::Components::MessageLogComponent::initializeComponent()
{
    // Do nothing.
}

void GCF::Components::MessageLogComponent::finalizeComponent()
{
    qInstallMsgHandler(0);
}

void GCF::Components::MessageLogComponentData::scrollToEnd(QTextEdit* textEdit)
{
    textEdit->verticalScrollBar()->setValue( textEdit->verticalScrollBar()->maximum() );
}

GCF_EXPORT_STD_COMPONENT_PLUGIN(MessageLogComponent)

