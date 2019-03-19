//-----------------------------------------------------------------------------
// File: VendorExtensionsFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.03.2019
//
// Description:
// Sorting proxy model for vendor extensions editor.
//-----------------------------------------------------------------------------

#ifndef VENDOREXTENSIONSFILTER_H
#define VENDOREXTENSIONSFILTER_H

#include <common/views/EditableTreeView/EditableTreeSortFilter.h>

#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
//! Sorting proxy model for vendor extensions editor.
//-----------------------------------------------------------------------------
class VendorExtensionsFilter : public EditableTreeSortFilter
{
    Q_OBJECT

public:

	 /*!
      *  Constructor.
      *
      *      @param [in] parent          The parent object.
      */
    VendorExtensionsFilter(QObject* parent = 0);

	 /*!
      *  Destructor.
      */
    virtual ~VendorExtensionsFilter() = default;

    // No copying. No assignments.
    VendorExtensionsFilter(VendorExtensionsFilter const& rhs) = delete;
    VendorExtensionsFilter& operator=(VendorExtensionsFilter const& rhs) = delete;

protected:

    /*!
	 *  Implementation of the row filtering.
	 *
	 *      @param [in] source_row      The row to check for filtering.
     *      @param [in] source_parent   The parent index of the row.
     *
     *      @return True, if the row passes the filters, otherwise false.
	 */
    virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;
};

#endif // VENDOREXTENSIONSFILTER_H