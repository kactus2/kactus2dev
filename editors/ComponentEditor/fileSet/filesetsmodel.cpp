/* 
 *  	Created on: 25.5.2012
 *      Author: Antti Kamppi
 * 		filename: filesetsmodel.cpp
 *		Project: Kactus 2
 */

#include "filesetsmodel.h"

#include <QStringList>
#include <QString>
#include <QColor>

FileSetsModel::FileSetsModel(QSharedPointer<Component> component, 
							 QObject *parent):
QAbstractTableModel(parent),
component_(component),
fileSets_(component->getFileSets()) {

}

FileSetsModel::~FileSetsModel() {
}

int FileSetsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return fileSets_.size();
}

int FileSetsModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return FileSetsModel::COLUMN_COUNT;
}

Qt::ItemFlags FileSetsModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant FileSetsModel::headerData( int section,
								   Qt::Orientation orientation,
								   int role /*= Qt::DisplayRole*/ ) const {

	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case 0: {
				return tr("Name");
					}
			case 1: {
				return tr("Description");
					}
			case 2: {
				return tr("Group identifiers");
					}
			default: {
				return QVariant();
					 }
		}
	}
	else {
		return QVariant();
	}
}

QVariant FileSetsModel::data( const QModelIndex& index, 
							 int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= fileSets_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case 0: {
				return fileSets_.at(index.row())->getName();
					}
			case 1: {
				return fileSets_.at(index.row())->getDescription();
					}
			case 2: {
				// each group name if in it's own index
				QStringList groupNames = fileSets_.at(index.row())->getGroups();

				// concatenate the names to a single string with spaces in between
				QString str;
				foreach (QString groupName, groupNames) {
					str += groupName;
					str += " ";
				}

				// return the string with all group names
				return str;
					}
			default: {
				return QVariant();
					 }
		}
	}
	// for user role a QStringList is returned
	else if (FileSetsModel::USER_DISPLAY_ROLE == role && index.column() == 2) {
		return fileSets_.at(index.row())->getGroups();
	}
	else if (Qt::ForegroundRole == role) {
		if (fileSets_.at(index.row())->isValid(false)) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case 0: {
				return QColor("LemonChiffon");
					}
			default:
				return QColor("white");
		}
	}
	else {
		return QVariant();
	}
}

bool FileSetsModel::setData( const QModelIndex& index,
							const QVariant& value,
							int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= fileSets_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {
		
		switch (index.column()) {
			case 0: {
				fileSets_[index.row()]->setName(value.toString());
				break;
					}
			case 1: {
				fileSets_[index.row()]->setDescription(value.toString());
				break;
					}
			case 2: {
				QString str = value.toString();
				QStringList groupNames = str.split(' ', QString::SkipEmptyParts);
				fileSets_[index.row()]->setGroups(groupNames);
				break;
					}
			default: {
				return false;
					 }
		}

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	// for user edit role a QStringList is used
	else if (FileSetsModel::USER_EDIT_ROLE == role && index.column() == 2) {
		fileSets_[index.row()]->setGroups(value.toStringList());
		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	else {
		return false;
	}
}

void FileSetsModel::onAddItem( const QModelIndex& index ) {
	int row = fileSets_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
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

void FileSetsModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= fileSets_.size()) {
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

bool FileSetsModel::isValid() const {
	
	// if at least one file set is invalid
	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		if (!fileSet->isValid(true)) {
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
