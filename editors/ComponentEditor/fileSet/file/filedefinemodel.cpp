//-----------------------------------------------------------------------------
// File: filedefinemodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.11.2015
//
// Description:
// Model to display the define-elements for a file.
//-----------------------------------------------------------------------------

#include "filedefinemodel.h"

#include <IPXACTmodels/common/NameValuePair.h>

#include <IPXACTmodels/Component/File.h>

//-----------------------------------------------------------------------------
// Function: FileDefineModel::FileDefineModel()
//-----------------------------------------------------------------------------
FileDefineModel::FileDefineModel(QSharedPointer<File> file, QObject* parent): 
QAbstractTableModel(parent), 
    defines_(file->getDefines())
{

}

//-----------------------------------------------------------------------------
// Function: FileDefineModel::~FileDefineModel()
//-----------------------------------------------------------------------------
FileDefineModel::~FileDefineModel()
{
}

//-----------------------------------------------------------------------------
// Function: FileDefineModel::rowCount()
//-----------------------------------------------------------------------------
int FileDefineModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return defines_->count();
}

//-----------------------------------------------------------------------------
// Function: FileDefineModel::columnCount()
//-----------------------------------------------------------------------------
int FileDefineModel::columnCount(QModelIndex const& parent) const 
{
	if (parent.isValid())
    {
		return 0;
	}

	return 4;
}

//-----------------------------------------------------------------------------
// Function: FileDefineModel::data()
//-----------------------------------------------------------------------------
QVariant FileDefineModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= defines_->count() || role != Qt::DisplayRole)
    {
		return QVariant();
	}

    switch (index.column())
    {
    case 0: 
        return defines_->at(index.row())->name();
    case 1:
        return defines_->at(index.row())->getValue();
    case 2:
        return defines_->at(index.row())->displayName();
    case 3:
        return defines_->at(index.row())->description();
    default:
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDefineModel::headerData()
//-----------------------------------------------------------------------------
QVariant FileDefineModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return tr("Name");
        case 1:
            return tr("Value");
        case 2:
            return tr("Display name");
        case 3:
            return tr("Description");
        default:
            return QVariant();
        }
    }
    else if (orientation == Qt::Vertical)
    {
        return section;
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDefineModel::setData()
//-----------------------------------------------------------------------------
bool FileDefineModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid())
    {
		return false;
	}

	if (role == Qt::EditRole)
    {
		switch (index.column())
        {
        case 0: 
            {
                defines_->at(index.row())->setName(value.toString());
                break;
            }
        case 1: 
            {
                defines_->at(index.row())->setValue(value.toString());
                break;
            }
        case 2: 
            {
                defines_->at(index.row())->setDisplayName(value.toString());
                break;
            }
        case 3: 
            {
                defines_->at(index.row())->setDescription(value.toString());
                break;
            }
        default:
            {
                return false;
            }
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
// Function: FileDefineModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FileDefineModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: FileDefineModel::insertRows()
//-----------------------------------------------------------------------------
bool FileDefineModel::insertRows(int row, int count, QModelIndex const& parent)
{
	// row has to be valid
	if (row < 0 || row > defines_->size() || parent.isValid())
    {
		return false;
	}

	beginInsertRows(parent, row, row + count -1);
	for (int i = 0; i < count; ++i)
    {
		defines_->insert(row, QSharedPointer<NameValuePair>(new NameValuePair()));
	}
	endInsertRows();
	emit contentChanged();
	return true;
}

//-----------------------------------------------------------------------------
// Function: FileDefineModel::removeRows()
//-----------------------------------------------------------------------------
bool FileDefineModel::removeRows(int row, int count, QModelIndex const& parent)
{
	// row has to be valid and there must be enough rows to remove
	if (row < 0 || row >= defines_->count() || (row + count >= defines_->count()) || parent.isValid())
    {
		return false;
	}

	beginRemoveRows(parent, row, row + count -1);
	// remove the right amount of rows
	for (int i = 0; i < count; ++i)
    {
		defines_->removeAt(row);
	}
	endInsertRows();
	emit contentChanged();
	return true;
}

//-----------------------------------------------------------------------------
// Function: FileDefineModel::isValid()
//-----------------------------------------------------------------------------
bool FileDefineModel::isValid() const
{
	// check all define items
	for (int i = 0; i < defines_->count(); ++i)
    {	
		// if one item has mandatory element missing
		if (defines_->at(i)->name().isEmpty() || defines_->at(i)->getValue().isEmpty())
        {
				return false;
		}
	}
	return true;
}
