//-----------------------------------------------------------------------------
// File: BusInterfaceUtilities.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.02.2019
//
// Description:
// Utilities for bus interface mode compatibilities.
//-----------------------------------------------------------------------------

#include <KactusAPI/include/BusInterfaceUtilities.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <KactusAPI/include/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: BusInterfaceUtilities::hasMatchingBusDefinitions()
//-----------------------------------------------------------------------------
bool BusInterfaceUtilities::hasMatchingBusDefinitions(QSharedPointer<const BusDefinition> firstDefinition,
    VLNV const& secondDefinitionVLNV, LibraryInterface* library)
{
    if (firstDefinition)
    {
        QSharedPointer<const BusDefinition> comparisonDefinition =
            library->getModelReadOnly(secondDefinitionVLNV).dynamicCast<const BusDefinition>();
        if (comparisonDefinition)
        {
            if (firstDefinition == comparisonDefinition)
            {
                return true;
            }
            else
            {
                VLNV comparisonExtendVLNV = comparisonDefinition->getExtends();
                VLNV firstExtendVLNV = firstDefinition->getExtends();

                if ((comparisonExtendVLNV.isValid() &&
                    hasMatchingBusDefinitions(firstDefinition, comparisonExtendVLNV, library)) ||
                    (firstExtendVLNV.isValid() &&
                        hasMatchingBusDefinitions(comparisonDefinition, firstExtendVLNV, library)))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceUtilities::hasMatchingBusDefinitions()
//-----------------------------------------------------------------------------
bool BusInterfaceUtilities::busDefinitionVLNVsMatch(VLNV const& firstDefinitionVLNV,
    VLNV const& secondDefinitionVLNV, LibraryInterface* library)
{
    QSharedPointer<const BusDefinition> firstDefinition =
        library->getModelReadOnly(firstDefinitionVLNV).dynamicCast<const BusDefinition>();
    if (firstDefinition)
    {
        return BusInterfaceUtilities::hasMatchingBusDefinitions(firstDefinition, secondDefinitionVLNV, library);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceUtilities::getIconPath()
//-----------------------------------------------------------------------------
QString BusInterfaceUtilities::getIconPath(General::InterfaceMode busMode)
{
    QString iconPath("");
    if (busMode == General::MASTER || busMode == General::INITIATOR)
    {
        iconPath = ":icons/common/graphics/busInterfaceMaster.png";
    }
    else if (busMode == General::MIRRORED_MASTER || busMode == General::MIRRORED_INITIATOR)
    {
        iconPath = ":icons/common/graphics/busInterfaceMirroredMaster.png";
    }
    else if (busMode == General::SLAVE || busMode == General::TARGET)
    {
        iconPath = ":icons/common/graphics/busInterfaceSlave.png";
    }
    else if (busMode == General::MIRRORED_SLAVE || busMode == General::MIRRORED_TARGET)
    {
        iconPath = ":icons/common/graphics/busInterfaceMirroredSlave.png";
    }
    else if (busMode == General::SYSTEM)
    {
        iconPath = ":icons/common/graphics/busInterfaceSystem.png";
    }
    else if (busMode == General::MIRRORED_SYSTEM)
    {
        iconPath = ":icons/common/graphics/busInterfaceMirroredSystem.png";
    }
    else if (busMode == General::MONITOR)
    {
        iconPath = ":icons/common/graphics/busInterfaceMonitor.png";
    }

    return iconPath;
}
