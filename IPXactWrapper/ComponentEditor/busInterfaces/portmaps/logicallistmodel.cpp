/* 
 *
 *  Created on: 11.5.2011
 *      Author: Antti Kamppi
 * 		filename: logicallistmodel.cpp
 */

#include "logicallistmodel.h"
#include "portmapsmodel.h"

#include <LibraryManager/libraryinterface.h>
#include <models/librarycomponent.h>
#include <LibraryManager/vlnv.h>
#include <models/abstractiondefinition.h>

LogicalListModel::LogicalListModel(LibraryInterface* libHandler,
								   PortMapsModel* portMapsModel,
								   QObject *parent ):
PortListModel(portMapsModel, parent), 
libHandler_(libHandler),
absTypeVLNV_(),
mode_(General::MASTER) {

	refresh();
}

LogicalListModel::~LogicalListModel() {
}

void LogicalListModel::setAbsType( const VLNV& vlnv, General::InterfaceMode mode ) {

	absTypeVLNV_ = vlnv;
	
	// update the mode
	mode_ = mode;

	if (!vlnv.isValid()) {
		beginResetModel();
		PortListModel::list_.clear();
		endResetModel();
		return;
	}

	// ask library to parse the model for abstraction definition
	QSharedPointer<LibraryComponent> libComb;
	if (libHandler_->contains(vlnv)) 
		libComb = libHandler_->getModel(vlnv);

	// if library did not contain the vlnv
	else
		return;

	// make sure the model is for abstraction definition
	QSharedPointer<AbstractionDefinition> absdef;
	if (libComb->getVlnv()->getType() == VLNV::ABSTRACTIONDEFINITION)
		absdef = libComb.staticCast<AbstractionDefinition>();

	// if model was some other type then free memory and exit
	else {
		return;
	}

	// ask the abstraction definition for list of the port names
	QStringList portNames = absdef->getPortNames(mode_);

	beginResetModel();

	list_.clear();

	foreach (QString portName, portNames) {

		// if the port is not yet in the list
		if (!PortListModel::list_.contains(portName))
			PortListModel::list_.append(portName);
	}

	endResetModel();
}

void LogicalListModel::refresh() {

	setAbsType(absTypeVLNV_, mode_);
}

// void LogicalListModel::onPortDropped( const QString& droppedPort, const QModelIndex& dropIndex ) {
// 
// 	if (!dropIndex.isValid())
// 		return;
// 	else if (droppedPort.isEmpty())
// 		return;
// 
// 	// find the physical port name 
// 	QString logicalName = list_.at(dropIndex.row());
// 	
// 	if (portMapsModel_->canCreateMap(droppedPort, logicalName)) {
// 		// inform that ports should be mapped together
// 		emit createMap(droppedPort, logicalName);
// 
// 		removeItem(dropIndex);
// 	}
// 
// }
// 
// void LogicalListModel::onPortsDropped( const QStringList& ports, const QModelIndex& dropIndex ) {
// 	if (!dropIndex.isValid())
// 		return;
// 	else if (ports.isEmpty())
// 		return;
// 
// 	QString logicalName = list_.at(dropIndex.row());
// 
// 	bool madeConnection = false;
// 	foreach (QString physicalPort, ports) {
// 		if (!physicalPort.isEmpty() && 
// 			portMapsModel_->canCreateMap(physicalPort, logicalName)) {
// 				madeConnection = true;
// 				emit createMap(physicalPort, logicalName);
// 		}
// 	}
// 	if (madeConnection)
// 		removeItem(dropIndex);
// }
// 
// void LogicalListModel::onPortsDropped( const QStringList& ports, const QModelIndexList& indexes ) {
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
// 		QString logicalPort = list_.at(indexes.at(i).row());
// 
// 		if (portMapsModel_->canCreateMap(ports.at(i), logicalPort)) {
// 			emit createMap(ports.at(i), logicalPort);
// 			indexesToRemove.append(indexes.at(i));
// 		}
// 	}
// 	removeItems(indexesToRemove);
// }
