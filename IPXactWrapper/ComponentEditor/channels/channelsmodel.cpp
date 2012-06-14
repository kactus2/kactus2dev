/* 
 *  	Created on: 14.6.2012
 *      Author: Antti Kamppi
 * 		filename: channelsmodel.cpp
 *		Project: Kactus 2
 */

#include "channelsmodel.h"
#include "channelsdelegate.h"

#include <QStringList>
#include <QColor>

ChannelsModel::ChannelsModel( QSharedPointer<Component> component, 
							 QObject *parent ):
QAbstractTableModel(parent),
component_(component),
channels_(component->getChannels()) {

	Q_ASSERT(component);
}

ChannelsModel::~ChannelsModel() {
}

int ChannelsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return channels_.size();
}

int ChannelsModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return ChannelsDelegate::COLUMN_COUNT;
}

Qt::ItemFlags ChannelsModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant ChannelsModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case ChannelsDelegate::NAME_COLUMN: {
				return tr("Name");
													 }
			case ChannelsDelegate::DISPLAY_NAME_COLUMN: {
				return tr("Display name");
														  }
			case ChannelsDelegate::INTERFACE_COLUMN: {
				return tr("Interface\nreferences");
													  }
			case ChannelsDelegate::DESCRIPTION_COLUMN: {
				return tr("Description");
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

QVariant ChannelsModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= channels_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case ChannelsDelegate::NAME_COLUMN: {
				return channels_.at(index.row())->getName();
													 }
			case ChannelsDelegate::DISPLAY_NAME_COLUMN: {
				return channels_.at(index.row())->getDisplayName();
														  }
			case ChannelsDelegate::INTERFACE_COLUMN: {
				// each group name if in it's own index
				QStringList interfaceNames = channels_.at(index.row())->getInterfaces();

				// concatenate the names to a single string with spaces in between
				QString str;
				foreach (QString interfaceName, interfaceNames) {
					str += interfaceName;
					str += " ";
				}

				// return the string with all group names
				return str;
													  }
			case ChannelsDelegate::DESCRIPTION_COLUMN: {
				return channels_.at(index.row())->getDescription();
													  }
			default: {
				return QVariant();
					 }
		}
	}
	// user display role for interface column returns a QStringList
	else if (ChannelsDelegate::USER_DISPLAY_ROLE == role && 
		index.column() == ChannelsDelegate::INTERFACE_COLUMN) {
		return channels_.at(index.row())->getInterfaces();
	}
	else if (Qt::ForegroundRole == role) {

		// interface names are needed to check that references to bus interfaces are valid
		QStringList interfaceNames = component_->getBusInterfaceNames();

		if (channels_.at(index.row())->isValid(interfaceNames)) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else {
		return QVariant();
	}
}

bool ChannelsModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= channels_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case ChannelsDelegate::NAME_COLUMN: {
				channels_[index.row()]->setName(value.toString());
				break;
													 }
			case ChannelsDelegate::DISPLAY_NAME_COLUMN: {
				channels_[index.row()]->setDisplayName(value.toString());
				break;
														  }
			case ChannelsDelegate::INTERFACE_COLUMN: {
				QString str = value.toString();
				QStringList interfaceNames = str.split(' ', QString::SkipEmptyParts);
				channels_[index.row()]->setInterfaces(interfaceNames);
				break;
													  }
			case ChannelsDelegate::DESCRIPTION_COLUMN: {
				channels_[index.row()]->setDescription(value.toString());
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
	// user edit role for interface column operates on QStringList
	else if (ChannelsDelegate::USER_EDIT_ROLE == role &&
		ChannelsDelegate::INTERFACE_COLUMN == index.column()) {
		channels_[index.row()]->setInterfaces(value.toStringList());
		emit dataChanged(index, index);
		emit contentChanged();
	}
	else {
		return false;
	}
}

void ChannelsModel::onAddItem( const QModelIndex& index ) {
	int row = channels_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	channels_.insert(row, QSharedPointer<Channel>(new Channel()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit channelAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void ChannelsModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= channels_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	channels_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit channelRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

bool ChannelsModel::isValid() const {

	// interface names are needed to check that references to bus interfaces are valid
	QStringList interfaceNames = component_->getBusInterfaceNames();

	// if at least one address space is invalid
	foreach (QSharedPointer<Channel> channel, channels_) {
		if (!channel->isValid(interfaceNames)) {
			return false;
		}
	}
	// all address spaces were valid
	return true;
}
