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

#include <GCF/AppAccessPointComponent.h>
#include <GCF/ComponentFactory.h>
#include <GCF/AppAccessPoint.h>
#include <GCF/RemoteAppAccessPoint.h>
#include <GCF/RemoteObject.h>

#include "ui_AddRemoteAppDialog.h"

#include <QTextEdit>
#include <QMessageBox>

struct GCF::AppAccessPointComponentData
{
    AppAccessPointComponentData() : messageLog(0), serverPort(-1) { }

    QTextEdit* messageLog;
    qint16 serverPort;
};

GCF_DEFINE_COMPONENT(GCF::AppAccessPointComponent)

GCF::AppAccessPointComponent& GCF::AppAccessPointComponent::instance()
{
    static GCF::AppAccessPointComponent* theInstance = GCF_CREATE_COMPONENT(GCF::AppAccessPointComponent);
    return *theInstance;
}

GCF::AppAccessPointComponent::AppAccessPointComponent()
{
    d = new GCF::AppAccessPointComponentData;
}

GCF::AppAccessPointComponent::~AppAccessPointComponent()
{
    delete d;
}

GCF::AppAccessPoint* GCF::AppAccessPointComponent::appAccessPoint() const
{
    return & GCF::AppAccessPoint::instance();
}

QList<GCF::RemoteApp*> GCF::AppAccessPointComponent::connectedRemoteAppList() const
{
    return GCF::AppAccessPoint::instance().connectedRemoteAppList();
}

QList<GCF::RemoteAppAccessPoint*> GCF::AppAccessPointComponent::remoteAccessPoints() const
{
    return GCF::RemoteAppAccessPoint::getRemoteAppAccessPointList();
}

QTextEdit* GCF::AppAccessPointComponent::messageLog() const
{
    return d->messageLog;
}

void GCF::AppAccessPointComponent::appendMessageLog(const QString& text)
{
    if( d->messageLog )
    {
        d->messageLog->append( text );
        qApp->processEvents();
    }
}

QObject* GCF::AppAccessPointComponent::getRemoteApplication(const QString& ipAddress, int port) const
{
    return GCF::RemoteAppAccessPoint::getRemoteAppAccessPoint( QHostAddress(ipAddress), qint16(port) );
}

QObject* GCF::AppAccessPointComponent::getRemoteObject(QObject* remoteAppObj, const QString& objectName) const
{
    GCF::RemoteAppAccessPoint* remoteApp = qobject_cast<GCF::RemoteAppAccessPoint*>(remoteAppObj);
    if( !remoteApp )
        return 0;

    return remoteApp->getRemoteObject(objectName);
}

QObject* GCF::AppAccessPointComponent::getLocalObject(const QString& objectName) const
{
    return GCF::AppAccessPoint::instance().findObject(objectName);
}

QObject* GCF::AppAccessPointComponent::fetchObject(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

	GCF::AppAccessPointComponent* that = const_cast<GCF::AppAccessPointComponent*>(this);

    if( comps.last() == "controller" )
        return that;

    if( comps.last() == "appAccessPoint")
        return that->appAccessPoint();

    return 0;
}

QWidget* GCF::AppAccessPointComponent::fetchWidget(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

    if( comps.last() == "messageLog" )
    {
        if( !d->messageLog )
        {
            d->messageLog = new QTextEdit;
            d->messageLog->setReadOnly(true);
            d->messageLog->setFont( QFont("Monospace") );
        }
        return d->messageLog;
    }

    return 0;
}

void GCF::AppAccessPointComponent::initializeComponent()
{
    // Open the application access point for others to use.
    GCF::AppAccessPoint::instance();
    setGuiXmlFileName(":/GCF/guixml/AppAccessPointComponent.xml");
}

void GCF::AppAccessPointComponent::finalizeComponent()
{
    // Do nothing
}

QObject* GCF::AppAccessPointComponent::containerObject()
{
    return this;
}

