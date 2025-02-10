//-----------------------------------------------------------------------------
// File: ListHelper.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 1.11.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef LIST_HELPER_H 
#define LIST_HELPER_H 

#include <vector>
#include <string>

namespace ListHelper
{
    template <typename T>
    std::vector<std::string> listNames(QSharedPointer<QList<QSharedPointer<T> > > container)
    {
        std::vector<std::string> names;
        std::for_each(container->cbegin(), container->cend(), [&names](auto const& item)
            {
                names.push_back(item->name().toStdString());
            });
        return names;
    }

    template <typename T>
    int itemIndex(std::string const& itemName, QSharedPointer<QList<QSharedPointer<T> > > container)
    {
        if (container == nullptr)
        {
            return -1;
        }

        const QString name = QString::fromStdString(itemName);

        const auto pos = std::find_if(container->cbegin(), container->cend(), [&name](auto const& item)
            {
                return item->name().compare(name) == 0;
            });

        if (pos == container->cend())
        {
            return -1;
        }

        return std::distance(container->cbegin(), pos);
    }
};

#endif // LIST_HELPER_H 