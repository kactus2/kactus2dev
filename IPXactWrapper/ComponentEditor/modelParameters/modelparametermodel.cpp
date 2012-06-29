/* 
 *
 *  Created on: 29.3.2011
 *      Author: Antti Kamppi
 * 		filename: modelparametermodel.cpp
 */

#include "modelparametermodel.h"

#include <QColor>

#include <QDebug>

ModelParameterModel::ModelParameterModel(void* dataPointer, 
										 QObject *parent): 
QAbstractTableModel(parent),
modelParameters_(), 
table_() {

	Q_ASSERT_X(dataPointer, "ModelParameterModel constructor",
		"Null dataPointer given as parameter");

	// set the pointer to the actual data structure containing the 
	// model parameters
	modelParameters_ = static_cast<
		QMap<QString, QSharedPointer<ModelParameter> > *>(dataPointer);

	restore();
}

ModelParameterModel::~ModelParameterModel() {
}

int ModelParameterModel::rowCount( const QModelIndex & parent /*= QModelIndex() */ ) const {
	
	// not hierarchical model
	if (parent.isValid())
		return 0;

	return table_.size();
}

int ModelParameterModel::columnCount( const QModelIndex & parent /*= QModelIndex() */ ) const {
	
	// not hierarchical model
	if (parent.isValid())
		return 0;

	return 5;
}

QVariant ModelParameterModel::data( const QModelIndex & index, 
								   int role /*= Qt::DisplayRole */ ) const {

	if (!index.isValid())
		return QVariant();

	// if row is invalid
	if (index.row() < 0 || index.row() >= table_.size())
		return QVariant();

	if (role == Qt::DisplayRole) {
		
		switch (index.column()) {
			case 0: 
				return table_.at(index.row())->getName();
			case 1:
				return table_.at(index.row())->getDataType();
			case 2:
				return table_.at(index.row())->getUsageType();
			case 3:
				return table_.at(index.row())->getValue();
			case 4:
				return table_.at(index.row())->getDescription();
			default:
				return QVariant();
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case 0:
			case 2:
			case 3: {
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

QVariant ModelParameterModel::headerData( int section, 
										 Qt::Orientation orientation, 
										 int role /*= Qt::DisplayRole */ ) const {

	// only horizontal headers are supported
	if (orientation != Qt::Horizontal) 
		return QVariant();

	if (role == Qt::DisplayRole) {
		
		switch (section) {
			case 0:
				return tr("Name");
			case 1:
				return tr("Data type");
			case 2:
				return tr("Usage type");
			case 3:
				return tr("Value");
			case 4:
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

bool ModelParameterModel::setData( const QModelIndex& index, 
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

				// if name changes then the map has to change the indexing also
				QString oldKey = table_.value(index.row())->getName();

				// remove the item with old key and insert the value with new key
				//QSharedPointer<ModelParameter> modParam = modelParameters_->take(oldKey);
				//modelParameters_->insert(value.toString(), modParam);

				table_[index.row()]->setName(value.toString());
				break;				
					}
			case 1: {
				table_[index.row()]->setDataType(value.toString());
				break;
					}
			case 2: {
				table_[index.row()]->setUsageType(value.toString());
				break;
					}
			case 3: {
				table_[index.row()]->setValue(value.toString());
				break;
					}
			case 4: {
				table_[index.row()]->setDescription(value.toString());
				break;
					}
			default:
				return false;

		}
		emit dataChanged(index, index);
		return true;
	}
	// if role is not supported
	else {
		return false;
	}
}

Qt::ItemFlags ModelParameterModel::flags(const QModelIndex& index) const {

	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void ModelParameterModel::onRemoveRow( int row ) {
	
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

void ModelParameterModel::onRemoveItem( const QModelIndex& index ) {
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

void ModelParameterModel::onAddRow() {

	beginInsertRows(QModelIndex(), table_.size(), table_.size());
	
	table_.append(QSharedPointer<ModelParameter>(new ModelParameter()));
	
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void ModelParameterModel::onAddItem( const QModelIndex& index ) {
	int row = table_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	table_.insert(row, QSharedPointer<ModelParameter>(new ModelParameter()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

bool ModelParameterModel::isValid() const {
	
	for (int i = 0; i < table_.size(); ++i) {
		
		// if one model parameter is not valid
		if (!table_.value(i)->isValid()) {
			return false;
		}
	}
	return true;
}

void ModelParameterModel::apply() {
	// remove old model parameters from original model
	modelParameters_->clear();

	for (int i = 0; i < table_.size(); ++i) {
		modelParameters_->insert(table_.value(i)->getName(), table_.value(i));
	}
}

void ModelParameterModel::restore() {

	beginResetModel();

	// remove old model parameters
	table_.clear();

	// add all model parameters to the table
	foreach (QSharedPointer<ModelParameter> modelParameter, *modelParameters_) {

		// create a copy of the model parameter
		table_.append(QSharedPointer<ModelParameter>(
			new ModelParameter(*modelParameter.data())));
	}

	endResetModel();
}
