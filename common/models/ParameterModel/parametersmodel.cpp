/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: parametersmodel.cpp
 */

#include "parametersmodel.h"

#include <models/component.h>

#include <QColor>

ParametersModel::ParametersModel(QList<QSharedPointer<Parameter> >* parameters,
								 QObject *parent): 
QAbstractTableModel(parent), 
parameters_(parameters), 
table_() {

	Q_ASSERT_X(parameters, "ParametersModel constructor",
		"Invalid Component-pointer given as parameter");

	restore();
}

ParametersModel::~ParametersModel() {
}

int ParametersModel::rowCount(const QModelIndex& parent /*= QModelIndex() */ ) const {

	if (parent.isValid())
		return 0;

	return table_.size();
}

int ParametersModel::columnCount( const QModelIndex& parent /*= QModelIndex() */ ) const {

	if (parent.isValid())
		return 0;

	return 3;
}

QVariant ParametersModel::data( const QModelIndex& index, 
							   int role /*= Qt::DisplayRole */ ) const {

	if (!index.isValid())
		return QVariant();

	// if row is invalid
	else if (index.row() < 0 || index.row() >= table_.size())
		return QVariant();

	if (role == Qt::DisplayRole) {

		switch (index.column()) {
			case 0: {
				return table_.at(index.row())->getName();
					}
			case 1: {
				return table_.at(index.row())->getValue();
					}
			case 2: {
				return table_.at(index.row())->getDescription();
					}
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case 0:
			case 1: {
				return QColor("LemonChiffon");
					}
			default:
				return QColor("white");
		}
	}
	else if (Qt::ForegroundRole == role) {
		if (table_.at(index.row())->isValid()) {
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

QVariant ParametersModel::headerData( int section, 
									 Qt::Orientation orientation, 
									 int role /*= Qt::DisplayRole */ ) const {

	if (orientation != Qt::Horizontal)
		return QVariant();

	if (role == Qt::DisplayRole) {
		switch (section) {
			case 0:
				return tr("Name");
			case 1:
				return tr("Value");
			case 2:
				return tr("Description");
			default:
				return QVariant();
		}
	}

	// if unsupported role
	else {
		return QVariant();
	}
}

bool ParametersModel::setData( const QModelIndex& index, 
							  const QVariant& value, 
							  int role /*= Qt::EditRole */ ) {

	if (!index.isValid())
		return false;

	// if row is invalid
	else if (index.row() < 0 || index.row() >= table_.size())
		return false;

	if (role == Qt::EditRole) {
		
		switch (index.column()) {
			case 0: {
				table_.value(index.row())->setName(value.toString());
				break;
					}
			case 1: {
				table_.value(index.row())->setValue(value.toString());
				break;
					}
			case 2: {
				table_.value(index.row())->setDescription(value.toString());
				break;
					}
			default: 
				return false;
		}
		emit contentChanged();
		return true;
	}

	// is unsupported role
	else {
		return false;
	}
}

Qt::ItemFlags ParametersModel::flags(const QModelIndex& index ) const {

	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool ParametersModel::isValid() const {

	// check all parameters
	foreach (QSharedPointer<Parameter> parameter, table_) {

		// if one parameter is invalid
		if (!parameter->isValid())
			return false;
	}

	// all parameters are valid
	return true;
}

void ParametersModel::apply() {
	*parameters_ = table_;
}

void ParametersModel::restore() {
	beginResetModel();
	table_.clear();
	table_ = *parameters_;
	endResetModel();
}

void ParametersModel::onRemoveRow( int row ) {

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

void ParametersModel::onRemoveItem( const QModelIndex& index ) {
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

void ParametersModel::onAddRow() {

	beginInsertRows(QModelIndex(), table_.size(), table_.size());

	table_.append(QSharedPointer<Parameter>(new Parameter()));

	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void ParametersModel::onAddItem( const QModelIndex& index ) {
	int row = table_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	table_.insert(row, QSharedPointer<Parameter>(new Parameter()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
