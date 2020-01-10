////-----------------------------------------------------------------------------
// File: BusDefinitionUtils.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.03.2019
//
// Description:
// Utilities for Bus Definitions and abstraction definitions.
//-----------------------------------------------------------------------------

#include "BusDefinitionUtils.h"

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <library/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: BusDefinitionUtils::getSystemGroups()
//-----------------------------------------------------------------------------
IPXACTMODELS_EXPORT QStringList BusDefinitionUtils::getSystemGroups(QSharedPointer<const BusDefinition> bus,
    LibraryInterface* libraryAccess)
{
    QStringList systemGroups = bus->getSystemGroupNames();

    VLNV const extendVLNV = bus->getExtends();
    if (extendVLNV.isValid() && extendVLNV.getType() == VLNV::BUSDEFINITION)
    {
        QSharedPointer<const Document> extendDocument = libraryAccess->getModelReadOnly(extendVLNV);
        if (extendDocument)
        {
            QSharedPointer<const BusDefinition> extendBus = extendDocument.dynamicCast<const BusDefinition>();
            if (extendBus)
            {
                for (auto extendedSystem : getSystemGroups(extendBus, libraryAccess))
                {
                    if (!systemGroups.contains(extendedSystem))
                    {
                        systemGroups.append(extendedSystem);
                    }
                }
            }
        }
    }

    return systemGroups;
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionUtils::getExtendedLogicalSignals()
//-----------------------------------------------------------------------------
IPXACTMODELS_EXPORT QList<QSharedPointer<PortAbstraction> > BusDefinitionUtils::getExtendedLogicalSignals(
    QSharedPointer<const AbstractionDefinition> abstraction, LibraryInterface* libraryAccess)
{
    QList<QSharedPointer<PortAbstraction> > extendsignals = *abstraction->getLogicalPorts();

    VLNV const extendVLNV = abstraction->getExtends();
    if (extendVLNV.isValid() && extendVLNV.getType() == VLNV::BUSDEFINITION)
    {
        QSharedPointer<const Document> extendDocument = libraryAccess->getModelReadOnly(extendVLNV);
        if (extendDocument)
        {
            QSharedPointer<const AbstractionDefinition> extendAbstraction =
                extendDocument.dynamicCast<const AbstractionDefinition>();
            if (extendAbstraction)
            {
                extendsignals.append(getExtendedLogicalSignals(extendAbstraction, libraryAccess));
            }
        }
    }

    return extendsignals;
}
