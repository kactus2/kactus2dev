/* 
 *
 *  Created on: 18.4.2011
 *      Author: Antti Kamppi
 * 		filename: envidentifiersmodel.cpp
 */

#include "envidentifiersmodel.h"

#include <models/view.h>

EnvIdentifiersModel::EnvIdentifiersModel(View* view, 
										 QObject *parent ):
QAbstractTableModel(parent), 
view_(view),
table_() {

	Q_ASSERT_X(view_, "EnvIdentifiersModel constructor",
		"Null view pointer given as parameter");

	restore();
}

EnvIdentifiersModel::~EnvIdentifiersModel() {
}

int EnvIdentifiersModel::rowCount(const QModelIndex& parent /*= QModelIndex() */ ) const {
	if (parent.isValid())
		return 0;

	return table_.size();
}

int EnvIdentifiersModel::columnCount( const QModelIndex& parent /*= QModelIndex() */ ) const {

	if (parent.isValid())
		return 0;
	return 3;
}

QVariant EnvIdentifiersModel::data( const QModelIndex& index, 
								   int role /*= Qt::DisplayRole */ ) const {

	if (!index.isValid())
		return QVariant();

	// if row is invalid
	else if (index.row() < 0 || index.row() >= table_.size())
		return QVariant();

	if (Qt::DisplayRole == role) {

		// split the identifier
		QStringList fields = table_.at(index.row()).split(":");

		// and return the correct field
		return fields.value(index.column());
		
	}

	// if unsupported role
	else {
		return QVariant();
	}
}

QVariant EnvIdentifiersModel::headerData( int section, 
										 Qt::Orientation orientation, 
										 int role /*= Qt::DisplayRole */ ) const {
	if (orientation != Qt::Horizontal)
	 return QVariant();
	
	if (role == Qt::DisplayRole) {

		switch (section) {
			case 0: 
				return tr("Language");
			case 1:
				return tr("Tool");
			case 2:
				return tr("Vendor specific");
			default:
				return QVariant();
		}
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

bool EnvIdentifiersModel::setData( const QModelIndex& index,
								  const QVariant& value, 
								  int role /*= Qt::EditRole */ ) {

	if (!index.isValid())
		return false;

	// if row is invalid
	else if (index.row() < 0 || index.row() >= table_.size())
		return false;

	if (Qt::EditRole == role) {

		QStringList identifier = table_.at(index.row()).split(":");
		identifier.replace(index.column(), value.toString());
		QString result = identifier.value(0);
		result += QString(":");
		result += identifier.value(1);
		result += QString(":");
		result += identifier.value(2);
		table_.replace(index.row(), result);
		
		emit contentChanged();
		return true;
	}
	// if unsupported role
	else
		return false;
}

Qt::ItemFlags EnvIdentifiersModel::flags( const QModelIndex& index ) const {

	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool EnvIdentifiersModel::isValid() const {

	// at least one has to be specified.
	return !table_.isEmpty();
}

void EnvIdentifiersModel::apply() {
	view_->setEnvIdentifiers(table_);
}

void EnvIdentifiersModel::restore() {

	beginResetModel();
	table_.clear();
	table_ = view_->getEnvIdentifiers();
	endResetModel();
}

void EnvIdentifiersModel::onRemoveRow( int row ) {
	// if row is invalid
	if (row < 0 || row >= table_.size())
		return;

	beginRemoveRows(QModelIndex(), row, row);

	// remove the object from the map
	table_.removeAt(row);

	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void EnvIdentifiersModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	table_.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void EnvIdentifiersModel::onAddRow() {
	beginInsertRows(QModelIndex(), table_.size(), table_.size());

	table_.append(QString("::"));

	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void EnvIdentifiersModel::onAddItem( const QModelIndex& index ) {
	int row = table_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	table_.insert(row, QString("::"));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
