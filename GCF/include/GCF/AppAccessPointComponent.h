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

#ifndef APP_ACCESS_POINT_COMPONENT_H
#define APP_ACCESS_POINT_COMPONENT_H

#include "AbstractComponent.h"
#include "IConfigurationDialogPageProvider.h"
#include <QWidget>

class QTextEdit;

namespace GCF
{
    class AppAccessPoint;
	class RemoteApp;
	class RemoteAppAccessPoint;

	struct AppAccessPointComponentData;
    class GCF_EXPORT_COMPONENT AppAccessPointComponent : public AbstractComponent,
                                                      public IConfigurationDialogPageProvider
    {
        Q_OBJECT
        Q_INTERFACES(IConfigurationDialogPageProvider)
        GCF_DECLARE_COMPONENT(AppAccessPointComponent)

    public:
        static AppAccessPointComponent & instance();
        ~AppAccessPointComponent();

        AppAccessPoint* appAccessPoint() const;
        QList<RemoteApp*> connectedRemoteAppList() const;
        QList<RemoteAppAccessPoint*> remoteAccessPoints() const;

        QTextEdit* messageLog() const;
        void appendMessageLog(const QString& text);

    public: // Purely for the sake of usage within a scripting environment only.
        Q_INVOKABLE QObject* getRemoteApplication(const QString& ipAddress, int port) const;
        Q_INVOKABLE QObject* getRemoteObject(QObject* remoteApp, const QString& objectName) const;
        Q_INVOKABLE QObject* getLocalObject(const QString& objectName) const;

    protected:
        AppAccessPointComponent();
        QObject* fetchObject(const QString& completeName) const;
        QWidget* fetchWidget(const QString& completeName) const;
        void initializeComponent();
        void finalizeComponent();

    public:
        // IContainer implementation
        QObject* containerObject();

        // IConfigurationDialogPageProvider implementation
        QString pageTitle() const;
        QIcon pageIcon() const;
        QWidget* createPage(QWidget* parent);
        void savePage(QWidget* page);

    private:
        AppAccessPointComponentData* d;
    };

    struct AppAccessPointConfigPageData;
    class GCF_EXPORT_CLASS AppAccessPointConfigPage : public QWidget
    {
        Q_OBJECT

    public:
        AppAccessPointConfigPage(QWidget* parent=0);
        ~AppAccessPointConfigPage();

    protected slots:
        void on_cmdMeToAppsAdd_clicked();
        void on_cmdMeToAppsRemove_clicked();
        void on_cmdAppsToMeRemove_clicked();

    private:
        AppAccessPointConfigPageData* d;
    };
}

#endif
