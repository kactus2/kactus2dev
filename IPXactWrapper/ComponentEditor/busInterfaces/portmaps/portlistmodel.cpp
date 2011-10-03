/* 
 *
 *  Created on: 10.5.2011
 *      Author: Antti Kamppi
 * 		filename: portlistmodel.cpp
 */

#include "portlistmodel.h"
#include "portmapsmodel.h"

#include <QDebug>

PortListModel::PortListModel(PortMapsModel* portMapsModel, QObject *parent): 
QAbstractListModel(parent), 
list_(),
portMapsModel_(portMapsModel) {
}

PortListModel::~PortListModel() {
}

int PortListModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	
	if (parent.isValid())
		return 0;
	return list_.size();
}

QVariant PortListModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole */ ) const {

	if (!index.isValid())
		return QVariant();
	else if (index.row() < 0 || index.row() >= list_.size())
		return QVariant();
	
	if (Qt::DisplayRole == role) {
		return list_.at(index.row());
	}
	// if unsupported role
	else
		return QVariant();
}

void PortListModel::addPort( const QString& portName ) {

	// if the port is already on the list
	if (list_.contains(portName))
		return;

	// add the item to the list
	beginInsertRows(QModelIndex(), list_.size(), list_.size());
	list_.append(portName);
	endInsertRows();
}

void PortListModel::removeItem( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	beginRemoveRows(QModelIndex(), index.row(), index.row());
	list_.removeAt(index.row());
	endRemoveRows();
}

void PortListModel::removeItems( const QModelIndexList& indexes ) {

	if (indexes.isEmpty())
		return;

	// first find the ports that were indexed
	QStringList ports;
	foreach (QModelIndex index, indexes) {

		// if index is not valid then proceed to next index.
		if (!index.isValid())
			continue;
		
		// if row is not valid
		else if (index.row() < 0 || index.row() >= list_.size())
			continue;

		// append the indexed port to the list
		ports.append(list_.at(index.row()));
	}

	beginResetModel();

	// remove all indexed ports from the list
	foreach (QString port, ports) {
		list_.removeAll(port);
	}
	endResetModel();
}

void PortListModel::removePorts( const QStringList& portList ) {

	beginResetModel();

	foreach (QString port, portList) {
		// if the port is on the list
		if (list_.contains(port))
			list_.removeAll(port);
	}
	endResetModel();
}

void PortListModel::removePort( const QString& port ) {

	beginResetModel();

	if (list_.contains(port)) {
		list_.removeAll(port);
	}
	endResetModel();
}

void PortListModel::onMoveItems( const QStringList& portNames, 
								const QModelIndex& targetIndex ) {

	bool append = false;
	if (portNames.isEmpty())
		return;
	else if (!targetIndex.isValid())
		append = true;

	QString targetItem;
	// find the target item so items can be inserted before it even when it's
	// index changes
	if (!append)
		targetItem = list_.at(targetIndex.row());

	beginResetModel();
	// move each item
	foreach (QString port, portNames) {
		
		// if the source is the same as target
		if (port == targetItem)
			continue;

		// first remove the port from the list
		list_.removeAll(port);

		int index = 0;
		// find the new place for the item
		if (!append)
			index = list_.indexOf(targetItem);
		else 
			index = list_.size();

		// insert the port to the correct location
		list_.insert(index, port);
	}
	endResetModel();
}
