//-----------------------------------------------------------------------------
// File: Copy.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 24.10.2023
//
// Description:
// Utilities for copying objects.
//-----------------------------------------------------------------------------

#ifndef COPY_H
#define COPY_H

#include <QSharedPointer>
#include <QList>

//-----------------------------------------------------------------------------
//! Utilities for copying objects.
//-----------------------------------------------------------------------------
namespace Copy
{

    template <typename Item>
    void copyList(QSharedPointer<QList<QSharedPointer<Item> > > from,
        QSharedPointer<QList<QSharedPointer<Item> > > to)
    {
        if (from.isNull() || to.isNull())
        {
            return;
        }

        std::for_each(from->cbegin(), from->cend(), [&to](auto const& item) 
            { to->append(QSharedPointer<Item>(new Item(*item))); });

    };

};

//-----------------------------------------------------------------------------

#endif // COPY_H
