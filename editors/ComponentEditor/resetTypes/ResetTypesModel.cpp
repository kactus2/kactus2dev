//-----------------------------------------------------------------------------
// File: ResetTypesModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.01.2019
//
// Description:
//! Model for reset type elements within a component.
//-----------------------------------------------------------------------------

#include "ResetTypesModel.h"
#include "ResetTypeColumns.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/ResetType.h>
#include <IPXACTmodels/Component/validators/ComponentValidator.h>

#include <common/KactusColors.h>

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ResetTypesModel::ResetTypesModel()
//-----------------------------------------------------------------------------
ResetTypesModel::ResetTypesModel(QSharedPointer<Component> component, ComponentValidator& validator,
    QObject* parent):
QAbstractTableModel(parent),
component_(component),
resetTypes_(component_->getResetTypes()),
validator_(validator)
{

}

//-----------------------------------------------------------------------------
// Function: ResetTypesModel::~ResetTypesModel()
//-----------------------------------------------------------------------------
ResetTypesModel::~ResetTypesModel()
{
}

//-----------------------------------------------------------------------------
// Function: ResetTypesModel::rowCount()
//-----------------------------------------------------------------------------
int ResetTypesModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return resetTypes_->size();
}

//-----------------------------------------------------------------------------
// Function: ResetTypesModel::ResetTypesModel()
//-----------------------------------------------------------------------------
int ResetTypesModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return ResetTypeColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ResetTypesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ResetTypesModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ResetTypesModel::headerData()
//-----------------------------------------------------------------------------
QVariant ResetTypesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (section == ResetTypeColumns::NAME_COLUMN)
    {
        return tr("Name");
    }
    else if (section == ResetTypeColumns::DISPLAY_NAME_COLUMN)
    {
        return tr("Display name");
    }
    else if (section == ResetTypeColumns::DESCRIPTION_COLUMN)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ResetTypesModel::data()
//-----------------------------------------------------------------------------
QVariant ResetTypesModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= resetTypes_->size())
    {
		return QVariant();
	}
    
    QSharedPointer<ResetType> resetType = resetTypes_->at(index.row());

	if (role == Qt::DisplayRole)
    {
        if (index.column() == ResetTypeColumns::NAME_COLUMN)
        {
            return resetType->name();
        }
        else if (index.column() == ResetTypeColumns::DISPLAY_NAME_COLUMN)
        {
            return resetType->displayName();
        }
        else if (index.column() == ResetTypeColumns::DESCRIPTION_COLUMN)
        {
            return resetType->description();
        }
        else
        {
            return QVariant();
        }
	}
    else if (role == Qt::ForegroundRole)
    {
        if (index.column() == ResetTypeColumns::NAME_COLUMN &&
            !validator_.singleResetTypeIsValid(resetType, resetTypes_))
        {
            return KactusColors::ERROR;
        }
        else
        {
           return KactusColors::REGULAR_TEXT;
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == ResetTypeColumns::NAME_COLUMN)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
        }
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ResetTypesModel::setData()
//-----------------------------------------------------------------------------
bool ResetTypesModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= resetTypes_->size())
    {
		return false;
	}

    QSharedPointer<ResetType> resetType = resetTypes_->at(index.row());

    if (role == Qt::EditRole)
    {
        if (index.column() == ResetTypeColumns::NAME_COLUMN)
        {
            resetType->setName(value.toString());
        }
        else if (index.column() == ResetTypeColumns::DISPLAY_NAME_COLUMN)
        {
            resetType->setDisplayName(value.toString());
        }
        else if (index.column() == ResetTypeColumns::DESCRIPTION_COLUMN)
        {
            resetType->setDescription(value.toString());
        }
        else
        {
            return false;
        }

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
    else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: ResetTypesModel::onAddItem()
//-----------------------------------------------------------------------------
void ResetTypesModel::onAddItem(QModelIndex const& index)
{
	int row = resetTypes_->size();

	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	resetTypes_->insert(row, QSharedPointer<ResetType>(new ResetType()));
	endInsertRows();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ResetTypesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ResetTypesModel::onRemoveItem(QModelIndex const& index)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= resetTypes_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	resetTypes_->removeAt(index.row());
	endRemoveRows();

    emit contentChanged();
}
