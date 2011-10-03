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

#ifndef DATA_STORE_H
#define DATA_STORE_H

#include "Common.h"
#include <QVariant>
#include <QStringList>

namespace GCF
{
    struct DataStoreData;
    class GCF_EXPORT_CLASS DataStore
    {
    public:
        DataStore();
        DataStore(const DataStore & other);
        ~DataStore();

        bool operator == (const DataStore & other) const;
        DataStore & operator = (const DataStore & other);

        QVariant & operator [] (const QString& key);
        const QVariant & operator [] (const QString& key) const;
        bool hasKey(const QString& key) const;
        void removeKey(const QString& key);
        QStringList keys() const;
        void clear();

    private:
        DataStoreData* d;
    };
}

Q_DECLARE_METATYPE(GCF::DataStore)

#endif
