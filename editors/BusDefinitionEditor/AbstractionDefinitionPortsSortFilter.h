//-----------------------------------------------------------------------------
// File: AbstractionDefinitionPortsSortFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.11.2017
//
// Description:
// Sort filter proxy model for abstraction definition ports.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONDEFINITIONPORTSSORTFILTER_H
#define ABSTRACTIONDEFINITIONPORTSSORTFILTER_H

#include <QSortFilterProxyModel>
#include <QColor>

class PortAbstractionInterface;

//-----------------------------------------------------------------------------
//! Sort filter proxy model for abstraction definition ports.
//-----------------------------------------------------------------------------
class AbstractionDefinitionPortsSortFilter : public QSortFilterProxyModel
{

public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] columns         Storage for the column indexes.
     *      @param [in] portInterface   Interface for accessing port abstractions.
     *      @param [in] parent          Pointer to the owner of this model.
	 */
    AbstractionDefinitionPortsSortFilter(PortAbstractionInterface* portInterface,
        QObject *parent);

	/*!
	 *  The destructor.
	 */
    virtual ~AbstractionDefinitionPortsSortFilter() = default;

	/*!
	 *  Get the data for the specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
     *
     *      @return QVariant contains the data for the item.
	 */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected:

    /*!
     *  Get the background color for the selected index.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return Background color for the selected index.
     */
    virtual QColor getBackgroundColorForIndex(QModelIndex const& index) const;

    /*!
     *  Check if the filter should accept the selected row.
     *
     *      @param [in] source_row      Row of the selected item.
     *      @param [in] source_parent   Parent index of the item.
     *
     *      @return True, if the row is accepted, false otherwise.
     */
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    /*!
     *  Get the interface for accessing port abstractions.
     *
     *      @return Interface for accessing port abstractions.
     */
    PortAbstractionInterface* getPortInterface() const;

private:
	//! No copying. No assignment.
	AbstractionDefinitionPortsSortFilter(const AbstractionDefinitionPortsSortFilter& other);
	AbstractionDefinitionPortsSortFilter& operator=(const AbstractionDefinitionPortsSortFilter& other);

    /*!
     *  Check if the system group column is mandatory for the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return True, if the system group is mandatory, false otherwise.
     */
    bool isSystemGroupMandatory(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing port abstractions.
    PortAbstractionInterface* portInterface_;
};

#endif // ABSTRACTIONDEFINITIONPORTSSORTFILTER_H
