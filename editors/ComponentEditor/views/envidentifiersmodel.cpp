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

#include <IPXACTmodels/Component/View.h>

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::EnvIdentifiersModel()
//-----------------------------------------------------------------------------
EnvIdentifiersModel::EnvIdentifiersModel(QSharedPointer<View> view, QObject *parent ):
QAbstractTableModel(parent), 
view_(view),
table_(view->getEnvIdentifiers())
{
	Q_ASSERT_X(view_, "EnvIdentifiersModel constructor",
		"Null view pointer given as parameter");
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
int EnvIdentifiersModel::rowCount(const QModelIndex& parent /*= QModelIndex() */ ) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return table_.size();
}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::columnCount()
//-----------------------------------------------------------------------------
int EnvIdentifiersModel::columnCount( const QModelIndex& parent /*= QModelIndex() */ ) const
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
QVariant EnvIdentifiersModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole */ ) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
		return QVariant();
    }

	else if (Qt::DisplayRole == role)
    {
		// split the identifier
		QStringList fields = table_.at(index.row()).split(":");

		// and return the correct field
		return fields.value(index.column());
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
QVariant EnvIdentifiersModel::headerData( int section, Qt::Orientation orientation,
    int role /*= Qt::DisplayRole */ ) const
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
bool EnvIdentifiersModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole */ )
{
	if (!index.isValid() ||index.row() < 0 || index.row() >= table_.size())
    {
        return false;
    }
	else if (Qt::EditRole == role)
    {
		QStringList identifier = table_.at(index.row()).split(":");
		identifier.replace(index.column(), value.toString());
		QString result = identifier.value(0);
		result += QString(":");
		result += identifier.value(1);
		result += QString(":");
		result += identifier.value(2);
		table_.replace(index.row(), result);
		
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
Qt::ItemFlags EnvIdentifiersModel::flags( const QModelIndex& index ) const
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
	// at least one has to be specified.
	return !table_.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::onRemoveItem()
//-----------------------------------------------------------------------------
void EnvIdentifiersModel::onRemoveItem( const QModelIndex& index )
{
	// don't remove anything if index is invalid
	if (!index.isValid())
    {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= table_.size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	table_.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: envidentifiersmodel::onAddItem()
//-----------------------------------------------------------------------------
void EnvIdentifiersModel::onAddItem( const QModelIndex& index )
{
	int row = table_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	table_.insert(row, QString("::"));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
