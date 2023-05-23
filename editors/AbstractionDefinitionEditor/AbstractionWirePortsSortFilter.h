//-----------------------------------------------------------------------------
// File: AbstractionWirePortsSortFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 27.11.2019
//
// Description:
// Sort filter proxy model for abstraction definition wire ports.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONWIREPORTSSORTFILTER_H
#define ABSTRACTIONWIREPORTSSORTFILTER_H

#include "AbstractionDefinitionPortsSortFilter.h"

//-----------------------------------------------------------------------------
//! Sort filter proxy model for abstraction definition wire ports.
//-----------------------------------------------------------------------------
class AbstractionWirePortsSortFilter : public AbstractionDefinitionPortsSortFilter
{

public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] parent      Pointer to the owner of this model.
	 */
    AbstractionWirePortsSortFilter(PortAbstractionInterface* portInterface, QObject *parent);

	/*!
	 *  The destructor.
	 */
    virtual ~AbstractionWirePortsSortFilter() = default;

    //! No copying. No assignment.
    AbstractionWirePortsSortFilter(const AbstractionWirePortsSortFilter& other) = delete;
    AbstractionWirePortsSortFilter& operator=(const AbstractionWirePortsSortFilter& other) = delete;

protected:

    /*!
     *  Check if the filter accepts the selected row.
     *
     *      @param [in] source_row      The selected row.
     *      @param [in] source_parent   Parent index of the selected row.
     *
     *      @return True, if the row is accepted, false otherwise.
     */
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override final;

};

#endif // ABSTRACTIONWIREPORTSSORTFILTER_H
