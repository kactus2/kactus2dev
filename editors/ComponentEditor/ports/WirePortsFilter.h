//-----------------------------------------------------------------------------
// File: WirePortsFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.05.2019
//
// Description:
// Sorting proxy model for wire ports.
//-----------------------------------------------------------------------------

#ifndef WIREPORTSFILTER_H
#define WIREPORTSFILTER_H

#include <editors/ComponentEditor/ports/PortsFilter.h>

//-----------------------------------------------------------------------------
//! Sorting proxy model for wire ports.
//-----------------------------------------------------------------------------
class WirePortsFilter : public PortsFilter
{
    Q_OBJECT

public:

	 /*!
      *  Constructor.
      *
      *    @param [in] portsInterface     Interface for accessing the component ports.
      *    @param [in] parent             The parent object.
      */
    WirePortsFilter(QSharedPointer<PortsInterface> portInterface, QObject* parent = 0);

	 /*!
      *  Destructor.
      */
    virtual ~WirePortsFilter() = default;

    // No copying. No assignments.
    WirePortsFilter(WirePortsFilter const& rhs) = delete;
    WirePortsFilter& operator=(WirePortsFilter const& rhs) = delete;

private:

    /*!
     *  Get the index of the name column.
     *
     *    @return Index of the name column.
     */
    virtual int nameColumn() const override final;

    /*!
     *  Check if a port is filtered.
     *
     *    @param [in] portName    Name of the selected port.
     *
     *    @return True, if the port is accepted, false otherwise.
     */
    virtual bool portIsAccepted(QString const& portName) const override final;
};

#endif // WIREPORTSFILTER_H