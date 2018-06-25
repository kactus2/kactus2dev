//-----------------------------------------------------------------------------
// File: AbstractionTransactionalPortsSortFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.06.2018
//
// Description:
// Sort filter proxy model for abstraction transactional ports.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONTRANSACTIONALPORTSSORTFILTER_H
#define ABSTRACTIONTRANSACTIONALPORTSSORTFILTER_H

#include <editors/BusDefinitionEditor/AbstractionDefinitionPortsSortFilter.h>

#include <QSortFilterProxyModel>
#include <QColor>

//-----------------------------------------------------------------------------
//! Sort filter proxy model for abstraction definition ports.
//-----------------------------------------------------------------------------
class AbstractionTransactionalPortsSortFilter : public AbstractionDefinitionPortsSortFilter
{

public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] columns     Storage for the column indexes.
     *      @param [in] parent      Pointer to the owner of this model.
	 */
    AbstractionTransactionalPortsSortFilter(AbstractionDefinitionPortsSortFilter::ColumnHandles columns,
        QObject *parent);
	
	/*!
	 *  The destructor.
	 */
    virtual ~AbstractionTransactionalPortsSortFilter() = default;

protected:

    /*!
     *  Get the background color for the selected index.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return Background color for the selected index.
     */
    virtual QColor getBackgroundColorForIndex(QModelIndex const& index) const;

private:
	//! No copying. No assignment.
    AbstractionTransactionalPortsSortFilter(const AbstractionTransactionalPortsSortFilter& other);
	AbstractionTransactionalPortsSortFilter& operator=(const AbstractionTransactionalPortsSortFilter& other);

    /*!
     *  Check if the indexed row contains a protocol payload.
     *
     *      @param [in] index   Index of the selected row.
     *
     *      @return True, if the indexed row contains a protocol payload, false otherwise.
     */
    bool indexedRowContainsPayload(QModelIndex const& index) const;
};

#endif // ABSTRACTIONTRANSACTIONALPORTSSORTFILTER_H
