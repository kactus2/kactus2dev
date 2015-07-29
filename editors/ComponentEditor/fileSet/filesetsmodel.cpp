//-----------------------------------------------------------------------------
// File: filesetsmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.5.2012
//
// Description:
// The model class to manage the objects for FileSetsEditor.
//-----------------------------------------------------------------------------

#include "filesetsmodel.h"

#include <QColor>
#include <QStringList>
#include <QString>
#include <QRegularExpression>

namespace
{
    enum FileSetColumns
    {
        NAME_COLUMN = 0,
        GROUP_COLUMN,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::FileSetsModel()
//-----------------------------------------------------------------------------
FileSetsModel::FileSetsModel(QSharedPointer<Component> component, 
							 QObject *parent):
QAbstractTableModel(parent),
component_(component),
fileSets_(component->getFileSets())
{

}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::~FileSetsModel()
//-----------------------------------------------------------------------------
FileSetsModel::~FileSetsModel()
{
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::rowCount()
//-----------------------------------------------------------------------------
int FileSetsModel::rowCount(QModelIndex const& parent /*= QModelIndex()*/ ) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return fileSets_.size();
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::columnCount()
//-----------------------------------------------------------------------------
int FileSetsModel::columnCount(QModelIndex const& parent /*= QModelIndex()*/ ) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FileSetsModel::flags(QModelIndex const& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::headerData()
//-----------------------------------------------------------------------------
QVariant FileSetsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (section == NAME_COLUMN)
        {
            return tr("Name");
        }
        else if (section == GROUP_COLUMN)
        {
            return tr("Group identifiers");
        }
        else if (section == DESCRIPTION)
        {
            return tr("Description");
        }
        else
        {
            return QVariant();
        }
	}
	else
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::data()
//-----------------------------------------------------------------------------
QVariant FileSetsModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= fileSets_.size())
    {
		return QVariant();
	}

    QSharedPointer<FileSet> fileSet = fileSets_.at(index.row());
    if (role == Qt::DisplayRole)
    {
        if (index.column() == NAME_COLUMN)
        {
            return fileSet->getName();
        }
        else if (index.column() == DESCRIPTION)
        {
            return fileSet->getDescription().replace(QRegularExpression("\n.*$", 
                QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else if (index.column() == GROUP_COLUMN)
        {
            return fileSet->getGroups().join(" ");
        }
        else
        {
            return QVariant();
        }
    }
    else if ((role == Qt::EditRole || role == Qt::ToolTipRole) && index.column() == DESCRIPTION)
    {
        return fileSet->getDescription();
    }
	else if (role == Qt::ForegroundRole)
    {
		if (fileSet->isValid(false))
        {
			return QColor("black");
		}
		else
        {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role)
    {
        if (index.column() == NAME_COLUMN)
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
// Function: FileSetsModel::setData()
//-----------------------------------------------------------------------------
bool FileSetsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= fileSets_.size())
    {
		return false;
	}

    QSharedPointer<FileSet> fileSet = fileSets_[index.row()];

    if (Qt::EditRole == role)
    {
        if (index.column() == NAME_COLUMN)
        {
            fileSet->setName(value.toString());
        }
        else if (index.column() == DESCRIPTION)
        {
            fileSet->setDescription(value.toString());
        }
        else if(index.column() == GROUP_COLUMN)
        {
            QString str = value.toString();
            QStringList groupNames = str.split(' ', QString::SkipEmptyParts);
            fileSet->setGroups(groupNames);
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
// Function: FileSetsModel::onAddItem()
//-----------------------------------------------------------------------------
void FileSetsModel::onAddItem(QModelIndex const& index)
{
	int row = fileSets_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	fileSets_.insert(row, QSharedPointer<FileSet>(new FileSet()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit fileSetAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void FileSetsModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= fileSets_.size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	fileSets_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit fileSetRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::isValid()
//-----------------------------------------------------------------------------
bool FileSetsModel::isValid() const
{
	// if at least one file set is invalid
	foreach (QSharedPointer<FileSet> fileSet, fileSets_)
    {
		if (!fileSet->isValid(true))
        {
			return false;
		}
	}
	// all file sets valid
	return true;
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::refresh()
//-----------------------------------------------------------------------------
void FileSetsModel::refresh()
{
    beginResetModel();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::onFileSetAdded()
//-----------------------------------------------------------------------------
void FileSetsModel::onFileSetAdded(FileSet* fileSet)
{
    beginResetModel();
    endResetModel();

    // Find out the corresponding index and signal fileSetAdded().
    for (int i = 0; i < fileSets_.size(); ++i)
    {
        if (fileSets_[i].data() == fileSet)
        {
            emit fileSetAdded(i);
            emit contentChanged();
            break;
        }
    }
}