QString GCF::AppAccessPointComponent::pageTitle() const
{
    return "IPC Configuration";
}

QIcon GCF::AppAccessPointComponent::pageIcon() const
{
    return QIcon();
}

QWidget* GCF::AppAccessPointComponent::createPage(QWidget* parent)
{
    return new GCF::AppAccessPointConfigPage(parent);
}

void GCF::AppAccessPointComponent::savePage(QWidget* page)
{
    GCF::AppAccessPointConfigPage* configPage = qobject_cast<GCF::AppAccessPointConfigPage*>(page);
    if( !configPage )
        return;

    // do something about the page.
}

/////////////////////////////////////////////////////////////////////////////////

#include "ui_AppAccessPointConfigPage.h"

struct GCF::AppAccessPointConfigPageData
{
    Ui::AppAccessPointConfigPage ui;
};

GCF::AppAccessPointConfigPage::AppAccessPointConfigPage(QWidget* parent)
:QWidget(parent)
{
    d = new GCF::AppAccessPointConfigPageData;
    d->ui.setupUi(this);

    d->ui.txtAccessPointName->setText( GCF::AppAccessPoint::instance().accessPointName() );
    d->ui.chkEnableAccess->setChecked( GCF::AppAccessPoint::instance().isAppAccessEnabled() );
    d->ui.txtAddress->setText( GCF::AppAccessPoint::instance().serverAddress().toString() );
    d->ui.txtPort->setText( QString("%1").arg(GCF::AppAccessPoint::instance().serverPort()) );

    // Add to lstAppsToMe information about other clients that have connected
    // to me.
    QList<GCF::RemoteApp*> apps = GCF::AppAccessPoint::instance().connectedRemoteAppList();
    for(int i=0; i<apps.count(); i++)
    {
        QAbstractSocket* socket = apps[i]->socket();
        QString socketInfo = QString("%1/%2")
            .arg( socket->peerAddress().toString() )
            .arg( socket->peerPort() );
        d->ui.lstAppsToMe->addItem( socketInfo );
    }

    // Add to listMeToApps information about the applications that I have connected to
    QList<GCF::RemoteAppAccessPoint*> remoteApps = GCF::RemoteAppAccessPoint::getRemoteAppAccessPointList();
    for(int i=0; i<remoteApps.count(); i++)
    {
        GCF::RemoteAppAccessPoint* remoteApp = remoteApps[i];
        QAbstractSocket* socket = remoteApp->socket();
        if( !socket )
            continue;

        QString socketInfo = QString("%1/%2")
            .arg( socket->peerAddress().toString() )
            .arg( socket->peerPort() );
        d->ui.lstMeToApps->addItem( socketInfo );
    }
}

GCF::AppAccessPointConfigPage::~AppAccessPointConfigPage()
{
    delete d;
}

void GCF::AppAccessPointConfigPage::on_cmdMeToAppsAdd_clicked()
{
    QDialog dialog(this);
    Ui::AddRemoteAppDialog ui;
    ui.setupUi(&dialog);

    if( dialog.exec() == QDialog::Rejected )
        return;

    QString ipAddr = ui.txtIPAddress->text();
    int port = ui.txtPort->text().toInt();
    GCF::RemoteAppAccessPoint* pt = GCF::RemoteAppAccessPoint::getRemoteAppAccessPoint( QHostAddress(ipAddr), port );
    if( !pt )
    {
        QMessageBox::information(this, "Could not connect",
            QString("Could not connect to remote application at %1:%2").arg(ipAddr).arg(port));
        return;
    }

    QString socketInfo = QString("%1/%2").arg( ipAddr ).arg( port );
    d->ui.lstMeToApps->addItem( socketInfo );
}

void GCF::AppAccessPointConfigPage::on_cmdMeToAppsRemove_clicked()
{

}

void GCF::AppAccessPointConfigPage::on_cmdAppsToMeRemove_clicked()
{

}


