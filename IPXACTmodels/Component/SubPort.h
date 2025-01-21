//-----------------------------------------------------------------------------
// File: SubPort.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 21.11.2023
//
// Description:
// Implementation of ipxact:subPort in component port.
//-----------------------------------------------------------------------------

#ifndef SUBPORT_H
#define SUBPORT_H

#include <IPXACTmodels/common/Array.h>
#include <IPXACTmodels/common/NameGroup.h>

#include <IPXACTmodels/ipxactmodels_global.h>

class Structured;
class Wire;

//-----------------------------------------------------------------------------
//! Implementation of ipxact:SubPort in component port.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SubPort : public NameGroup
{
public:

    using List = QSharedPointer<QList<QSharedPointer<SubPort> > >;

    /*!
     *  Set for input, inout or output ports in an interface.
     *
     *    @param [in] isIO Wheter the port is an input, inout or output.
     */
    void setIsIO(bool isIO) noexcept { isIO_ = isIO; }

    /*!
     *  Check if the port is an input, output or inout.
     *
     *    @return True, if the port is input, output or inout, otherwise false.
     */
    [[nodiscard]] bool isIO() const noexcept { return isIO_; }

    /*!
     *  Check if the sub-port has a wire definition.
     *
     *    @return True, if the sub-port has a wire, otherwise false.
     */
    [[nodiscard]] bool isWire() const noexcept { return wire_.isNull() == false; }

    /*!
     *  Set the wire port in the sub-port.
     *
     *    @param [in] wire The wire to set.
     */
    void setWire(QSharedPointer<Wire> wire);

    /*!
     *  Get the wire port in the sub-port.
     *
     *    @return The wire port, if available, otherwise nullptr.
     */
    [[nodiscard]] QSharedPointer<Wire> getWire() const  { return wire_; }

    /*!
     *  Check if the sub-port has a structured definition.
     *
     *    @return True, if the sub-port has a structured, otherwise false.
     */
    [[nodiscard]] bool isStructured() const noexcept { return structured_.isNull() == false; }

    /*!
     *  Set the structured port in the sub-port.
     *
     *    @param [in] structured  The structured port to set.
     */
    void setStructured(QSharedPointer<Structured> structured);

    /*!
     *  Get the structured port in the sub-port.
     *
     *    @return The structured port, if available, otherwise nullptr.
     */
    [[nodiscard]] QSharedPointer<Structured> getStructured() const { return structured_; }

    [[nodiscard]] QSharedPointer<QList<Array> > getArrays() const { return arrays_; }

private:

    //! The sub-port wire port.
    QSharedPointer<Wire> wire_{ nullptr };

    //! The nested structured port.
    QSharedPointer<Structured> structured_{ nullptr };

    //! The sub-port arrays.
    QSharedPointer<QList<Array> > arrays_{ new QList<Array>() };

    //! Is the sub-port input, output or inout in an interface.
    bool isIO_;
};

#endif // SUBPORT_H