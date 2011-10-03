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

#ifndef APP_ACCESS_POINT_H
#define APP_ACCESS_POINT_H

#include "Common.h"
#include "GenericSignalSpy.h"

#include <QTcpServer>
#include <QVariant>

class QAbstractSocket;

namespace GCF
{
	class RemoteApp;
	class AbstractComponent;

	// Required for RemoteApp class...
	class Message;
	class RequestAccessMessage;	

	struct AppAccessPointData;
    class GCF_EXPORT_CLASS AppAccessPoint : public QTcpServer
    {
        Q_OBJECT
        Q_PROPERTY(QString AccessPointName READ accessPointName WRITE setAccessPointName)
        Q_PROPERTY(bool AppAccessEnabled READ isAppAccessEnabled WRITE setAppAccessEnabled)
        Q_PROPERTY(quint16 ServerPort READ port WRITE setPort)

    public:
        static AppAccessPoint & instance();
        ~AppAccessPoint();

        // Meta data about the access point.
        void setAccessPointName(const QString& name);
        QString accessPointName() const;

        void setAppAccessEnabled(bool val);
        bool isAppAccessEnabled() const;

        void setPort(quint16 port);
        quint16 port() const;

        // Ability to add/remove custom access points
		enum AllowedRemoteOperation
		{
			AllowRemoteMethodInvocation		= 1,
			AllowRemoteGetProperty			= 2,
			AllowRemoteSetProperty			= 4,
			AllowRemoteSignalConnect		= 8,
			AllowRemoteSignalDelivery		= 16,
			AllowAll						= 31 // OR of all of the above
		};
		Q_DECLARE_FLAGS(AllowedRemoteOperations, AllowedRemoteOperation);
        bool addExposedObject(AbstractComponent* owner, QObject* object, const QString& objectName=QString(), AllowedRemoteOperations allowedOps=AllowAll);
        bool removeExposedObject(AbstractComponent* owner, QObject* object);

        // Ability to map object names to QObjects
        QObject* findObject(const QString& completeName, AbstractComponent** owner=0, bool forRemoteAccess=false) const;

        // List of all connected remote applications
        QList<RemoteApp*> connectedRemoteAppList() const;

    protected:
        AppAccessPoint(QObject* parent);
        void addRemoteApp(RemoteApp* app);
        void removeRemoteApp(RemoteApp* app);

    protected slots:
        void slotNewConnection();

    private:
        friend class RemoteApp;
        AppAccessPointData* d;
    };


    struct RemoteAppData;
    class GCF_EXPORT_CLASS RemoteApp : public QObject
    {
        Q_OBJECT

    public:
        static RemoteApp* currentRemoteApp();
        ~RemoteApp();

        QAbstractSocket* socket() const;
        QString localInfo() const;
        QString peerInfo() const;

        void sendMessage(Message* message);

    protected slots:
        void slotDisconnected();
        void slotError();
        void slotAboutToClose();
        void slotReadyRead();
        void objectDestroyed(QObject* obj);
        void sendPendingMessages();

    protected:
        RemoteApp(QAbstractSocket* socket, QObject* parent=0);
        virtual bool handleMessage(Message* received);
        virtual bool handleObjectMessage(qint32 objectId, AbstractComponent* objectOwner, QObject* object, Message* received);

    protected:
        // access control methods. Works in tandem with IComponentRemoteAccessControl and
        // IObjectRemoteAccessControl interfaces that may be available with component and/or object.
        bool isAllowObjectAccess(AbstractComponent* comp, QObject* object, RemoteApp* remoteApp);
        bool isAllowObjectMethodInvoke(AbstractComponent* comp, QObject* object, int methodIdx, RemoteApp* remoteApp);
        bool isAllowObjectSetProperty(AbstractComponent* comp, QObject* object, int propIdx, RemoteApp* remoteApp);
        bool isAllowObjectGetProperty(AbstractComponent* comp, QObject* object, int propIdx, RemoteApp* remoteApp);
        bool isAllowObjectSignalConnect(AbstractComponent* comp, QObject* object, int signalIdx, RemoteApp* remoteApp);
        bool isAllowObjectSignalDelivery(AbstractComponent* comp, QObject* object, int signalIdx, RemoteApp* remoteApp);

    private:
        friend class AppAccessPoint;
        friend class SignalSpy1;
        RemoteAppData* d;
    };

    struct SignalSpy1Data;
    class SignalSpy1 : public GenericSignalSpy
    {
    public:
        SignalSpy1(RemoteApp* parent, AbstractComponent* objectOwner, qint32 objectId, QObject* sender, const char* signal);
        ~SignalSpy1();

        QByteArray signal2() const;

    protected:
        void handleSignalEmission(const QVariantList& args);

    private:
        SignalSpy1Data* d;
    };

}

Q_DECLARE_OPERATORS_FOR_FLAGS(GCF::AppAccessPoint::AllowedRemoteOperations);

#endif

