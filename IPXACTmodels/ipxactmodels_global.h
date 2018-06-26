#ifndef IPXACTMODELS_GLOBAL_H
#define IPXACTMODELS_GLOBAL_H

#include <QSharedPointer>
#include <QList>

#ifdef IPXACTMODELS_LIB
# define IPXACTMODELS_EXPORT Q_DECL_EXPORT
#else
# define IPXACTMODELS_EXPORT Q_DECL_IMPORT
#endif

namespace Utilities
{
    /*!
     *  Copy the items from a list to another list.
     *
     *      @param [in] listTo      List to copy the items into.
     *      @param [in] listFrom    List to copy the items from.
     */
    template <typename ContainerItem, template <typename> class Container>
    void copyList(QSharedPointer<Container<QSharedPointer<ContainerItem> > > listTo,
        const QSharedPointer<Container<QSharedPointer<ContainerItem> > > listFrom)
    {
        listTo->clear();

        for (auto item = listFrom->cbegin(); item != listFrom->cend(); ++item)
        {
            listTo->append(QSharedPointer<ContainerItem>(new ContainerItem(**item)));
        }
    };
};

#endif // IPXACTMODELS_GLOBAL_H
