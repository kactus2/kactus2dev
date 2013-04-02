/* 
 *	Created on:	2.4.2013
 *	Author:		Antti Kamppi
 *	File name:	swbuildcommandmodel.cpp
 *	Project:		Kactus 2
*/

#include "swbuildcommandmodel.h"
#include "swbuilddelegate.h"

SWBuildCommandModel::SWBuildCommandModel(QSharedPointer<Component> component,
	QList<QSharedPointer<SWBuildCommand> >& swBuildCommands, 
	QObject *parent):
QAbstractTableModel(parent),
component_(component),
swBuildComs_(swBuildCommands) {

}

SWBuildCommandModel::~SWBuildCommandModel() {
}

int SWBuildCommandModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}

	return swBuildComs_.count();
}

int SWBuildCommandModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}

	return SWBuildDelegate::COLUMN_COUNT;
}

Qt::ItemFlags SWBuildCommandModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	// the replace default column is boolean and uses check box
	if (index.column() == SWBuildDelegate::REPLACE_DEF_COLUMN) {
		flags |= Qt::ItemIsUserCheckable;
	}
	// others can be freely edited by user
	else {
		flags |= Qt::ItemIsEditable;
	}

	return flags;
}

QVariant SWBuildCommandModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (section) {
		case SWBuildDelegate::FILETYPE_COLUMN:
			return tr("File type");
		case SWBuildDelegate::COMMAND_COLUMN:
			return tr("Command");
		case SWBuildDelegate::FLAGS_COLUMN:
			return tr("Flags");
		case SWBuildDelegate::REPLACE_DEF_COLUMN:
			return tr("Replace default flags");
		default:
			return QVariant();
		}
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

QVariant SWBuildCommandModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}

	// if row is invalid
	else if (index.row() < 0 || index.row() >= swBuildComs_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
		case SWBuildDelegate::FILETYPE_COLUMN: {
			return swBuildComs_.at(index.row())->getFileType();
															}
		case SWBuildDelegate::COMMAND_COLUMN: {
			return swBuildComs_.at(index.row())->getCommand();
														  }
		case SWBuildDelegate::FLAGS_COLUMN: {
			return swBuildComs_.at(index.row())->getFlags();
														}
		case SWBuildDelegate::REPLACE_DEF_COLUMN: {
			return swBuildComs_.at(index.row())->getReplaceDefaultFlags();
																}
		default: {
			return QVariant();
					}
		}
	}
	else if (Qt::CheckStateRole == role) {
		if (index.column() == SWBuildDelegate::REPLACE_DEF_COLUMN) {
			if (swBuildComs_.at(index.row())->getReplaceDefaultFlags()) {
				return Qt::Checked;
			}
			else {
				return Qt::Unchecked;
			}
		}
		else {
			return QVariant();
		}
	}
	else if (Qt::BackgroundRole == role) {
		if (index.column() == SWBuildDelegate::FILETYPE_COLUMN) {
			return QColor("LemonChiffon");
		}
		else {
			return QColor("white");
		}
	}
	else if (Qt::ForegroundRole == role) {
		if (swBuildComs_.at(index.row())->isValid()) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

bool SWBuildCommandModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}

	// if row is invalid
	else if (index.row() < 0 || index.row() >= swBuildComs_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
		case SWBuildDelegate::FILETYPE_COLUMN: {
			swBuildComs_[index.row()]->setFileType(value.toString());
			break;
															}
		case SWBuildDelegate::COMMAND_COLUMN: {
			swBuildComs_[index.row()]->setCommand(value.toString());
			break;
														  }
		case SWBuildDelegate::FLAGS_COLUMN: {
			swBuildComs_[index.row()]->setFlags(value.toString());
			break;
														}
		case SWBuildDelegate::REPLACE_DEF_COLUMN: {
			swBuildComs_[index.row()]->setReplaceDefaultFlags(value.toBool());
			break;
																}
		default: {
			return false;
					}
		}

		emit contentChanged();
		emit dataChanged(index, index);
		return true;
	}
	else if (role == Qt::CheckStateRole) {
		swBuildComs_.at(index.row())->setReplaceDefaultFlags(value == Qt::Checked);
		emit dataChanged(index, index);
		return true;
	}
	else {
		return false;
	}
}

bool SWBuildCommandModel::isValid() const {

	// if at least one SW build command is invalid
	foreach (QSharedPointer<SWBuildCommand> buildCom, swBuildComs_) {
		if (!buildCom->isValid()) {
			return false;
		}
	}

	// all were valid
	return true;
}

void SWBuildCommandModel::onAddItem( const QModelIndex& index ) {
	int row = swBuildComs_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	swBuildComs_.insert(row, QSharedPointer<SWBuildCommand>(new SWBuildCommand()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void SWBuildCommandModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= swBuildComs_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	swBuildComs_.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
