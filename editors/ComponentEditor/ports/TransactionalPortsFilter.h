//-----------------------------------------------------------------------------
// File: TransactionalPortsFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.05.2019
//
// Description:
// Sorting proxy model for transactional ports.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONALPORTSFILTER_H
#define TRANSACTIONALPORTSFILTER_H

#include <editors/ComponentEditor/ports/PortsFilter.h>

//-----------------------------------------------------------------------------
//! Sorting proxy model for transactional ports.
//-----------------------------------------------------------------------------
class TransactionalPortsFilter : public PortsFilter
{
    Q_OBJECT

public:

	 /*!
      *  Constructor.
      *
      *      @param [in] parent     The parent object.
      */
    TransactionalPortsFilter(QObject* parent = 0);

	 /*!
      *  Destructor.
      */
    virtual ~TransactionalPortsFilter() = default;

    // No copying. No assignments.
    TransactionalPortsFilter(TransactionalPortsFilter const& rhs) = delete;
    TransactionalPortsFilter& operator=(TransactionalPortsFilter const& rhs) = delete;

private:

    /*!
     *  Check if a port is filtered.
     *
     *      @param [in] port    The selected port.
     *
     *      @return True, if the port is accepted, false otherwise.
     */
    virtual bool portIsAccepted(QSharedPointer<Port> port) const;
};

#endif // TRANSACTIONALPORTSFILTER_H