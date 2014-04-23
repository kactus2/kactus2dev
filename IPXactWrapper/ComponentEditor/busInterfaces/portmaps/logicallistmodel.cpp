/* 
 *
 *  Created on: 11.5.2011
 *      Author: Antti Kamppi
 * 		filename: logicallistmodel.cpp
 */

#include "logicallistmodel.h"
#include "PortMapsTreeModel.h"

#include <library/LibraryManager/libraryinterface.h>
#include <library/LibraryManager/vlnv.h>
#include <models/librarycomponent.h>
#include <models/abstractiondefinition.h>

#include <QIcon>

LogicalListModel::LogicalListModel(LibraryInterface* libHandler,
								   PortMapsTreeModel* portMapsModel,
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

QVariant LogicalListModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole */ ) const 
{
    if (Qt::DecorationRole == role)
    {
        QSharedPointer<AbstractionDefinition> absdef;

        // ask library to parse the model for abstraction definition
        if (libHandler_->contains(absTypeVLNV_) && 
            absTypeVLNV_.getType() == VLNV::ABSTRACTIONDEFINITION) 
        {
            absdef = libHandler_->getModel(absTypeVLNV_).staticCast<AbstractionDefinition>();
        }
        else
        {
            return QVariant();
        }   

        General::Direction direction =absdef->getPortDirection(data(index).toString(), mode_);      
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