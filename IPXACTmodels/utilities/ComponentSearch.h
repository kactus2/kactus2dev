////-----------------------------------------------------------------------------
// File: ComponentSearch.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 26.06.2018
//
// Description:
// Utilities for searching items in Component.
//-----------------------------------------------------------------------------

#ifndef COMPONENT_SEARCH_H
#define COMPONENT_SEARCH_H

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Utilities for searching items in Component.
//-----------------------------------------------------------------------------
namespace ComponentSearch
{
    VLNV findDesignReference(QSharedPointer<Component> component, QString const& viewName);

    VLNV findDesignReference(QSharedPointer<Component> component, QSharedPointer<View> view);

    VLNV findDesignConfigurationReference(QSharedPointer<Component> component, QSharedPointer<View> view);

    QSharedPointer<View> findView(QSharedPointer<Component> component, QString const& viewName);

    template <typename ContainerItem, template <typename> class Container>
    QSharedPointer<ContainerItem> findByName(QString const& name, 
        Container<QSharedPointer<ContainerItem> > container)
    {
        for (auto item : container)
        {
            if (item->name().compare(name) == 0)
            {
                return item;
            }
        }

        return QSharedPointer<ContainerItem>();
    }
};

//-----------------------------------------------------------------------------

#endif // COMPONENT_SEARCH_H
