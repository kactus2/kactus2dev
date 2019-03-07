//-----------------------------------------------------------------------------
// File: AutoConnectorListFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 19.02.2019
//
// Description:
// Sorting proxy model for auto connector lists.
//-----------------------------------------------------------------------------

#ifndef AUTOCONNECTORLISTFILTER_H
#define AUTOCONNECTORLISTFILTER_H

#include <QSortFilterProxyModel>
#include <QTableWidget>
#include <QCheckBox>

//-----------------------------------------------------------------------------
//! Sorting proxy model for auto connector lists.
//-----------------------------------------------------------------------------
class AutoConnectorListFilter : public QSortFilterProxyModel
{
    Q_OBJECT

public:

	 /*!
      *  Constructor.
      *
      *      @param [in] itemTable       Table containing the connected items.
      *      @param [in] tableColumn     Column in the connection table.
      *      @param [in] parent          The parent object.
      */
    AutoConnectorListFilter(QTableWidget* itemTable, int const& tableColumn, QCheckBox* hideConnectedBox,
        QObject* parent = 0);

	 /*!
      *  Destructor.
      */
    virtual ~AutoConnectorListFilter() = default;

    // No copying. No assignments.
    AutoConnectorListFilter(AutoConnectorListFilter const& rhs) = delete;
    AutoConnectorListFilter& operator=(AutoConnectorListFilter const& rhs) = delete;

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

private:

    /*!
     *  Check if the selected item exists in the connection table.
     *
     *      @param [in] itemName    Name of the selected item.
     *
     *      @return True, if the item has been connected, false otherwise.
     */
    bool itemIsConnected(QString const& itemName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Column in the connection table.
    int tableColumn_;

    //! Table containing the connected items.
    QTableWidget* itemTable_;

    //! Informs whether the connected items should be hidden.
    QCheckBox* hideConnectedBox_;
};

#endif // AUTOCONNECTORLISTFILTER_H