//-----------------------------------------------------------------------------
// File: Structural.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.11.2023
//
// Description:
// Implementation of ipxact:structural in component port.
//-----------------------------------------------------------------------------


#ifndef STRUCTURAL_H
#define STRUCTURAL_H

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/Vector.h>

#include <IPXACTmodels/ipxactmodels_global.h>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:structural in component port..
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Structural
{
public:

    enum class Type
    {
        Struct,
        Union,
        Interface,
        Undefined
    };


    constexpr void setType(Type type) noexcept;

    [[nodiscard]] constexpr Type getType() const noexcept;

    /*!
     *  Set the direction of this port.
     *
     *      @param [in] direction   The new direction.
     */
    constexpr void setDirection(DirectionTypes::Direction direction) noexcept;

    /*!
     *  Get the direction of the port.
     *
     *      @return Enum direction specifying the port direction.
     */
    [[nodiscard]] constexpr DirectionTypes::Direction getDirection() const noexcept;


    [[nodiscard]] QSharedPointer<QList<Vector> > getVectors() const;


    [[nodiscard]] static QString toString(Type type);

private:

    //! The type of this port.
    Type portType_{ Type::Undefined };

    //! The direction of the port.
    DirectionTypes::Direction direction_{ DirectionTypes::DIRECTION_INVALID };

    QSharedPointer<QList<Vector> > vectors_{ new QList<Vector>() };
};

#endif // STRUCTURAL_H