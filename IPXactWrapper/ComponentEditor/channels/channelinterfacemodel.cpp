/* 
 *  	Created on: 7.2.2012
 *      Author: Antti Kamppi
 * 		filename: channelinterfacemodel.cpp
 *		Project: Kactus 2
 */

#include "channelinterfacemodel.h"

#include <QColor>

ChannelInterfaceModel::ChannelInterfaceModel( QObject *parent,
											 QSharedPointer<Component> component,
											 const QStringList& items /*= QStringList()*/ ):
ListManagerModel(parent, items),
component_(component) {
}

ChannelInterfaceModel::~ChannelInterfaceModel() {
}

QVariant ChannelInterfaceModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	// nothing for invalid indexes
	if (!index.isValid()) {
		return QVariant();
	}

	// if index.row() is invalid
	else if (index.row() < 0 || index.row() >= items_.size()) {
		return QVariant();
	}

	// return data for display role
	if (role == Qt::DisplayRole) {
		return items_.at(index.row());
	}
	else if (role == Qt::ForegroundRole) {

		// if component contains the named file set.
		if (component_->hasInterface(items_.at(index.row()))) {
			return QColor("black");
		}

		// if the file set does not exist.
		else {
			return QColor("red");
		}
	}
	// if unsupported role
	else {
		return QVariant();
	}
}
