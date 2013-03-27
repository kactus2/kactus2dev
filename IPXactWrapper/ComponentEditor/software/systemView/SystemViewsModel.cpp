//-----------------------------------------------------------------------------
// File: SystemViewsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The model to manage system views for a table view.
//-----------------------------------------------------------------------------

#include "SystemViewsModel.h"
#include "SystemViewsDelegate.h"
#include <LibraryManager/vlnv.h>

#include <QColor>

SystemViewsModel::SystemViewsModel(QSharedPointer<Component> component,
						   QObject *parent):
QAbstractTableModel(parent),
views_(component->getSystemViews()),
component_(component) {

	Q_ASSERT(component_);
}

SystemViewsModel::~SystemViewsModel() {

}

int SystemViewsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return views_.size();
}

int SystemViewsModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return SystemViewsDelegate::COLUMN_COUNT;
}

Qt::ItemFlags SystemViewsModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	// hierarchy reference can not be edited
	else if (SystemViewsDelegate::HIER_REF_COLUMN == index.column()) {
			return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant SystemViewsModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case SystemViewsDelegate::NAME_COLUMN: {
				return tr("Name");
													 }
			case SystemViewsDelegate::HIER_REF_COLUMN: {
				return tr("Hierarchy reference");
													   }
			case SystemViewsDelegate::DISPLAY_NAME_COLUMN: {
				return tr("Display name");
													   }
			case SystemViewsDelegate::DESCRIPTION_COLUMN: {
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

QVariant SystemViewsModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= views_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case SystemViewsDelegate::NAME_COLUMN: {
				return views_.at(index.row())->getName();
													 }
			case SystemViewsDelegate::HIER_REF_COLUMN: {
				return views_.at(index.row())->getHierarchyRef().toString(":");
													   }
			case SystemViewsDelegate::DISPLAY_NAME_COLUMN: {
				return views_.at(index.row())->getDisplayName();
													   }
			case SystemViewsDelegate::DESCRIPTION_COLUMN: {
				return views_.at(index.row())->getDescription();
														}
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {

		QStringList fileSetNames = component_->getFileSetNames();

		if (views_.at(index.row())->isValid(fileSetNames)) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case SystemViewsDelegate::NAME_COLUMN:
			case SystemViewsDelegate::HIER_REF_COLUMN: {
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

bool SystemViewsModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= views_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case SystemViewsDelegate::NAME_COLUMN: {
				views_[index.row()]->setName(value.toString());
				break;
													 }
			case SystemViewsDelegate::HIER_REF_COLUMN: {
				VLNV hierRef = VLNV(VLNV::DESIGNCONFIGURATION, value.toString(), ":");
				views_[index.row()]->setHierarchyRef(hierRef);
				break;
													   }
			case SystemViewsDelegate::DISPLAY_NAME_COLUMN: {
				views_[index.row()]->setDisplayName(value.toString());
				break;
													   }
			case SystemViewsDelegate::DESCRIPTION_COLUMN: {
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

bool SystemViewsModel::isValid() const {

	QStringList fileSetNames = component_->getFileSetNames();

	// check that each software view is valid
	foreach (QSharedPointer<SystemView> swView, views_) {
		if (!swView->isValid(fileSetNames)) {
			return false;
		}
	}
	return true;
}

void SystemViewsModel::onAddItem( const QModelIndex& index ) {
	int row = views_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	views_.insert(row, QSharedPointer<SystemView>(new SystemView()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit viewAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void SystemViewsModel::onRemoveItem( const QModelIndex& index ) {
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
