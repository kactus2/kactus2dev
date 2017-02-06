//-----------------------------------------------------------------------------
// File: CatalogFileFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.03.2015
//
// Description:
// Class for filtering catalog files model.
//-----------------------------------------------------------------------------

#include "CatalogFileFilter.h"

//-----------------------------------------------------------------------------
// Function: CatalogFileFilter::CatalogFileFilter()
//-----------------------------------------------------------------------------
CatalogFileFilter::CatalogFileFilter(QObject *parent): QSortFilterProxyModel(parent)
{
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

//-----------------------------------------------------------------------------
// Function: CatalogFileFilter::~CatalogFileFilter()
//-----------------------------------------------------------------------------
CatalogFileFilter::~CatalogFileFilter()
{
}

//-----------------------------------------------------------------------------
// Function: CatalogFileFilter::onAddItem()
//-----------------------------------------------------------------------------
void CatalogFileFilter::onAddItem(QModelIndex const& index)
{
    emit addItem(mapToSource(index));
}

//-----------------------------------------------------------------------------
// Function: CatalogFileFilter::onRemoveItem()
//-----------------------------------------------------------------------------
void CatalogFileFilter::onRemoveItem(QModelIndex const& index)
{
    emit removeItem(mapToSource(index));
}
