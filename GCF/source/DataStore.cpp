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

#include <GCF/DataStore.h>
#include <QMap>

/**
\class GCF::DataStore GCF/DataStore
\brief Generic Data Store class.

The GCF::DataStore provides a generic data storage. Using this class one can store named
data values. The data values can be of any type as long as they can fit into a QVariant.
Using Qt's Q_DECLARE_METATYPE() we can now store almost any information in QVariant.
GCF::DataStore is a designed to be very light weight, hence it is not derived from any
other class.

\code
GCF::DataStore dataStore;

dataStore["name"] = "Prashanth";
dataStore["sirName"] = "Udupa";
dataStore["age"] = 26;
dataStore["imageLabel"] = qVariantFromValue<QLabel*>(label_widget);

// to use qVariantFromValue on QLabel*, QLabel* must have been declared
// as a meta type like this
// Q_DECLARE_METATYPE(QLabel*)
\endcode
*/

struct GCF::DataStoreData : public GCF::AbstractSharedData
{
    QMap<QString, QVariant> keyMap;
    QStringList keyList; // in the order in which it was inserted.
};

/**
Constructor.
These methods are implemented in GCF::DataStore to ensure that data stores are implicitly
copyable and shared. This means that multiple copies of GCF::DataStore share the same data.
*/
GCF::DataStore::DataStore()
{
    d = new GCF::DataStoreData;
}

/**
Copy Constructor.
These methods are implemented in GCF::DataStore to ensure that data stores are implicitly
copyable and shared. This means that multiple copies of GCF::DataStore share the same data.
*/
GCF::DataStore::DataStore(const GCF::DataStore & other)
{
    d = other.d;
    d->ref();
}

/**
Destructor.
*/
GCF::DataStore::~DataStore()
{
    d->deref();
}

/**
These methods are implemented in GCF::DataStore to ensure that data stores are implicitly
copyable and shared. This means that multiple copies of GCF::DataStore share the same data.
*/
bool GCF::DataStore::operator == (const GCF::DataStore & other) const
{
    return d == other.d;
}

/**
These methods are implemented in GCF::DataStore to ensure that data stores are implicitly
copyable and shared. This means that multiple copies of GCF::DataStore share the same data.
*/
GCF::DataStore & GCF::DataStore::operator = (const GCF::DataStore & other)
{
    d->deref();
    d = other.d;
    d->ref();
    return *this;
}

/**
These functions help store and retrieve information from the data store.
While the name operator helps query and set new keys, hasKey() and
removeKey() methods help detect the existence of a key and delete them.
*/
QVariant & GCF::DataStore::operator [] (const QString& key)
{
    if( !d->keyList.contains(key) )
        d->keyList.append(key);

    return d->keyMap[key];
}

/**
These functions help store and retrieve information from the data store.
While the name operator helps query and set new keys, hasKey() and
removeKey() methods help detect the existence of a key and delete them.
*/
const QVariant & GCF::DataStore::operator [] (const QString& key) const
{
    if( !d->keyList.contains(key) )
        d->keyList.append(key);

    return d->keyMap[key];
}

/**
These functions help store and retrieve information from the data store.
While the name operator helps query and set new keys, hasKey() and
removeKey() methods help detect the existence of a key and delete them.
*/
bool GCF::DataStore::hasKey(const QString& key) const
{
    return d->keyMap.contains(key);
}

/**
These functions help store and retrieve information from the data store.
While the name operator helps query and set new keys, hasKey() and
removeKey() methods help detect the existence of a key and delete them.
*/
void GCF::DataStore::removeKey(const QString& key)
{
    d->keyMap.remove(key);
    d->keyList.removeAll(key);
}

/**
These functions help store and retrieve information from the data store.
While the name operator helps query and set new keys, hasKey() and
removeKey() methods help detect the existence of a key and delete them.
*/
QStringList GCF::DataStore::keys() const
{
    // return d->keyMap.keys();
    return d->keyList;
}

/**
Clears this data store of all information and brings the data store to a
clean state.
*/
void GCF::DataStore::clear()
{
    d->keyMap.clear();
    d->keyList.clear();
}

/**
\defgroup MetaTypes
\brief Groups together all the datatypes declared as metatypes by this
library.

Metatypes declared in here help store instances of such types in QVariant.
For example you can store a pointer to IComponent in a QVariant, because
'IComponent*' has been declared as a metatype.

\code
// Native Pointer.
IComponent* comp = new IComponent_Impl;

// Store the pointer in a QVariant
QVariant compPtr = qVariantFromValue<IComponent*>(comp);

// Fetch the pointer from the QVariant.
comp = compPtr.value<IComponent*>();
\endcode

List of supported metatypes
\li IComponent*

*/

