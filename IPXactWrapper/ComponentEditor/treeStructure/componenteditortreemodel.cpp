/* 
 *  	Created on: 7.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditortreemodel.cpp
 *		Project: Kactus 2
 */

#include "componenteditortreemodel.h"

#include <QColor>

ComponentEditorTreeModel::ComponentEditorTreeModel(QObject *parent):
QAbstractItemModel(parent) {
}

ComponentEditorTreeModel::~ComponentEditorTreeModel() {
}

int ComponentEditorTreeModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	// only one column exists
	if (parent.column() > 0) {
		return 0;
	}

	ComponentEditorItem* parentItem = 0;

	// if the given item is invalid, it is interpreted as root item
	if (!parent.isValid()) {
		parentItem = rootItem_.data();
	}
	else {
		parentItem = static_cast<ComponentEditorItem*>(
			parent.internalPointer());
	}

	// return how many children the parent has
	return parentItem->rowCount();
}

int ComponentEditorTreeModel::columnCount( const QModelIndex&) const {
	return 1;
}

QVariant ComponentEditorTreeModel::data( const QModelIndex& index, 
										int role /*= Qt::DisplayRole*/ ) const {

	if (!index.isValid()) {
		return QVariant();
	}

	ComponentEditorItem* item = static_cast<ComponentEditorItem*>(
		index.internalPointer());
	Q_ASSERT(item);

	if (role == Qt::DisplayRole) {

		QString text = item->text();

		// if the object has not a name
		if (text.isEmpty()) {
			text = tr("unnamed");
		}

		return text;
	}
	else if (role == Qt::FontRole) {
		return item->getFont();
	}
	else if (role == Qt::ForegroundRole) {

		if (item->isValid())
			return QColor("black");
		else
			return QColor("red");
	}
	// not supported role
	else {
		return QVariant();
	}
}

bool ComponentEditorTreeModel::setData( const QModelIndex& index, 
									   const QVariant& value, int role /*= Qt::EditRole*/ ) {
	return false;
}

Qt::ItemFlags ComponentEditorTreeModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
