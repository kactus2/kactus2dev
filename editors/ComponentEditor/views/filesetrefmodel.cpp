/* 
 *  	Created on: 7.2.2012
 *      Author: Antti Kamppi
 * 		filename: filesetrefmodel.cpp
 *		Project: Kactus 2
 */

#include "filesetrefmodel.h"

#include <QColor>

FileSetRefModel::FileSetRefModel(QObject *parent, 
								 QSharedPointer<Component> component,
								 const QStringList& items):
ListManagerModel(parent, items),
component_(component) {
}

FileSetRefModel::~FileSetRefModel() {
}

QVariant FileSetRefModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	// nothing for invalid indexes
	if (!index.isValid()) {
		return QVariant();
	}

	// if there are no real items then display the help text
	else if (index.row() == 0 && items_.isEmpty()) {
		switch (role) {
			case Qt::DisplayRole: {
				return tr("Double click to add new item.");
								  }
			case Qt::ForegroundRole: {
				return QColor("silver");
									 }
			default: {
				return QVariant();
					 }
		}
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
		if (component_->hasFileSet(items_.at(index.row()))) {
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
