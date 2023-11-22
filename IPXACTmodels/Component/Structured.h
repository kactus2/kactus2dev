//-----------------------------------------------------------------------------
// File: Structured.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.11.2023
//
// Description:
// Implementation of ipxact:structured in component port.
//-----------------------------------------------------------------------------

#ifndef STRUCTURED_H
#define STRUCTURED_H

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/Vector.h>

#include "SubPort.h"

#include <IPXACTmodels/ipxactmodels_global.h>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:structured in component port.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Structured
{
public:

    enum class Type
    {
        Struct,
        Union,
        Interface,
        Undefined
    };

    /*!
     *  Set the structured port as packed.
     *
     *      @param [in] packed Packed or not.
     */
    constexpr void setPacked(bool packed) noexcept { packed_ = packed; }

    /*!
     *  Check if the structured port can be packed.
     *
     *      @return True, if the port can be packed, otherwise false.
     */
    [[nodiscard]] bool isPacked() const noexcept { return packed_; }

    /*!
     *  Set the port type.
     *
     *      @param [in] type The type to set.
     */
    constexpr void setType(Type type) noexcept { portType_ = type; }

    /*!
     *  Get the port type.
     *
     *      @return The port type.
     */
    [[nodiscard]] constexpr Type getType() const noexcept { return portType_; }

    /*!
     *  Set the direction of this port.
     *
     *      @param [in] direction   The new direction.
     */
    constexpr void setDirection(DirectionTypes::Direction direction) noexcept { direction_ = direction; }

    /*!
     *  Get the direction of the port.
     *
     *      @return Enum direction specifying the port direction.
     */
    [[nodiscard]] constexpr DirectionTypes::Direction getDirection() const noexcept { return direction_; }

    /*!
     *  Get the vectors in the structured port.
     *
     *      @return The vectors in the port.
     */
    [[nodiscard]] QSharedPointer<QList<Vector> > getVectors() const { return vectors_; }

    /*!
     *  Get the sub-ports in the structured port.
     *
     *      @return The sub-ports in the port.
     */
    [[nodiscard]] SubPort::List getSubPorts() const { return subPorts_; }

    /*!
     *  Convert the structured type to string.
     *
     *      @return The string representation of the type.
     */
    [[nodiscard]] static QString toString(Type type);

private:

    //! Port is mappable to packed bit sequence.
    bool packed_{ true };

    //! The type of this port.
    Type portType_{ Type::Undefined };

    //! The direction of the port.
    DirectionTypes::Direction direction_{ DirectionTypes::DIRECTION_INVALID };

    //! The vectors of the port.
    QSharedPointer<QList<Vector> > vectors_{ new QList<Vector>() };

    //! The sub-ports of the structural port.
    SubPort::List subPorts_{ new QList<QSharedPointer<SubPort> >() };
};

#endif // STRUCTURED_H