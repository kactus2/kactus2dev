/* 
 *
 *  Created on: 11.5.2011
 *      Author: Antti Kamppi
 * 		filename: physlistmodel.cpp
 */

#include "physlistmodel.h"
#include "PortMapsTreeModel.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/port.h>

#include <QIcon>

PhysListModel::PhysListModel(QSharedPointer<Component> component, 
							 PortMapsTreeModel* portMapsModel,
							 QObject *parent ):
PortListModel(portMapsModel, parent), component_(component)
{
	refresh();
}

PhysListModel::~PhysListModel() {
}

void PhysListModel::refresh() {
	
	beginResetModel();

	list_.clear();

    QStringList portNames = component_->getPortNames();
    foreach (QString portName, portNames) {

        // if the port is not yet in the list
        if (!PortListModel::list_.contains(portName))
        {
            PortListModel::list_.append(portName);
        }
    }

	endResetModel();
}

QVariant PhysListModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole */ ) const 
{
    if (Qt::DecorationRole == role)
    {
        General::Direction direction = component_->getPortDirection(data(index).toString());
        switch( direction )
        {
        case General::IN :
            return QIcon(":icons/graphics/control-180.png");

        case General::OUT :
            return QIcon(":icons/graphics/control.png");

        case General::INOUT :
            return QIcon(":icons/graphics/control-dual.png");

        default:
            return QIcon(":icons/graphics/cross.png");
        }
    }

    return PortListModel::data(index,role);
}
