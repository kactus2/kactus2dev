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
void BusInterfaceListFiller::initializeList(QListWidget* selectedList,
    QSharedPointer<Component> containingComponent) const
{
    for (auto busInterface : *containingComponent->getBusInterfaces())
    {
        new QListWidgetItem(busInterface->name(), selectedList);
    }
}
