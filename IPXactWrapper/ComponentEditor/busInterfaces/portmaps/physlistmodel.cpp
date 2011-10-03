/* 
 *
 *  Created on: 11.5.2011
 *      Author: Antti Kamppi
 * 		filename: physlistmodel.cpp
 */

#include "physlistmodel.h"
#include "portmapsmodel.h"

#include <models/component.h>
#include <models/port.h>

PhysListModel::PhysListModel(QSharedPointer<Component> component, 
							 PortMapsModel* portMapsModel,
							 QObject *parent ):
PortListModel(portMapsModel, parent), component_(component) {

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
			PortListModel::list_.append(portName);
	}

	endResetModel();
}

// void PhysListModel::onPortDropped( const QString& droppedPort, const QModelIndex& dropIndex ) {
// 
// 	if (!dropIndex.isValid())
// 		return;
// 	else if (droppedPort.isEmpty())
// 		return;
// 
// 	// find the physical port name 
// 	QString PhysicalName = list_.at(dropIndex.row());
// 	
// 	if (portMapsModel_->canCreateMap(PhysicalName, droppedPort)) {
// 		// inform that ports should be mapped together
// 		emit createMap(PhysicalName, droppedPort);
// 		removeItem(dropIndex);
// 	}
// }
// 
// void PhysListModel::onPortsDropped( const QStringList& ports, 
// 								   const QModelIndex& dropIndex ) {
// 	if (!dropIndex.isValid())
// 		return;
// 	else if (ports.isEmpty())
// 		return;
// 
// 	QString physicalName = list_.at(dropIndex.row());
// 
// 	bool madeConnection = false;
// 	foreach (QString logicalPort, ports) {
// 		if (!logicalPort.isEmpty() && 
// 			portMapsModel_->canCreateMap(physicalName, logicalPort)) {
// 			madeConnection = true;
// 			emit createMap(physicalName, logicalPort);
// 		}
// 	}
// 	if (madeConnection)
// 		removeItem(dropIndex);
// }
// 
// void PhysListModel::onPortsDropped( const QStringList& ports, 
// 								   const QModelIndexList& indexes ) {
// 	if (ports.isEmpty())
// 		return;
// 	else if (indexes.isEmpty())
// 		return;
// 
// 	// list of indexes to remove when connections are made
// 	QModelIndexList indexesToRemove;
// 
// 	for (int i = 0; i < ports.size() && i < indexes.size(); ++i) {
// 		if (!indexes.at(i).isValid())
// 			continue;
// 		QString physicalPort = list_.at(indexes.at(i).row());
// 
// 		if (portMapsModel_->canCreateMap(physicalPort, ports.at(i))) {
// 			emit createMap(physicalPort, ports.at(i));
// 			indexesToRemove.append(indexes.at(i));
// 		}
// 	}
// 	removeItems(indexesToRemove);
// }
