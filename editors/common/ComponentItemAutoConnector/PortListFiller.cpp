//-----------------------------------------------------------------------------
// File: PortListFiller.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Fills a list with ports.
//-----------------------------------------------------------------------------

#include "PortListFiller.h"

#include <editors/common/PortUtilities.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: PortListFiller::initializeList()
//-----------------------------------------------------------------------------
void PortListFiller::initializeList(QStandardItemModel* selectedList,
    QSharedPointer<Component> containingComponent) const
{
    for (auto port : *containingComponent->getPorts())
    {
        QString iconPath = PortUtilities::getDirectionIconPath(port->getDirection());

        QStandardItem* portItem = new QStandardItem(QIcon(iconPath), port->name());
        portItem->setEditable(false);

        selectedList->appendRow(portItem);
    }
}
