//-----------------------------------------------------------------------------
// File: cominterfacesmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 28.06.2012
//
// Description:
// The model that manages the COM interfaces.
//-----------------------------------------------------------------------------

#include "cominterfacesmodel.h"

#include "ComInterfaceColumns.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QColor>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::ComInterfacesModel()
//-----------------------------------------------------------------------------
ComInterfacesModel::ComInterfacesModel(LibraryInterface* libHandler, QSharedPointer<Component> component,
    QObject *parent):
QAbstractTableModel(parent),
    libHandler_(libHandler),
    component_(component),
    comIfs_(component->getComInterfaces())
{
    Q_ASSERT(component);
    Q_ASSERT(libHandler_);
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::~ComInterfacesModel()
//-----------------------------------------------------------------------------
ComInterfacesModel::~ComInterfacesModel()
{
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::rowCount()
//-----------------------------------------------------------------------------
int ComInterfacesModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return comIfs_.size();
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::columnCount()
//-----------------------------------------------------------------------------
int ComInterfacesModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return ComInterfaceColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ComInterfacesModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	// Com definition column can only be dropped..
	if (ComInterfaceColumns::COM_DEFINITION == index.column())
    {
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::headerData()
//-----------------------------------------------------------------------------
QVariant ComInterfacesModel::headerData( int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) 
    {
        return QVariant();
    }

    if (section == ComInterfaceColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == ComInterfaceColumns::COM_DEFINITION)
    {
        return tr("COM definition");
    }
    else if (section == ComInterfaceColumns::TRANSFER_TYPE)
    {
        return tr("Transfer type");
    }
    else if (section == ComInterfaceColumns::DIRECTION)
    {
        return tr("Direction");
    }
    else if (section == ComInterfaceColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::data()
//-----------------------------------------------------------------------------
QVariant ComInterfacesModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= comIfs_.size())
    {
		return QVariant();
	}

	if (role == Qt::DisplayRole)
    {
        if (index.column() == ComInterfaceColumns::NAME)
        {
            return comIfs_.at(index.row())->name();
        }
        else if (index.column() == ComInterfaceColumns::COM_DEFINITION)
        {
            return comIfs_.at(index.row())->getComType().toString(":");
        }
        else if (index.column() == ComInterfaceColumns::TRANSFER_TYPE)
        {
            return comIfs_.at(index.row())->getTransferType();
        }
        else if (index.column() == ComInterfaceColumns::DIRECTION)
        {
            return DirectionTypes::direction2Str(comIfs_.at(index.row())->getDirection());
        }
        else if (index.column() == ComInterfaceColumns::DESCRIPTION)
        {
            return comIfs_.at(index.row())->description();
        }
        else
        {
            return QVariant();
        }

    }
    else if (role == ComInterfaceColumns::TRANSFER_TYPE_OPTIONS)
    {
        // find the vlnv of the com interface
        VLNV comDefVLNV = comIfs_.at(index.row())->getComType();

        // if the com def is not defined
        if (!comDefVLNV.isValid() || !libHandler_->contains(comDefVLNV) || 
            libHandler_->getDocumentType(comDefVLNV) != VLNV::COMDEFINITION)
        {
            return QStringList();
        }

        // parse the com definition
        QSharedPointer<Document const> libComp = libHandler_->getModelReadOnly(comDefVLNV);
        const QSharedPointer<ComDefinition const> comDef = libComp.staticCast<ComDefinition const>();

        // and return the transfer types specified in the com definition
        return *comDef->getTransferTypes();
    }
    else if (role == Qt::ForegroundRole)
    {
        if (comIfs_.at(index.row())->isValid())
        {
            return QColor("black");
        }
        else
        {
            return QColor("red");
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == ComInterfaceColumns::NAME ||
            index.column() == ComInterfaceColumns::DIRECTION)
        {
            return QColor("LemonChiffon");
        }
        else
        {
            return QColor("white");
        }
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::setData()
//-----------------------------------------------------------------------------
bool ComInterfacesModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= comIfs_.size() || role != Qt::EditRole)
    {
		return false;
	}

    if (index.column() == ComInterfaceColumns::NAME)
    {
        comIfs_[index.row()]->setName(value.toString());
    }                                    
    else if (index.column() == ComInterfaceColumns::COM_DEFINITION)
    {
        VLNV comDef = VLNV(VLNV::COMDEFINITION, value.toString(), ":");
        comIfs_[index.row()]->setComType(comDef);
    }
    else if (index.column() == ComInterfaceColumns::TRANSFER_TYPE)
    {
        comIfs_[index.row()]->setTransferType(value.toString());
    }
    else if (index.column() == ComInterfaceColumns::DIRECTION)
    {
        comIfs_[index.row()]->setDirection(DirectionTypes::str2Direction(value.toString(), DirectionTypes::IN));
    }
    else if (index.column() == ComInterfaceColumns::DESCRIPTION)
    {
        comIfs_[index.row()]->setDescription(value.toString());
    }
    else
    {
        return false;
    }

    emit dataChanged(index, index);
    emit contentChanged();
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::isValid()
//-----------------------------------------------------------------------------
bool ComInterfacesModel::isValid() const
{
	foreach (QSharedPointer<ComInterface> comIf, comIfs_)
    {
		if (!comIf->isValid())
        {
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions ComInterfacesModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList ComInterfacesModel::mimeTypes() const
{
    QStringList types(QAbstractTableModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::dropMimeData()
//-----------------------------------------------------------------------------
bool ComInterfacesModel::dropMimeData(QMimeData const* data, Qt::DropAction action, int row, int column, 
    QModelIndex const& parent)
{
    if (action == Qt::IgnoreAction)
    {
        return true;
    }

    // Dropped data must be directly on parent.
    if ( row != -1 || column != -1 || !parent.isValid() || row > rowCount())
    {
        return false;
    }

    QVariant variant = data->imageData();
    if (!variant.canConvert<VLNV>())
    {
        return false;
    }

    VLNV vlnv = variant.value<VLNV>();

    if (parent.column() == ComInterfaceColumns::COM_DEFINITION)
    {
        if (vlnv.getType() != VLNV::COMDEFINITION)
        {
            return false;
        }

        setData(index(parent.row(),parent.column()),vlnv.toString(":"));
        emit contentChanged();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::onAddItem()
//-----------------------------------------------------------------------------
void ComInterfacesModel::onAddItem(QModelIndex const& index)
{
	int row = comIfs_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	comIfs_.insert(row, QSharedPointer<ComInterface>(new ComInterface()));
    component_->setComInterfaces(comIfs_);
	endInsertRows();

	// inform navigation tree that COM interface is added
	emit comIfAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ComInterfacesModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= comIfs_.size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	comIfs_.removeAt(index.row());
    component_->setComInterfaces(comIfs_);
	endRemoveRows();

	// inform navigation tree that COM interface has been removed
	emit comIfRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
