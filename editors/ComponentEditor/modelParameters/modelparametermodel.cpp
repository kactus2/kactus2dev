/* 
 *
 *  Created on: 29.3.2011
 *      Author: Antti Kamppi
 * 		filename: modelparametermodel.cpp
 */

#include "modelparametermodel.h"

#include <QColor>
#include <QPersistentModelIndex>

#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/model.h>

ModelParameterModel::ModelParameterModel(QSharedPointer<Model> model, 
										 QObject *parent): 
QAbstractTableModel(parent),
model_(model), lockedIndexes_()
{

}

ModelParameterModel::~ModelParameterModel() {
}

int ModelParameterModel::rowCount( const QModelIndex & parent /*= QModelIndex() */ ) const {
	
	// not hierarchical model
	if (parent.isValid())
		return 0;

	return model_->getModelParameters().count();
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
	if (index.row() < 0 || index.row() >= model_->getModelParameters().count())
		return QVariant();

    QList<QSharedPointer<ModelParameter> > modelParameters = model_->getModelParameters();

	if (role == Qt::DisplayRole) {
		        
		switch (index.column()) {
			case 0: 
				return modelParameters.at(index.row())->getName();
			case 1:
				return modelParameters.at(index.row())->getDataType();
			case 2:
				return modelParameters.at(index.row())->getUsageType();
			case 3:
				return modelParameters.at(index.row())->getValue();
			case 4:
				return modelParameters.at(index.row())->getDescription();
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

        if (modelParameters.at(index.row())->isValid()) {
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
	else if (index.row() < 0 || index.row() >= model_->getModelParameters().count())
		return false;

    QList<QSharedPointer<ModelParameter> > modelParameters = model_->getModelParameters();

	if (role == Qt::EditRole) {

        if ( isLocked(index) ){
            return false;
        }

		switch (index.column()) {
			case 0: {
				modelParameters[index.row()]->setName(value.toString());
				break;				
					}
			case 1: {
				modelParameters[index.row()]->setDataType(value.toString());
				break;
					}
			case 2: {
				modelParameters[index.row()]->setUsageType(value.toString());
				break;
					}
			case 3: {
				modelParameters[index.row()]->setValue(value.toString());
				break;
					}
			case 4: {
				modelParameters[index.row()]->setDescription(value.toString());
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
	if (row < 0 || row >= model_->getModelParameters().count())
		return;

    if (isLocked(QAbstractTableModel::index(row, 0, QModelIndex())))
    {
        return;
    }

	beginRemoveRows(QModelIndex(), row, row);

	// remove the object from the map
    QList<QSharedPointer<ModelParameter> >& modelParameters = model_->getModelParameters();
	modelParameters.removeAt(row);

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
	else if (index.row() < 0 || index.row() >=  model_->getModelParameters().count()) {
		return;
    }

    if (isLocked( QAbstractTableModel::index(index.row(), 0, QModelIndex())))
    {
        return;
    }

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
    QList<QSharedPointer<ModelParameter> >& modelParameters = model_->getModelParameters();
	modelParameters.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void ModelParameterModel::onAddRow()
{
    int row = model_->getModelParameters().count();
	beginInsertRows(QModelIndex(), row, row);
	
	model_->addModelParameter(QSharedPointer<ModelParameter>(new ModelParameter()));
	
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void ModelParameterModel::onAddItem( const QModelIndex& index )
{
    QList<QSharedPointer<ModelParameter> >& modelParameters = model_->getModelParameters();
	int row = modelParameters.count();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	modelParameters.insert(row, QSharedPointer<ModelParameter>(new ModelParameter()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void ModelParameterModel::addModelParameter( QSharedPointer<ModelParameter> modelParam )
{
    int row = model_->getModelParameters().count();
	beginInsertRows(QModelIndex(), row, row);
	model_->addModelParameter(modelParam);
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

bool ModelParameterModel::isValid() const
{	
    foreach (QSharedPointer<ModelParameter> modelParameter, model_->getModelParameters())
    {
        if (!modelParameter->isValid())
        {
            return false;
        }
    }

	return true;
}

QModelIndex ModelParameterModel::index( QSharedPointer<ModelParameter> modelParam ) const
{
	// find the correct row
	int row =  model_->getModelParameters().indexOf(modelParam);

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
    return  model_->getModelParameters().at(index.row());
}   

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::setModelAndLockCurrentModelParameters()
//-----------------------------------------------------------------------------
void ModelParameterModel::setModelAndLockCurrentModelParameters(QSharedPointer<Model> model)
{
    beginResetModel();
    
    lockedIndexes_.clear();
    model_ = model;
    endResetModel();

    foreach(QSharedPointer<ModelParameter> modelParameter, model_->getModelParameters())
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
