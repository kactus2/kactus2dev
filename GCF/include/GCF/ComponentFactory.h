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

#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include "Common.h"
#include "AbstractComponent.h"
#include <QObject>
#include <QList>

namespace GCF
{
    struct ComponentFactoryData;
    class GCF_EXPORT_CLASS ComponentFactory : public QObject
    {
        Q_OBJECT

    public:
        static ComponentFactory & instance();
        ~ComponentFactory();

        void initialize();
        void finalize();

        const QMetaObject* componentMeta(const QString&  className) const;
        bool canCreate(const QString&  className) const;

        AbstractComponent* createComponent(const QString&  className);
        AbstractComponent* findComponent(const QString&  componentName) const;
        QList<AbstractComponent*> findComponents(const QString&  componentName) const;
        QList<AbstractComponent*> components() const;
        QStringList componentNames() const;

        QList<AbstractComponent*> activeComponents() const;
        QList<AbstractComponent*> inactiveComponents() const;

        void registerComponentInfo(const QString&  className, CreateComponentFunction createFn, const QMetaObject* meta);

    protected:
        ComponentFactory();
        void registerComponentInfo();
        void registerComponent(AbstractComponent* comp);
        void unregisterComponent(AbstractComponent* comp);
        void announceComponentCreation(AbstractComponent* comp);
        void announceComponentDestruction(AbstractComponent* comp);

    protected slots:
        void on_qApp_lastWindowClosed();

    private:
        friend class AbstractComponent;
        friend class AbstractComponentPlugin;
        ComponentFactoryData* d;
    };

}

#endif
