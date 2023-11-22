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
     *  Description
     *
     *      @param [in] packed
     *
     *      @return
     */
    constexpr void setPacked(bool packed) noexcept { packed_ = packed; }

    /*!
     *  Description
     *
     *
     *      @return 
     */
    [[nodiscard]] bool isPacked() const noexcept { return packed_; }

    /*!
     *  Description
     *
     *      @param [in] type
     *
     *      @return 
     */
    constexpr void setType(Type type) noexcept { portType_ = type; }

    /*!
     *  Description
     *
     *
     *      @return 
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

    [[nodiscard]] QSharedPointer<QList<Vector> > getVectors() const { return vectors_; }


    [[nodiscard]] SubPort::List getSubPorts() const { return subPorts_; }


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