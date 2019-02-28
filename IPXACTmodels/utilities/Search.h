////-----------------------------------------------------------------------------
// File: Search.hh
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 26.06.2018
//
// Description:
// Utilities for generic searching of items.
//-----------------------------------------------------------------------------

#ifndef SEARCH_H
#define SEARCH_H

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Utilities for generic searching of items.
//-----------------------------------------------------------------------------
namespace Search
{
    template <typename ContainerItem, template <typename> class Container>
    QSharedPointer<ContainerItem> findByName(QString const& name, 
        Container<QSharedPointer<ContainerItem> > container)
    {
        auto item = std::find_if(container.constBegin(), container.constEnd(),
            [name](QSharedPointer<ContainerItem> const& item) { return item->name().compare(name) == 0; });

        if (item == container.constEnd())
        {
            return QSharedPointer<ContainerItem>();
        }

        return *item;
    }
};

//-----------------------------------------------------------------------------

#endif // SEARCH_H
