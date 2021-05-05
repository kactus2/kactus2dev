//-----------------------------------------------------------------------------
// File: ConnectivityGraphUtilities.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.04.2021
//
// Description:
// Utility functions for connectivity graphs.
//-----------------------------------------------------------------------------

#include "ConnectivityGraphUtilities.h"

#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::getInterfacedComponent()
//-----------------------------------------------------------------------------
QSharedPointer<const Component> ConnectivityGraphUtilities::getInterfacedComponent(LibraryInterface* library,
    QSharedPointer<const ConnectivityComponent> masterComponent)
{
    QSharedPointer<const Component> interfaceComponent;

    if (masterComponent)
    {
        QStringList vlnvList = masterComponent->getVlnv().split(":");
        if (vlnvList.size() == 4)
        {
            VLNV componentVLNV(VLNV::COMPONENT, vlnvList.at(0), vlnvList.at(1), vlnvList.at(2), vlnvList.at(3));
            QSharedPointer<const Document> vlnvDocument = library->getModelReadOnly(componentVLNV);
            if (vlnvDocument)
            {
                interfaceComponent = vlnvDocument.dynamicCast<const Component>();
            }
        }
    }

    return interfaceComponent;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::getInterfacedCPU()
//-----------------------------------------------------------------------------
QSharedPointer<Cpu> ConnectivityGraphUtilities::getInterfacedCPU(QSharedPointer<const Component> containingComponent,
    QSharedPointer<const ConnectivityInterface> routeInterface)
{
    QSharedPointer<MemoryItem> interfacedMemory = routeInterface->getConnectedMemory();
    if (interfacedMemory && interfacedMemory->getType() == MemoryDesignerConstants::ADDRESSSPACE_TYPE)
    {
        for (auto componentCPU : *containingComponent->getCpus())
        {
            if (componentCPU->getAddressSpaceRefs().contains(interfacedMemory->getName()))
            {
                return componentCPU;
            }
        }
    }

    return QSharedPointer<Cpu>();
}
