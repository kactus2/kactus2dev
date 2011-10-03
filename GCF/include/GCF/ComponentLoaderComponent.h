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

#ifndef COMPONENT_LOADER_COMPONENT_H
#define COMPONENT_LOADER_COMPONENT_H

#include "AbstractComponent.h"
#include <QWidget>

namespace GCF
{
    struct ComponentLoaderComponentData;
    class GCF_EXPORT_COMPONENT ComponentLoaderComponent : public AbstractComponent
    {
        Q_OBJECT
        GCF_DECLARE_COMPONENT(ComponentLoaderComponent)

    public:
        static ComponentLoaderComponent & instance();
        ~ComponentLoaderComponent();

        QStringList componentDirectories() const;

    public slots:
        void loadComponent(const QString& compName);
        void loadComponent2(const QString& compName, const QString& libraryFile);
        void addComponentDirectory(const QString& dir);
        void removeComponentDirectory(const QString& dir);
        void registerComponents();

    protected:
        ComponentLoaderComponent();
        QObject* fetchObject(const QString& completeName) const;
        void initializeComponent();
        void finalizeComponent();
        void componentCreated(AbstractComponent* comp);

    signals:
        void loadingComponent(const QString& message);

    private:
        ComponentLoaderComponentData* d;
    };

    struct ComponentLoaderConfigPageData;
    class GCF_EXPORT_CLASS ComponentLoaderConfigPage : public QWidget
    {
        Q_OBJECT

    public:
        ComponentLoaderConfigPage(QWidget* parent=0);
        ~ComponentLoaderConfigPage();

    private slots:
        void on_cmdAddPath_clicked();
        void on_cmdRemovePath_clicked();

    protected:
        void showEvent(QShowEvent* se);

    private:
        ComponentLoaderConfigPageData* d;
    };

    class GCF_EXPORT_COMPONENT ComponentLoaderConfigComponent : public AbstractComponent
    {
        Q_OBJECT
        GCF_DECLARE_COMPONENT(ComponentLoaderConfigComponent)

    public:
        ComponentLoaderConfigComponent();
        ~ComponentLoaderConfigComponent();

    protected:
        void initializeComponent();
        void finalizeComponent();
        QWidget* createWidget(const QString& completeName);
    };

}
#endif
