//-----------------------------------------------------------------------------
// File: apiinterfacesmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.6.2012
//
// Description:
// The model that manages the API interfaces for API Interface editor.
//-----------------------------------------------------------------------------

#include "apiinterfacesmodel.h"
#include "ApiInterfaceColumns.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QColor>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: ApiInterfacesModel::ApiInterfacesModel()
//-----------------------------------------------------------------------------
ApiInterfacesModel::ApiInterfacesModel(QSharedPointer<Component> component, QObject* parent):
QAbstractTableModel(parent),
    component_(component),
    apis_(component->getApiInterfaces())
{

}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesModel::~ApiInterfacesModel()
//-----------------------------------------------------------------------------
ApiInterfacesModel::~ApiInterfacesModel()
{
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesModel::rowCount()
//-----------------------------------------------------------------------------
int ApiInterfacesModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return apis_.size();
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesModel::columnCount()
//-----------------------------------------------------------------------------
int ApiInterfacesModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return ApiInterfaceColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ApiInterfacesModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	// API definition can only be dropped.
	else if (ApiInterfaceColumns::API_DEFINITION == index.column())
    {
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesModel::headerData()
//-----------------------------------------------------------------------------
QVariant ApiInterfacesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
		return QVariant();
	}
	
    if (section == ApiInterfaceColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == ApiInterfaceColumns::API_DEFINITION)
    {
        return tr("API definition");
    }
    else if (section == ApiInterfaceColumns::DEPENDENCY)
    {
        return tr("Dependency");
    }
    else if (section == ApiInterfaceColumns::DISPLAY_NAME) 
    {
        return tr("Display name");
    }
    else if (section == ApiInterfaceColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesModel::data()
//-----------------------------------------------------------------------------
QVariant ApiInterfacesModel::data(QModelIndex const& index, int role) const 
{	
    if (!index.isValid() || index.row() < 0 || index.row() >= apis_.size())
    {
		return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (index.column() == ApiInterfaceColumns::NAME)
        {
            return apis_.at(index.row())->name();
        }
        else if (index.column() == ApiInterfaceColumns::API_DEFINITION)
        {
            return apis_.at(index.row())->getApiType().toString(":");
        }
        else if (index.column() == ApiInterfaceColumns::DEPENDENCY)
        {
            return dependencyDirection2Str(apis_.at(index.row())->getDependencyDirection());
        }
        else if (index.column() == ApiInterfaceColumns::DISPLAY_NAME)
        {
            return apis_.at(index.row())->displayName();
        }
        else if (index.column() == ApiInterfaceColumns::DESCRIPTION)
        {
            return apis_.at(index.row())->description();
        }
        else
        {
            return QVariant();
        }
    }

	else if (role == Qt::ForegroundRole)
    {
		if (apis_.at(index.row())->isValid())
        {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}

	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == ApiInterfaceColumns::NAME)
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
// Function: ApiInterfacesModel::setData()
//-----------------------------------------------------------------------------
bool ApiInterfacesModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= apis_.size() || role != Qt::EditRole)
    {
		return false;
	}

    if (index.column() == ApiInterfaceColumns::NAME)
    {
        apis_.at(index.row())->setName(value.toString());
    }
    else if (index.column() == ApiInterfaceColumns::API_DEFINITION)
    {
        VLNV apiDef = VLNV(VLNV::APIDEFINITION, value.toString(), ":");
        apis_.at(index.row())->setApiType(apiDef);
    }
    else if (index.column() == ApiInterfaceColumns::DEPENDENCY) 
    {
        apis_.at(index.row())->setDependencyDirection(str2DependencyDirection(value.toString(),
            DEPENDENCY_REQUESTER));
    }
    else if (index.column() == ApiInterfaceColumns::DISPLAY_NAME) 
    {
        apis_.at(index.row())->setDisplayName(value.toString());
    }
    else if (index.column() == ApiInterfaceColumns::DESCRIPTION)
    {
        apis_.at(index.row())->setDescription(value.toString());
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
// Function: ApiInterfacesModel::isValid()
//-----------------------------------------------------------------------------
bool ApiInterfacesModel::isValid() const
{	
	foreach (QSharedPointer<ApiInterface> apiIf, apis_)
    {
		if (!apiIf->isValid())
        {
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions ApiInterfacesModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList ApiInterfacesModel::mimeTypes() const
{
    QStringList types(QAbstractTableModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesModel::dropMimeData()
//-----------------------------------------------------------------------------
bool ApiInterfacesModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, 
    QModelIndex const& parent)
{
    if ( action == Qt::IgnoreAction)
    {
        return true;
    }

    // Dropped data must be directly on parent.
    if ( row != -1 || column != -1 || !parent.isValid() )
    {
        return false;
    }

    if ( row > rowCount() )
    {
        return false;
    }

    QVariant variant = data->imageData();

    if ( !variant.canConvert<VLNV>())
    {
        return false;
    }

    VLNV vlnv = variant.value<VLNV>();

    if ( parent.column() == ApiInterfaceColumns::API_DEFINITION )
    {
        if ( vlnv.getType() != VLNV::APIDEFINITION )
        {
            return false;
        }

        setData(index(parent.row(),parent.column()),vlnv.toString(":"));
        emit contentChanged();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesModel::onAddItem()
//-----------------------------------------------------------------------------
void ApiInterfacesModel::onAddItem(QModelIndex const& index)
{
	int row = apis_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	apis_.insert(row, QSharedPointer<ApiInterface>(new ApiInterface()));
    component_->setApiInterfaces(apis_);
	endInsertRows();

	// inform navigation tree that file set is added
	emit apiAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ApiInterfacesModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= apis_.size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	apis_.removeAt(index.row());
    component_->setApiInterfaces(apis_);
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit apiRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
