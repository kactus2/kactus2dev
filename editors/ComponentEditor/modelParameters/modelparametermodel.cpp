/* 
 *
 *  Created on: 29.3.2011
 *      Author: Antti Kamppi
 * 		filename: modelparametermodel.cpp
 */

#include "modelparametermodel.h"

#include <QColor>
#include <QPersistentModelIndex>

ModelParameterModel::ModelParameterModel(QSharedPointer<Component> component, 
										 QObject *parent): 
QAbstractTableModel(parent),
table_(component->getModelParameters()), lockedIndexes_()
{

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
            if ( lockedIndexes_.contains(index) )
            {
                return QColor("gray");
            }
            else
            {
                return QColor("black");
            } 
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

        if ( isLocked(index) ){
            return false;
        }

		switch (index.column()) {
			case 0: {
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

    if ( isLocked(index) )
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void ModelParameterModel::onRemoveRow( int row ) {
	
	// if row is invalid
	if (row < 0 || row >= table_.size())
		return;

    if ( isLocked( QAbstractTableModel::index(row, 0, QModelIndex())) )
    {
        return;
    }

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

    if (isLocked( QAbstractTableModel::index(index.row(), 0, QModelIndex())))
    {
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

void ModelParameterModel::onAddItem( const QModelIndex& index )
{
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

void ModelParameterModel::addModelParameter( QSharedPointer<ModelParameter> modelParam )
{
	beginInsertRows(QModelIndex(), table_.size(), table_.size());
	table_.append(modelParam);
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void ModelParameterModel::removeModelParameter(QSharedPointer<ModelParameter> removedParam ) {

    // find the index for the model parameter
    QModelIndex paramIndex = index(removedParam);

    // if the model parameter was found
    if (paramIndex.isValid()) {
        unlockModelParameter(removedParam);    
        onRemoveItem(paramIndex);
    }

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

QModelIndex ModelParameterModel::index( QSharedPointer<ModelParameter> modelParam ) const {
	// find the correct row
	int row = table_.indexOf(modelParam);

	// if the model parameter is not found
	if (row < 0) {
		return QModelIndex();
	}

	// the base class creates the index for the row
	return QAbstractTableModel::index(row, 0, QModelIndex());
}

QSharedPointer<ModelParameter> ModelParameterModel::getParameter(QModelIndex const& index) const
{
	Q_ASSERT(index.isValid());
    return table_.at(index.row());
}   

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::setComponentAndLockCurrentModelParameters()
//-----------------------------------------------------------------------------
void ModelParameterModel::setComponentAndLockCurrentModelParameters(QSharedPointer<Component> component)
{
    beginResetModel();
    
    lockedIndexes_.clear();
    table_ = component->getModelParameters();
    endResetModel();

    foreach(QSharedPointer<ModelParameter> modelParameter, table_)
    {
        lockModelParameter(modelParameter);
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: lockModelParameter()
//-----------------------------------------------------------------------------
void ModelParameterModel::lockModelParameter(QSharedPointer<ModelParameter> modelParam)
{
    QModelIndex nameIndex = index(modelParam);
    QModelIndex typeIndex = nameIndex.sibling(nameIndex.row(),1);
    QModelIndex usageIndex = nameIndex.sibling(nameIndex.row(),2);
    if ( nameIndex.isValid() && typeIndex.isValid() && usageIndex.isValid() )
    {     
        lockIndex(nameIndex);  
        lockIndex(typeIndex);
        lockIndex(usageIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: unlockModelParameter()
//-----------------------------------------------------------------------------
void ModelParameterModel::unlockModelParameter(QSharedPointer<ModelParameter> modelParam)
{
    QModelIndex nameIndex = index(modelParam);
    QModelIndex typeIndex = nameIndex.sibling(nameIndex.row(),1);
    QModelIndex usageIndex = nameIndex.sibling(nameIndex.row(),2);
    if ( nameIndex.isValid() && typeIndex.isValid() && usageIndex.isValid() )
    {     
        unlockIndex(nameIndex);  
        unlockIndex(typeIndex);
        unlockIndex(usageIndex);      
    }
}

//-----------------------------------------------------------------------------
// Function: lockIndex()
//-----------------------------------------------------------------------------
void ModelParameterModel::lockIndex(QModelIndex const& index)
{
    lockedIndexes_.append(QPersistentModelIndex(index));
}

//-----------------------------------------------------------------------------
// Function: unlockIndex()
//-----------------------------------------------------------------------------
void ModelParameterModel::unlockIndex(QModelIndex const& index)
{
    lockedIndexes_.removeAll(QPersistentModelIndex(index));
}

//-----------------------------------------------------------------------------
// Function: isLocked()
//-----------------------------------------------------------------------------
bool ModelParameterModel::isLocked(QModelIndex const& index) const
{
    return lockedIndexes_.contains(index);
}
