//-----------------------------------------------------------------------------
// File: envidentifiereditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 18.04.2011
//
// Description:
// Model that contains the environment identifiers to be displayed to user.
//-----------------------------------------------------------------------------

#include "envidentifiersmodel.h"
#include "EnvIdentifiersColumns.h"

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::EnvIdentifiersModel()
//-----------------------------------------------------------------------------
EnvIdentifiersModel::EnvIdentifiersModel(QSharedPointer<View> view, QObject *parent ):
QAbstractTableModel(parent), 
view_(view),
table_(view->getEnvIdentifiers())
{
	
}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::~EnvIdentifiersModel()
//-----------------------------------------------------------------------------
EnvIdentifiersModel::~EnvIdentifiersModel()
{

}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::rowCount()
//-----------------------------------------------------------------------------
int EnvIdentifiersModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return table_->size();
}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::columnCount()
//-----------------------------------------------------------------------------
int EnvIdentifiersModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return EnvIdentifiersColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::data()
//-----------------------------------------------------------------------------
QVariant EnvIdentifiersModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= table_->size())
    {
		return QVariant();
    }
	else if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		if ( index.column() == 0 )
		{
			return table_->at(index.row())->language;
		}
		else if ( index.column() == 1 )
		{
			return table_->at(index.row())->tool;
		}
		else if ( index.column() == 2 )
		{
			return table_->at(index.row())->vendorSpecific;
		}

		return "";
	}
	// if unsupported role
	else
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::headerData()
//-----------------------------------------------------------------------------
QVariant EnvIdentifiersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (section == EnvIdentifiersColumns::LANGUAGE)
        {
            return tr("Language");
        }
        else if (section == EnvIdentifiersColumns::TOOL)
        {
            return tr("Tool");
        }
        else if (section == EnvIdentifiersColumns::VENDOR_SPECIFIC)
        {
            return tr("Vendor specific");
        }
    }
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::setData()
//-----------------------------------------------------------------------------
bool EnvIdentifiersModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() ||index.row() < 0 || index.row() >= table_->size())
    {
        return false;
    }
	else if (role == Qt::EditRole)
    {
		int row = index.row();

		QSharedPointer<View::EnvironmentIdentifier> envId = table_->at(row);

		if ( index.column() == 0 )
		{
			envId->language = value.toString();
		}
		else if ( index.column() == 1 )
		{
			envId->tool = value.toString();
		}
		else if ( index.column() == 2 )
		{
			envId->vendorSpecific = value.toString();
		}

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	// if unsupported role
	else
    {
		return false;
    }
}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags EnvIdentifiersModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::isValid()
//-----------------------------------------------------------------------------
bool EnvIdentifiersModel::isValid() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::onRemoveItem()
//-----------------------------------------------------------------------------
void EnvIdentifiersModel::onRemoveItem( QModelIndex const& index )
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= table_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	table_->removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::onAddItem()
//-----------------------------------------------------------------------------
void EnvIdentifiersModel::onAddItem( QModelIndex const& index )
{
	int row = table_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	QSharedPointer<View::EnvironmentIdentifier> envId(new View::EnvironmentIdentifier);
	table_->insert(row, envId);
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
