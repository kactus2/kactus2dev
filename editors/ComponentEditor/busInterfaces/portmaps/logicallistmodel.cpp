//-----------------------------------------------------------------------------
// File: logicallistmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.5.2011
//
// Description:
// Model to display the Logical ports of an abstraction definition.
//-----------------------------------------------------------------------------

#include "logicallistmodel.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QIcon>

//-----------------------------------------------------------------------------
// Function: LogicalListModel::LogicalListModel()
//-----------------------------------------------------------------------------
LogicalListModel::LogicalListModel(LibraryInterface* libHandler, QObject* parent):
PortListModel(parent), 
    libHandler_(libHandler),
    absTypeVLNV_(),
    mode_(General::MASTER)
{
        refresh();
}

//-----------------------------------------------------------------------------
// Function: LogicalListModel::~LogicalListModel()
//-----------------------------------------------------------------------------
LogicalListModel::~LogicalListModel()
{
}

//-----------------------------------------------------------------------------
// Function: LogicalListModel::setAbsType()
//-----------------------------------------------------------------------------
void LogicalListModel::setAbsType(VLNV const& vlnv, General::InterfaceMode mode)
{
	absTypeVLNV_ = vlnv;
	
	// update the mode
	mode_ = mode;

	if (!vlnv.isValid())
    {
		beginResetModel();
		list_.clear();
		endResetModel();
		return;
	}

	// ask library to parse the model for abstraction definition
	QSharedPointer<Document> document = libHandler_->getModel(vlnv);
	if (!document) 
    {
        return;
    }

	QSharedPointer<AbstractionDefinition> absdef;
	if (document->getVlnv().getType() == VLNV::ABSTRACTIONDEFINITION)
    {
		absdef = document.staticCast<AbstractionDefinition>();
    }
	else 
    {
		return;
    }
    
	beginResetModel();
	list_ = absdef->getPortNames(mode_);
    list_.removeDuplicates();
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: LogicalListModel::refresh()
//-----------------------------------------------------------------------------
void LogicalListModel::refresh()
{
	setAbsType(absTypeVLNV_, mode_);
}

//-----------------------------------------------------------------------------
// Function: LogicalListModel::refresh()
//-----------------------------------------------------------------------------
QVariant LogicalListModel::data(QModelIndex const& index, int role) const 
{
    if (role == Qt::DecorationRole)
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
            return QIcon(":icons/common/graphics/cross.png");
        }   

        DirectionTypes::Direction direction = absdef->getPortDirection(data(index).toString(), mode_);      
        if (direction == DirectionTypes::IN)
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

    return PortListModel::data(index,role);
}
