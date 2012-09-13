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