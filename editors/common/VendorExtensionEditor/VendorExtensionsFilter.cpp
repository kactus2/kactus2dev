//-----------------------------------------------------------------------------
// File: VendorExtensionsFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.03.2019
//
// Description:
// Sorting proxy model for vendor extensions editor.
//-----------------------------------------------------------------------------

#include "VendorExtensionsFilter.h"

#include <editors/common/VendorExtensionEditor/VendorExtensionColumns.h>

#include <IPXACTmodels/common/VendorExtension.h>

//-----------------------------------------------------------------------------
// Function: VendorExtensionsFilter::VendorExtensionsFilter()
//-----------------------------------------------------------------------------
VendorExtensionsFilter::VendorExtensionsFilter(QObject* parent) :
EditableTreeSortFilter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool VendorExtensionsFilter::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    QModelIndex nameIndex = sourceModel()->index(source_row, VendorExtensionColumns::NAME, source_parent);
    QModelIndex nameSpaceIndex =
        sourceModel()->index(source_row, VendorExtensionColumns::NAMESPACE, source_parent);

    QString itemName = sourceModel()->data(nameIndex).toString();
    QString itemSpace = sourceModel()->data(nameSpaceIndex).toString();

    if (itemName.contains("Description", Qt::CaseInsensitive) ||
        itemSpace.startsWith("kactus2", Qt::CaseInsensitive))
    {
        return false;
    }
    else
    {
        return EditableTreeSortFilter::filterAcceptsRow(source_row, source_parent);
    }
}
