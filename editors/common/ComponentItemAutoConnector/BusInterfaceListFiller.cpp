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

#include <KactusAPI/include/BusInterfaceUtilities.h>

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
            QIcon modeIcon(BusInterfaceUtilities::getIconPath(busMode));
            QStandardItem* newBusItem = new QStandardItem(modeIcon, busInterface->name());
            newBusItem->setEditable(false);
            selectedList->appendRow(newBusItem);
        }
    }
}
