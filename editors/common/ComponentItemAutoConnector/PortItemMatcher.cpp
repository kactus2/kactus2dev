//-----------------------------------------------------------------------------
// File: PortItemMatcher.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.02.2019
//
// Description:
// Automatically defines possible matches between two ports.
//-----------------------------------------------------------------------------

#include "PortItemMatcher.h"

#include <editors/common/PortUtilities.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: PortItemMatcher::itemsCanBeConnected()
//-----------------------------------------------------------------------------
bool PortItemMatcher::itemsCanBeConnected(QString const& firstItem, QSharedPointer<Component> firstComponent,
    QString const& secondItem, QSharedPointer<Component> secondComponent) const
{
    QSharedPointer<Port> firstPort = firstComponent->getPort(firstItem);
    QSharedPointer<Port> secondPort = secondComponent->getPort(secondItem);
    if (firstPort && secondPort)
    {
        QVector<DirectionTypes::Direction> connectableDirections =
            PortUtilities::getConnectableDirections(firstPort->getDirection());

        return connectableDirections.contains(secondPort->getDirection());
    }

    return false;
}
