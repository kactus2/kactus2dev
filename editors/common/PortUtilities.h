//-----------------------------------------------------------------------------
// File: PortUtilities.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.02.2019
//
// Description:
// Utilities for port compatibilities.
//-----------------------------------------------------------------------------

#ifndef PORTUTILITIES_H
#define PORTUTILITIES_H

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QVector>

//-----------------------------------------------------------------------------
//! Utilities for port compatibilities.
//-----------------------------------------------------------------------------
namespace PortUtilities
{
    /*!
     *  Get the list of port directions connectible to the selected port direction.
     *
     *      @param [in] portDirection   The selected port direction.
     *
     *      @return List of connectible port directions.
     */
    QVector<DirectionTypes::Direction> getConnectableDirections(DirectionTypes::Direction portDirection);

    /*!
     *  Get the list of hierarchical port directions connectible to the selected port direction.
     *
     *      @param [in] portDirection   The selected port direction.
     *
     *      @return List of hierarchical connectible port directions.
     */
    QVector<DirectionTypes::Direction> getConnectableHierarchicalDirections(
        DirectionTypes::Direction portDirection);

    /*!
     *  Get the path for the icon matching the selected port direction.
     *
     *      @param [in] portDirection   The selected port direction.
     *
     *      @return Path containing the icon for the selected port direction.
     */
    QString getDirectionIconPath(DirectionTypes::Direction portDirection);
}

//-----------------------------------------------------------------------------

#endif // PORTUTILITIES_H
