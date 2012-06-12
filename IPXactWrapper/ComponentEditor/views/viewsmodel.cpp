/* 
 *  	Created on: 12.6.2012
 *      Author: Antti Kamppi
 * 		filename: viewsmodel.cpp
 *		Project: Kactus 2
 */

#include "viewsmodel.h"

#include <QStringList>
#include <QColor>

ViewsModel::ViewsModel(QSharedPointer<Component> component,
					   QObject *parent):
QAbstractTableModel(parent),
component_(component),
views_(component->getViews()) {

	Q_ASSERT(component);
}

ViewsModel::~ViewsModel() {
}

int ViewsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return views_.size();
}

int ViewsModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return ViewsModel::COLUMN_COUNT;
}

Qt::ItemFlags ViewsModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	// the type only displays the type of the view, it can not be edited.
	else if (index.column() == ViewsModel::TYPE_COLUMN) {
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}

	// other columns can be edited
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant ViewsModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case ViewsModel::NAME_COLUMN: {
				return tr("Name");
													 }
			case ViewsModel::TYPE_COLUMN: {
				return tr("View type");
														  }
			case ViewsModel::DESCRIPTION_COLUMN: {
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

QVariant ViewsModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= views_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case ViewsModel::NAME_COLUMN: {
				return views_.at(index.row())->getName();
													 }
			case ViewsModel::TYPE_COLUMN: {
				if (views_.at(index.row())->isHierarchical()) {
					return tr("hierarchical");
				}
				else {
					return tr("non-hierarchical");
				}
														  }
			case ViewsModel::DESCRIPTION_COLUMN: {
				return views_.at(index.row())->getDescription();
													  }
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {

		// file set names are needed to check that each view has valid references
		QStringList fileSetNames = component_->getFileSetNames();

		if (views_.at(index.row())->isValid(fileSetNames)) {
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

bool ViewsModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= views_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case ViewsModel::NAME_COLUMN: {
				views_[index.row()]->setName(value.toString());
				break;
													 }
			// type is not edited directly, it is only shown
			case ViewsModel::TYPE_COLUMN: {
				return false;
														  }
			case ViewsModel::DESCRIPTION_COLUMN: {
				views_[index.row()]->setDescription(value.toString());
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

bool ViewsModel::isValid() const {

	// file set names are needed to check that references within views are valid
	QStringList fileSetNames = component_->getFileSetNames();

	// if at least one view is invalid
	foreach (QSharedPointer<View> view, views_) {
		if (!view->isValid(fileSetNames)) {
			return false;
		}
	}
	// all views were valid
	return true;
}

void ViewsModel::onAddItem( const QModelIndex& index ) {
	int row = views_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	views_.insert(row, QSharedPointer<View>(new View()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit viewAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void ViewsModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= views_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	views_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit viewRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
