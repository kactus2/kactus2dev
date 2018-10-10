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

    QSharedPointer<ComponentInstantiation> findComponentInstantiation(QSharedPointer<Component> component,
        QString const& viewName);
};

//-----------------------------------------------------------------------------

#endif // COMPONENT_SEARCH_H
