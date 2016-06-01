//-----------------------------------------------------------------------------
// File: PortMapTreeSortProxyModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.05.2016
//
// Description:
// Sorting proxy model for bus interface port maps.
//-----------------------------------------------------------------------------

#ifndef PORTMAPTREESORTPROXYMODEL_H
#define PORTMAPTREESORTPROXYMODEL_H

#include <QSortFilterProxyModel>

#include <QSharedPointer>

class BusInterface;

//-----------------------------------------------------------------------------
//! Sorting proxy model for bus interface port maps.
//-----------------------------------------------------------------------------
class PortMapTreeSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    
    /*!
     *  The constructor.
     *
     *      @param [in] busInterface    The bus interface containing the port maps.
     *      @param [in] parent          The owner of this sorter.
     */
    PortMapTreeSortProxyModel(QSharedPointer<BusInterface> busInterface, QObject *parent = 0);
    
    /*!
     *  The destructor.
     */
    ~PortMapTreeSortProxyModel();

protected:

    /*!
	 *  Implementation of the row filtering.
	 *
	 *      @param [in] source_row      The row to check for filtering.
     *      @param [in] source_parent   The parent index of the row.
     *
     *      @return True, if the row passes the filters, otherwise false.
	 */
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

private:

    //! No copying.
    PortMapTreeSortProxyModel(const PortMapTreeSortProxyModel& other);

    //! No assignment.
    PortMapTreeSortProxyModel& operator=(const PortMapTreeSortProxyModel& other);  

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The bus interface containing the port maps.
    QSharedPointer<BusInterface> containingBus_;
};

#endif // PORTMAPTREESORTPROXYMODEL_H