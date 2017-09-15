//-----------------------------------------------------------------------------
// File: PortMapTreeSortProxyModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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

    /*!
     *  Check if left index is less than right index..
     *
     *      @param [in] left    The selected left index.
     *      @param [in] right   The selected right index.
     *
     *      @return True, if the left index is less than the right index.
     */
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

signals:

    /*!
     *  Add a new port map at the selected position.
     *
     *      @param [in] position    The parent index of the new port map.
     */
    void addItem(QModelIndex const& position);

    /*!
     *  Remove port map at the selected position.
     *
     *      @param [in] position    The parent index of the removed port map.
     */
    void removeItem(QModelIndex const& position);
      
    /*!
     *  Remove all port maps from the selected logical port.
     *
     *      @param [in] position    The index of the selected logical port.
     */
    void removeAllChildItemsFromIndex(QModelIndex const& index);

public slots:
            
    void onAddItem(QModelIndex const& index);

    void onRemoveItem(QModelIndex const& index);

    void onRemoveAllChildItemsFrom(QModelIndex const&);

private:

    //! No copying.
    PortMapTreeSortProxyModel(const PortMapTreeSortProxyModel& other);

    //! No assignment.
    PortMapTreeSortProxyModel& operator=(const PortMapTreeSortProxyModel& other);  

    /*!
     *  Check if the selected index is valid.
     *
     *      @param [in] index   The selected index.
     *
     *      @return True, if the index is valid, false otherwise.
     */
    bool indexIsValid(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The bus interface containing the port maps.
    QSharedPointer<BusInterface> containingBus_;
};

#endif // PORTMAPTREESORTPROXYMODEL_H