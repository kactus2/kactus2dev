/* 
 *  	Created on: 25.5.2012
 *      Author: Antti Kamppi
 * 		filename: filesetsmodel.cpp
 *		Project: Kactus 2
 */

#include "filesetsmodel.h"

#include <QStringList>
#include <QString>

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
	else {
		return false;
	}
}
