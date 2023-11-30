//-----------------------------------------------------------------------------
// File: PortSummaryFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.11.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef PORT_SUMMARY_FILTER_H
#define PORT_SUMMARY_FILTER_H

#include "PortSummaryColumns.h"
#include "PortsFilter.h"

//-----------------------------------------------------------------------------
//! Sorting proxy model for wire ports.
//-----------------------------------------------------------------------------
class PortSummaryFilter : public PortsFilter
{
    Q_OBJECT

public:

	 /*!
      *  Constructor.
      *
      *      @param [in] portsInterface     Interface for accessing the component ports.
      *      @param [in] parent             The parent object.
      */
    using PortsFilter::PortsFilter;

	 /*!
      *  Destructor.
      */
    ~PortSummaryFilter() = default;

private:

    /*!
     *  Get the index of the name column.
     *
     *      @return Index of the name column.
     */
    int nameColumn() const final 
    {
        return PortSummaryColumns::NAME;
    }

    /*!
     *  Check if a port is filtered.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the port is accepted, false otherwise.
     */
    bool portIsAccepted(QString const& /*portName*/) const final
    {
        return true;
    }

};

#endif // PORT_SUMMARY_FILTER_H