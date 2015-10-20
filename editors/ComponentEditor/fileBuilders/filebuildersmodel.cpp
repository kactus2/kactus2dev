//-----------------------------------------------------------------------------
// File: filebuildersmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.05.2012
//
// Description:
// Model that contains the items to edit file builders.
//-----------------------------------------------------------------------------

#include "filebuildersmodel.h"
#include "FileBuilderColumns.h"

#include <IPXACTmodels/common/FileBuilder.h>

#include <QColor>

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::FileBuildersModel()
//-----------------------------------------------------------------------------
FileBuildersModel::FileBuildersModel(QSharedPointer<QList<QSharedPointer<FileBuilder> > > fileBuilders,
                                     QObject* parent):
QAbstractTableModel(parent),
fileBuilders_(fileBuilders)
{

}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::~FileBuildersModel()
//-----------------------------------------------------------------------------
FileBuildersModel::~FileBuildersModel()
{

}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::rowCount()
//-----------------------------------------------------------------------------
int FileBuildersModel::rowCount( const QModelIndex& parent /*= QModelIndex() */ ) const
{
	if (parent.isValid())
    {
		return 0;
	}
    return fileBuilders_->size();
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::columnCount()
//-----------------------------------------------------------------------------
int FileBuildersModel::columnCount( const QModelIndex& parent /*= QModelIndex() */ ) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return 4;
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::data()
//-----------------------------------------------------------------------------
QVariant FileBuildersModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole */ ) const
{
	if (!index.isValid())
    {
		return QVariant();
	}

	// if row is invalid
	else if (index.row() < 0 || index.row() >= fileBuilders_->size())
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
        {
            return fileBuilders_->at(index.row())->getFileType();
        }
        else if (index.column() == FileBuilderColumns::COMMAND_COLUMN)
        {
            return fileBuilders_->at(index.row())->getCommand();
        }
        else if (index.column() == FileBuilderColumns::FLAGS_COLUMN)
        {
            return fileBuilders_->at(index.row())->getFlags();
        }
        else if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
        {
            return fileBuilders_->at(index.row())->getReplaceDefaultFlags();
        }
	}
	else if (Qt::BackgroundRole == role)
    {
		if (index.column() == 0)
        {
			return QColor("LemonChiffon");
		}
		else
        {
			return QColor("white");
		}
	}
	else if (Qt::ForegroundRole == role)
    {
// 		if (fileBuilders_->at(index.row())->isValid())
//         {
			return QColor("black");
// 		}
// 		else
//         {
// 			return QColor("red");
// 		}
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::headerData()
//-----------------------------------------------------------------------------
QVariant FileBuildersModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */ )
    const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (section == FileBuilderColumns::FILETYPE_COLUMN)
        {
            return tr("File type");
        }
        else if (section == FileBuilderColumns::COMMAND_COLUMN)
        {
            return tr("Command");
        }
        else if (section == FileBuilderColumns::FLAGS_COLUMN)
        {
            return tr("Flags");
        }
        else if (section == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
        {
            return tr("Replace default flags");
        }
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::setData()
//-----------------------------------------------------------------------------
bool FileBuildersModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole */ )
{
	if (!index.isValid())
    {
		return false;
	}

	// if row is invalid
	else if (index.row() < 0 || index.row() >= fileBuilders_->size())
    {
		return false;
	}

	if (Qt::EditRole == role)
    {
        if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
        {
            fileBuilders_->at(index.row())->setFileType(value.toString());
        }
        else if (index.column() == FileBuilderColumns::COMMAND_COLUMN)
        {
            fileBuilders_->at(index.row())->setCommand(value.toString());
        }
        else if (index.column() == FileBuilderColumns::FLAGS_COLUMN)
        {
            fileBuilders_->at(index.row())->setFlags(value.toString());
        }
        else if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
        {
            fileBuilders_->at(index.row())->setReplaceDefaultFlags(value.toString());
        }

		emit contentChanged();
		emit dataChanged(index, index);
		return true;
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FileBuildersModel::flags( const QModelIndex& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}
	
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	// the replace default column is boolean and uses check box
	if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
    {
		flags |= Qt::ItemIsUserCheckable;
	}
	// others can be freely edited by user
	else
    {
		flags |= Qt::ItemIsEditable;
	}

	return flags;
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::isValid()
//-----------------------------------------------------------------------------
bool FileBuildersModel::isValid() const
{
	// check all file builders
	foreach (QSharedPointer<FileBuilder> fileBuilder, *fileBuilders_)
    {
		// if one is invalid
// 		if (!fileBuilder->isValid()) {
// 			return false;
// 		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::onRemoveItem()
//-----------------------------------------------------------------------------
void FileBuildersModel::onRemoveItem( const QModelIndex& index )
{
	// don't remove anything if index is invalid
	if (!index.isValid())
    {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= fileBuilders_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	fileBuilders_->removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::onAddItem()
//-----------------------------------------------------------------------------
void FileBuildersModel::onAddItem( const QModelIndex& index )
{
	int row = fileBuilders_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	fileBuilders_->insert(row, QSharedPointer<FileBuilder>(new FileBuilder()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
