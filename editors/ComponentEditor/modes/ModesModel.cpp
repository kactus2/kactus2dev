//-----------------------------------------------------------------------------
// File: ModesModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.08.2023
//
// Description:
// Model for Mode elements within a component.
//-----------------------------------------------------------------------------

#include "ModesModel.h"

#include "ModeColumns.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Mode.h>

#include <QStringList>
#include <QRegularExpression>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: ModesModel::ModesModel()
//-----------------------------------------------------------------------------
ModesModel::ModesModel(QSharedPointer<Component> component, 
    ExpressionSet expressions,
    QObject* parent): 
    QAbstractTableModel(parent),
    modes_(component->getModes())
{

}

//-----------------------------------------------------------------------------
// Function: ModesModel::rowCount()
//-----------------------------------------------------------------------------
int ModesModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return modes_->count();
}

//-----------------------------------------------------------------------------
// Function: ModesModel::columnCount()
//-----------------------------------------------------------------------------
int ModesModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return ModeColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ModesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ModesModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ModesModel::headerData()
//-----------------------------------------------------------------------------
QVariant ModesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (section == ModeColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == ModeColumns::DISPLAY_NAME)
    {
        return tr("Display\nname");
    }
    else if (section == ModeColumns::SHORT_DESCRIPTION)
    {
        return tr("Short\ndescription");
    }
    else if (section == ModeColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ModesModel::data()
//-----------------------------------------------------------------------------
QVariant ModesModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= modes_->size())
    {
        return QVariant();
    }

    QSharedPointer<Mode> mode = modes_->at(index.row());
    const int column = index.column(); 

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (column == ModeColumns::NAME)
        {
            return mode->name();
        }
        else if (column == ModeColumns::DISPLAY_NAME)
        {
            return mode->displayName();
        }
        else if (column == ModeColumns::SHORT_DESCRIPTION)
        {
            return mode->shortDescription();
        }
        else if (column == ModeColumns::DESCRIPTION)
        {
            return mode->description();
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (column == ModeColumns::NAME)
        {
            return KactusColors::MANDATORY_FIELD;
        }
    }

    // if unsupported role
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ModesModel::setData()
//-----------------------------------------------------------------------------
bool ModesModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= modes_->size() || role != Qt::EditRole)
    {
        return false;
    }

    QSharedPointer<Mode> Mode = modes_->at(index.row());
    const int column = index.column();

    if (column == ModeColumns::NAME)
    {
        Mode->setName(value.toString());
    }
    else if (column == ModeColumns::DISPLAY_NAME)
    {
        Mode->setDisplayName(value.toString());
    }
    else if (column == ModeColumns::SHORT_DESCRIPTION)
    {
        Mode->setShortDescription(value.toString());
    }
    else if (column == ModeColumns::DESCRIPTION)
    {
        modes_->at(index.row())->setDescription(value.toString());
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
// Function: ModesModel::onAddItem()
//-----------------------------------------------------------------------------
void ModesModel::onAddItem(QModelIndex const& index)
{
	int row = modes_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	modes_->insert(row, QSharedPointer<Mode>(new Mode()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit modeAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ModesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ModesModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= modes_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	modes_->removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit modeRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
