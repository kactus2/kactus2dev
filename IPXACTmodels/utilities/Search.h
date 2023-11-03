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
    template <typename Container>
    QStringList getNames(QSharedPointer<Container> items)
    {
        if (items == nullptr)
        {
            return QStringList();
        }

        QStringList names;
        if (items.isNull() == false)
        {
            std::for_each(items->cbegin(), items->cend(), [&names](auto const& it) { names.append(it->name()); });
        }

        return names;
    };


    template <typename Container>
    QStringList getNames(Container const& items)
    {
        QStringList names;
        std::for_each(items.cbegin(), items.cend(), [&names](auto const& it) { names.append(it->name()); });
        return names;
    };

    template <typename ContainerItem, template <typename> class Container>
    QSharedPointer<ContainerItem> findByName(QStringView name,
        QSharedPointer<Container<QSharedPointer<ContainerItem> > > container)
    {
        if (container == nullptr)
        {
            return nullptr;
        }

        auto item = std::find_if(container->constBegin(), container->constEnd(),
            [&name](QSharedPointer<ContainerItem> const& item) { return item->name().compare(name) == 0; });

        if (item == container->constEnd())
        {
            return nullptr;
        }

        return *item;
    }

    template <typename ContainerItem, template <typename> class Container>
    QSharedPointer<ContainerItem> findByName(QStringView name,
        Container<QSharedPointer<ContainerItem> > container)
    {
        auto item = std::find_if(container.constBegin(), container.constEnd(),
            [&name](QSharedPointer<ContainerItem> const& item) { return item->name().compare(name) == 0; });

        if (item == container.constEnd())
        {
            return nullptr;
        }

        return *item;
    }
};

//-----------------------------------------------------------------------------

#endif // SEARCH_H
