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

