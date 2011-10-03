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

#include <GCF/ComponentSettings.h>

#include <QDataStream>
#include <QByteArray>
#include <QTextStream>
#include <GCF/Message>

namespace GCF
{
    struct ComponentSettingsData
    {
        ComponentSettingsData() : component(0) { }

        QMap<QString, GCF::DataStore> groupMap;
        GCF::AbstractComponent* component;
        GCF::DataStore defaultGroup;

        void writeStore(QDomDocument doc, QDomElement parentE, const GCF::DataStore& store);
        void readStore(QDomDocument doc, QDomElement parentE, GCF::DataStore& store);
    };
}

/**
\class GCF::ComponentSettings GCF/ComponentSettings
\brief Helps store and load component settings.

Each component has an instance of this class associated with it. A refernce to that instance can
be fetched by making use of the \ref GCF::AbstractComponent::settings() method.

The settings class can be thought of as a collection of QString to QVariant maps. Each map is called
as a group. You can add or remove groups by making use of the \ref addGroup(), \ref removeGroup()
methods.

The settings object contains a default group, whose reference can be fetched by the defaultGroup()
method.

Each group is basically a GCF::DataStore. <Key = Value> pairs can be added to stores in each group.
GCF::AbstractComponent takes care of loading and saving component settings into XML files. Component
settings are stored in a file that has the following name format

\code
{ComponentName}Settings.xml
\endcode

For example, the component settings for ProjectManager can be found in ProjectManagerSettings.xml. 

The settings XML file is read just before the component's \ref GCF::AbstractComponent::creationAnnounced()
method is called. The settings are written back to the disk just after the \ref GCF::AbstractComponent::finalizeComponent() 
method is called.

In order to make use of component settings, component developers can
\li Begin reading stored settings after the first call to \ref GCF::AbstractComponent::creationAnnounced().
\li Alter the settings during the life time of the component and make any final changes in 
\ref GCF::AbstractComponent::finalizeComponent().

Example: The following code is picked from ProjectManagerComponent.cpp of GCF.

\code
void GCF::Components::ProjectManagerComponent::finalizeComponent()
{
    if(!d->visProject.isRecentFileListModified())
        return;

    // Store the file names into the settings object
    QStringList rfList = d->visProject.recentFilesList();
    settings()["RecentFiles"] = rfList;
}

void GCF::Components::ProjectManagerComponent::creationAnnounced()
{
    // .......

    QStringList rfList = settings()["RecentFiles"].toStringList();
    for(int i=0; i<rfList.count(); i++)
        d->visProject.addRecentFile( rfList[i] );

    // ........
}
\endcode
*/

/**
\internal

Constructor. 
*/
GCF::ComponentSettings::ComponentSettings(GCF::AbstractComponent* component)
{
    d = new GCF::ComponentSettingsData;
    d->component = component;
}

/**
\internal

Destructor.
*/
GCF::ComponentSettings::~ComponentSettings()
{
    delete d;
}

/**
Adds a new group to the settings object. A group is a data store, which contains several
key=value pairs. 

\note If \c name already exists; then the store for it is replaced.
*/
void GCF::ComponentSettings::addGroup(const QString& name, const GCF::DataStore& store)
{
    if(d->groupMap.contains(name))
        return;

    d->groupMap[name] = store;
}

/**
Removes a group from the settings object.
*/
void GCF::ComponentSettings::removeGroup(const QString& name)
{
    d->groupMap.remove(name);
}

/**
Removes all groups and their data in the settings object. 

\note This method does not clear the data in the default group. To do that you must
make use of the \ref clear() method.
*/
void GCF::ComponentSettings::removeAllGroups()
{
    d->groupMap.clear();
}

/**
Returns a string list of all the group names.
*/
QStringList GCF::ComponentSettings::groupNames() const
{
    return d->groupMap.keys();
}

/**
Returns the data store associated with the group whose name is \c name. The second parameter if set to
true; causes the function to create a new group if \c name was not found.
*/
GCF::DataStore GCF::ComponentSettings::group(const QString& name, bool addIfNotFound) const
{
    static GCF::DataStore nullStore;
    nullStore.clear();

    if(d->groupMap.contains(name))
        return d->groupMap[name];

    if(addIfNotFound)
    {
        GCF::DataStore store;
        d->groupMap[name] = store;
        return store;
    }

    return nullStore;
}

