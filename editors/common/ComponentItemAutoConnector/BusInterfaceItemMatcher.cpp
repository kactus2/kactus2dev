//-----------------------------------------------------------------------------
// File: BusInterfaceItemMatcher.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.02.2019
//
// Description:
// Automatically defines possible matches between two bus interfaces.
//-----------------------------------------------------------------------------

#include "BusInterfaceItemMatcher.h"

#include <editors/common/BusInterfaceUtilities.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

//-----------------------------------------------------------------------------
// Function: BusInterfaceItemMatcher::BusInterfaceItemMatcher()
//-----------------------------------------------------------------------------
BusInterfaceItemMatcher::BusInterfaceItemMatcher(LibraryInterface* library):
TableItemMatcher(),
library_(library)
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItemMatcher::itemsCanBeConnected()
//-----------------------------------------------------------------------------
bool BusInterfaceItemMatcher::itemsCanBeConnected(QString const& firstItem,
    QSharedPointer<Component> firstComponent, QString const& secondItem,
    QSharedPointer<Component> secondComponent) const
{
    QSharedPointer<BusInterface> firstBus = firstComponent->getBusInterface(firstItem);
    QSharedPointer<BusInterface> secondBus = secondComponent->getBusInterface(secondItem);
    if (firstBus && secondBus)
    {
        ConfigurableVLNVReference firstBusDefinitionVLNV = firstBus->getBusType();
        if (firstBusDefinitionVLNV.isValid())
        {
            QSharedPointer<const BusDefinition> firstBusDefinition =
                library_->getModelReadOnly(firstBusDefinitionVLNV).dynamicCast<const BusDefinition>();
            if (firstBusDefinition)
            {
                General::InterfaceMode busMode = firstBus->getInterfaceMode();
                General::InterfaceMode secondBusMode = secondBus->getInterfaceMode();
                if (busMode != General::MONITOR && secondBusMode != General::MONITOR)
                {
                    QVector<General::InterfaceMode> compatibleModes = getCompatibleModes(busMode);

                    return compatibleModes.contains(secondBusMode) &&
                        BusInterfaceUtilities::hasMatchingBusDefinitions(
                            firstBusDefinition, secondBus->getBusType(), library_);
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItemMatcher::getCompatibleModes()
//-----------------------------------------------------------------------------
QVector<General::InterfaceMode> BusInterfaceItemMatcher::getCompatibleModes(General::InterfaceMode busMode) const
{
    return General::getCompatibleInterfaceModesForActiveInterface(busMode);
}
