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

#ifndef I_COMPONENT_PLUGIN_H
#define I_COMPONENT_PLUGIN_H

#include "Common.h"
#include "AbstractComponent.h"

class GCF_EXPORT_INTERFACE IComponentPlugin
{
public:
    static QString systemBuildKey();
    virtual QString componentName() const = 0;
    virtual void registerComponent() = 0;
    virtual const QMetaObject* componentMeta() const = 0;
    virtual QString buildKey() const = 0;

protected:
    void registerComponentInfo(const QString& className,
        CreateComponentFunction createFn, const QMetaObject* meta);
};

typedef IComponentPlugin* (*CreateComponentPluginFunction)();

#if(defined(Q_OS_WIN32) || defined(Q_OS_WIN64))

#define GCF_EXPORT_COMPONENT_PLUGIN(ComponentClassName) \
class  __declspec(dllexport) ComponentClassName##_Plugin : public IComponentPlugin \
{ \
public: \
    QString componentName() const { return #ComponentClassName; } \
    const QMetaObject* componentMeta() const { return &ComponentClassName::staticMetaObject; } \
    void registerComponent() { registerComponentInfo(#ComponentClassName, ComponentClassName::CreateInstance, &ComponentClassName::staticMetaObject); } \
    QString buildKey() const { return GCF_BUILD_KEY; } \
}; \
extern "C"  __declspec(dllexport) IComponentPlugin* GetComponentPlugin(); \
IComponentPlugin* GetComponentPlugin() { return new ComponentClassName##_Plugin; }

#else

#define GCF_EXPORT_COMPONENT_PLUGIN(ComponentClassName) \
class ComponentClassName##_Plugin : public IComponentPlugin \
{ \
public: \
    QString componentName() const { return #ComponentClassName; } \
    const QMetaObject* componentMeta() const { return &ComponentClassName::staticMetaObject; } \
    void registerComponent() { registerComponentInfo(#ComponentClassName, ComponentClassName::CreateInstance, &ComponentClassName::staticMetaObject); } \
    QString buildKey() const { return GCF_BUILD_KEY; } \
}; \
extern "C" IComponentPlugin* GetComponentPlugin(); \
IComponentPlugin* GetComponentPlugin() { return new ComponentClassName##_Plugin; }

#endif

#if(defined(Q_OS_WIN32) || defined(Q_OS_WIN64))

#define GCF_EXPORT_STD_COMPONENT_PLUGIN(ComponentClassName) \
namespace GCF \
{ \
    namespace Components \
    { \
        class  __declspec(dllexport) ComponentClassName##_Plugin : public IComponentPlugin \
        { \
        public: \
            QString componentName() const { return #ComponentClassName; } \
            const QMetaObject* componentMeta() const { return &ComponentClassName::staticMetaObject; } \
            void registerComponent() { registerComponentInfo(#ComponentClassName, GCF::Components::ComponentClassName::CreateInstance, &GCF::Components::ComponentClassName::staticMetaObject); } \
            QString buildKey() const { return GCF_BUILD_KEY; } \
        }; \
    } \
} \
extern "C" __declspec(dllexport) IComponentPlugin* GetComponentPlugin(); \
IComponentPlugin* GetComponentPlugin() { return new GCF::Components::ComponentClassName##_Plugin; }

#else

#define GCF_EXPORT_STD_COMPONENT_PLUGIN(ComponentClassName) \
namespace GCF \
{ \
    namespace Components \
    { \
        class ComponentClassName##_Plugin : public IComponentPlugin \
        { \
        public: \
            QString componentName() const { return #ComponentClassName; } \
            const QMetaObject* componentMeta() const { return &ComponentClassName::staticMetaObject; } \
            void registerComponent() { registerComponentInfo(#ComponentClassName, GCF::Components::ComponentClassName::CreateInstance, &GCF::Components::ComponentClassName::staticMetaObject); } \
            QString buildKey() const { return GCF_BUILD_KEY; } \
        }; \
    } \
} \
extern "C" IComponentPlugin* GetComponentPlugin(); \
IComponentPlugin* GetComponentPlugin() { return new GCF::Components::ComponentClassName##_Plugin; }

#endif

#endif
