//-----------------------------------------------------------------------------
// File: physlistmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.05.2011
//
// Description:
// Model to display the physical ports of a component.
//-----------------------------------------------------------------------------

#include "physlistmodel.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QIcon>

//-----------------------------------------------------------------------------
// Function: PhysListModel::PhysListModel()
//-----------------------------------------------------------------------------
PhysListModel::PhysListModel(QSharedPointer<Component> component, QObject* parent):
PortListModel(parent), 
    component_(component)
{
	refresh();
}

//-----------------------------------------------------------------------------
// Function: PhysListModel::~PhysListModel()
//-----------------------------------------------------------------------------
PhysListModel::~PhysListModel()
{
}

//-----------------------------------------------------------------------------
// Function: PhysListModel::refresh()
//-----------------------------------------------------------------------------
void PhysListModel::refresh()
{
	beginResetModel();

	list_ = component_->getPortNames();
    list_.removeDuplicates();

	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PhysListModel::data()
//-----------------------------------------------------------------------------
QVariant PhysListModel::data(QModelIndex const& index, int role) const 
{
    if (role == Qt::DecorationRole)
    {
        QSharedPointer<Port> selectedPort = component_->getPort(data(index).toString());
        if (selectedPort && selectedPort->getWire())
        {
            DirectionTypes::Direction direction = selectedPort->getDirection();
            if(direction == DirectionTypes::IN)
            {
                return QIcon(":icons/common/graphics/control-180.png");
            }
            else if (direction == DirectionTypes::OUT)
            {
                return QIcon(":icons/common/graphics/control.png");
            }
            else if (direction == DirectionTypes::INOUT)
            {
                return QIcon(":icons/common/graphics/control-dual.png");
            }
            else
            {
                return QIcon(":icons/common/graphics/cross.png");
            }
        }
    }

    return PortListModel::data(index,role);
}
