//-----------------------------------------------------------------------------
// File: CatalogFileFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 10.03.2015
//
// Description:
// Class for filtering catalog files model.
//-----------------------------------------------------------------------------

#include "CatalogFileFilter.h"
#include "CatalogFileColumns.h"

//-----------------------------------------------------------------------------
// Function: CatalogFileFilter::CatalogFileFilter()
//-----------------------------------------------------------------------------
CatalogFileFilter::CatalogFileFilter(Document::Revision docRevision, QObject *parent): 
    QSortFilterProxyModel(parent),
    docRevision_(docRevision)
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

//-----------------------------------------------------------------------------
// Function: CatalogFileFilter::onOpenItem()
//-----------------------------------------------------------------------------
void CatalogFileFilter::onOpenItem(QModelIndex const& index)
{
    emit openItem(mapToSource(index));
}

//-----------------------------------------------------------------------------
// Function: CatalogFileFilter::filterAcceptsColumn()
//-----------------------------------------------------------------------------
bool CatalogFileFilter::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (source_row == CatalogFileColumns::TYPE_DEFINITIONS && docRevision_ != Document::Revision::Std22)
    {
        return false;
    }

    return true;
}