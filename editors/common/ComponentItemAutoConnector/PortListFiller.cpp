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

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: PortListFiller::PortListFiller()
//-----------------------------------------------------------------------------
PortListFiller::PortListFiller():
ListFiller()
{

}

//-----------------------------------------------------------------------------
// Function: PortListFiller::~PortListFiller()
//-----------------------------------------------------------------------------
PortListFiller::~PortListFiller()
{

}

//-----------------------------------------------------------------------------
// Function: PortListFiller::initializeList()
//-----------------------------------------------------------------------------
void PortListFiller::initializeList(QListWidget* selectedList, QSharedPointer<Component> containingComponent) const
{
    for (auto port : *containingComponent->getPorts())
    {
        QString iconPath = getIconPath(port->getDirection());

        new QListWidgetItem(QIcon(iconPath), port->name(), selectedList);
    }
}

//-----------------------------------------------------------------------------
// Function: PortListFiller::getIconPath()
//-----------------------------------------------------------------------------
QString PortListFiller::getIconPath(DirectionTypes::Direction portDirection) const
{
    QString iconPath = ":icons/common/graphics/cross.png";
    if (portDirection == DirectionTypes::IN)
    {
        iconPath = ":icons/common/graphics/input.png";
    }
    else if (portDirection == DirectionTypes::OUT)
    {
        iconPath = ":icons/common/graphics/output.png";
    }
    else if (portDirection == DirectionTypes::INOUT)
    {
        iconPath = ":icons/common/graphics/inout.png";
    }
    else if (portDirection == DirectionTypes::DIRECTION_PHANTOM)
    {
        iconPath = ":icons/common/graphics/phantom.png";
    }

    return iconPath;
}
