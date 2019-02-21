//-----------------------------------------------------------------------------
// File: BusInterfaceListFiller.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 13.02.2019
//
// Description:
// Fills a list with bus interfaces.
//-----------------------------------------------------------------------------

#include "BusInterfaceListFiller.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>

//-----------------------------------------------------------------------------
// Function: BusInterfaceListFiller::initializeList()
//-----------------------------------------------------------------------------
void BusInterfaceListFiller::initializeList(QStandardItemModel* selectedList,
    QSharedPointer<Component> containingComponent) const
{
    for (auto busInterface : *containingComponent->getBusInterfaces())
    {
        General::InterfaceMode busMode = busInterface->getInterfaceMode();
        if (busMode != General::MONITOR)
        {
            QIcon modeIcon(getIconPath(busMode));
            selectedList->appendRow(new QStandardItem(modeIcon, busInterface->name()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceListFiller::getIconPath()
//-----------------------------------------------------------------------------
QString BusInterfaceListFiller::getIconPath(General::InterfaceMode interfaceMode) const
{
    QString iconPath("");
    if (interfaceMode == General::MASTER)
    {
        iconPath = ":icons/common/graphics/busInterfaceMaster.png";
    }
    else if (interfaceMode == General::MIRROREDMASTER)
    {
        iconPath = ":icons/common/graphics/busInterfaceMirroredMaster.png";
    }
    else if (interfaceMode == General::SLAVE)
    {
        iconPath = ":icons/common/graphics/busInterfaceSlave.png";
    }
    else if (interfaceMode == General::MIRROREDSLAVE)
    {
        iconPath = ":icons/common/graphics/busInterfaceMirroredSlave.png";
    }
    else if (interfaceMode == General::SYSTEM)
    {
        iconPath = ":icons/common/graphics/busInterfaceSystem.png";
    }
    else if (interfaceMode == General::MIRROREDSYSTEM)
    {
        iconPath = ":icons/common/graphics/busInterfaceMirroredSystem.png";
    }
    else if (interfaceMode == General::MONITOR)
    {
        iconPath = ":icons/common/graphics/busInterfaceMonitor.png";
    }

    return iconPath;
}