/**
Returns the default group store.
*/
GCF::DataStore GCF::ComponentSettings::defaultGroup() const
{
    return d->defaultGroup;
}

/**
Operates on the default group store.
*/
QVariant & GCF::ComponentSettings::operator [] (const QString& key)
{
    return d->defaultGroup[key];
}

/**
Operates on the default group store.
*/
bool GCF::ComponentSettings::hasKey(const QString& key)
{
    return d->defaultGroup.hasKey(key);
}

/**
Operates on the default group store.
*/
void GCF::ComponentSettings::removeKey(const QString& key)
{
    d->defaultGroup.removeKey(key);
}

/**
Operates on the default group store.
*/
QStringList GCF::ComponentSettings::keys() const
{
    return d->defaultGroup.keys();
}

/**
Operates on the default group store.
*/
void GCF::ComponentSettings::clear()
{
    d->defaultGroup.clear();
}

/**
As good as calling clear() and removeAllGroups(). After this function returns
\ref isEmpty() returns true.
*/
void GCF::ComponentSettings::reset()
{
    d->groupMap.clear();
    d->defaultGroup.clear();
}

/**
Returns true of the settings object contains no data.
*/
bool GCF::ComponentSettings::isEmpty() const
{
    return d->defaultGroup.keys().isEmpty() && d->groupMap.isEmpty();
}

/**
\internal
*/
void GCF::ComponentSettings::writeSettings(QDomDocument doc, QDomElement parentE)
{
    // First write the default group
    QDomElement defGroup = doc.createElement("DefaultStore");
    d->writeStore(doc, defGroup, d->defaultGroup);
    parentE.appendChild(defGroup);

    // Now write other stores
    QMap<QString,GCF::DataStore>::const_iterator it = d->groupMap.begin();
    QMap<QString,GCF::DataStore>::const_iterator end = d->groupMap.end();
    while(it != end)
    {
        QDomElement groupE = doc.createElement("Group");
        groupE.setAttribute("Name", it.key());
        d->writeStore(doc, groupE, it.value());

        ++it;
    }
}

/**
\internal
*/
void GCF::ComponentSettings::readSettings(QDomDocument doc, QDomElement parentE)
{
    reset();

    // Read default group
    QDomElement defGroup = parentE.firstChildElement("DefaultStore");
    d->readStore(doc, defGroup, d->defaultGroup);

    // Read other groups.
    d->groupMap.clear();
    QDomElement groupE = parentE.firstChildElement("Group");
    while(!groupE.isNull())
    {
        GCF::DataStore store;
        d->readStore(doc, groupE, store);
        QString name = groupE.attribute("Name");
        d->groupMap[name] = store;

        groupE = groupE.nextSiblingElement("Group");
    }
}

/**
\internal
*/
void GCF::ComponentSettingsData::writeStore(QDomDocument doc, QDomElement parentE, const GCF::DataStore& store)
{
    QStringList keys = store.keys();

    for(int i=0; i<keys.count(); i++)
    {
        QString key = keys[i];
        QVariant value = store[key];
        QByteArray ba;
        QDataStream ds(&ba, QIODevice::WriteOnly);

        bool success = GCF::Message::encodeVariant(ds, value);
        if(!success)
            continue;

        QString valueStr = GCF::Message::byteArrayToString(ba);
        QDomElement keyE = doc.createElement("Entry");
        keyE.setAttribute("Key", key);
        keyE.appendChild( doc.createCDATASection(valueStr) );
        parentE.appendChild(keyE);
    }
}

/**
\internal
*/
void GCF::ComponentSettingsData::readStore(QDomDocument doc, QDomElement parentE, GCF::DataStore& store)
{
    QDomElement keyE = parentE.firstChildElement("Entry");
    while(!keyE.isNull())
    {
        QString key = keyE.attribute("Key");
        QString valueStr = keyE.firstChild().toText().data();
        QByteArray ba = GCF::Message::stringToByteArray(valueStr);
        QDataStream ds(&ba, QIODevice::ReadOnly);
        QVariant value;
        bool success = GCF::Message::extractVariant(ds, value);

        store[key] = value;

        keyE = keyE.nextSiblingElement("Entry");
    }
}



