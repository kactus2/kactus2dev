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

#ifndef COMPONENT_SETTINGS_H
#define COMPONENT_SETTINGS_H

#include <GCF/DataStore>
#include <QDomDocument>
#include <QDomElement>

namespace GCF
{
    class AbstractComponent;
    struct AbstractComponentData;

    struct ComponentSettingsData;
    class GCF_EXPORT_CLASS ComponentSettings
    {
    public:
        void addGroup(const QString& name, const GCF::DataStore& store=GCF::DataStore());
        void removeGroup(const QString& name);
        void removeAllGroups();
        QStringList groupNames() const;
        GCF::DataStore group(const QString& name, bool addIfNotFound=true) const;
        GCF::DataStore defaultGroup() const;

        // Operates on the default store
        QVariant & operator [] (const QString& key);
        bool hasKey(const QString& key);
        void removeKey(const QString& key);
        QStringList keys() const;
        void clear();

        void reset();
        bool isEmpty() const;

    protected:
        void writeSettings(QDomDocument doc, QDomElement parentE);
        void readSettings(QDomDocument doc, QDomElement parentE);

    private:
        // Disable construction. Instances of this class can only be created
        // by AbstractComponent
        ComponentSettings(GCF::AbstractComponent* component=0);
        ~ComponentSettings();

        // Disable copy constructor (these are not implemented);
        ComponentSettings(const ComponentSettings& other);
        ComponentSettings& operator = (const ComponentSettings& other);

    private:
        friend class AbstractComponent;
        friend struct AbstractComponentData;
        ComponentSettingsData* d;
    };
}

#endif
