//-----------------------------------------------------------------------------
// File: PortList.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.01.2019
//
// Description:
// Lists component ports as name, direction pairs.
//-----------------------------------------------------------------------------

#include "PortList.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: PortList::PortList()
//-----------------------------------------------------------------------------
PortList::PortList(QSharedPointer<Component> containingComponent, QWidget* parent):
QListWidget(parent)
{
    setupPortList(containingComponent);
}

//-----------------------------------------------------------------------------
// Function: PortList::~PortList()
//-----------------------------------------------------------------------------
PortList::~PortList()
{

}

//-----------------------------------------------------------------------------
// Function: PortList::setupPortList()
//-----------------------------------------------------------------------------
void PortList::setupPortList(QSharedPointer<Component> containingComponent)
{
    for (auto currentPort : *containingComponent->getPorts())
    {
        QString iconPath = ":icons/common/graphics/cross.png";
        if (currentPort->getDirection() == DirectionTypes::IN)
        {
            iconPath = ":icons/common/graphics/input.png";
        }
        else if (currentPort->getDirection() == DirectionTypes::OUT)
        {
            iconPath = ":icons/common/graphics/output.png";
        }
        else if (currentPort->getDirection() == DirectionTypes::INOUT)
        {
            iconPath = ":icons/common/graphics/inout.png";
        }
        else if (currentPort->getDirection() == DirectionTypes::DIRECTION_PHANTOM)
        {
            iconPath = ":icons/common/graphics/phantom.png";
        }

        new QListWidgetItem(QIcon(iconPath), currentPort->name(), this);
    }
}
