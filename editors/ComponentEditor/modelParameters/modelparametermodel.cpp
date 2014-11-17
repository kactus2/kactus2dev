/* 
 *
 *  Created on: 29.3.2011
 *      Author: Antti Kamppi
 * 		filename: modelparametermodel.cpp
 */

#include "modelparametermodel.h"

#include "ModelParameterColumns.h"

#include <QColor>
#include <QPersistentModelIndex>

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/Enumeration.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/model.h>


//-----------------------------------------------------------------------------
// Function: ModelParameterModel::ModelParameterModel()
//-----------------------------------------------------------------------------
ModelParameterModel::ModelParameterModel(QSharedPointer<Model> model, 
    QSharedPointer<QList<QSharedPointer<Choice> > > choices,
										 QObject *parent): 
QAbstractTableModel(parent),
model_(model), choices_(choices), lockedIndexes_()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::~ModelParameterModel()
//-----------------------------------------------------------------------------
ModelParameterModel::~ModelParameterModel()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::rowCount()
//-----------------------------------------------------------------------------
int ModelParameterModel::rowCount( const QModelIndex & parent /*= QModelIndex() */ ) const 
{
	// not hierarchical model
	if (parent.isValid())
		return 0;

	return model_->getModelParameters().count();
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::columnCount()
//-----------------------------------------------------------------------------
int ModelParameterModel::columnCount( const QModelIndex & parent /*= QModelIndex() */ ) const
{
	// not hierarchical model
	if (parent.isValid())
		return 0;

	return ModelParameterColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::data()
//-----------------------------------------------------------------------------
QVariant ModelParameterModel::data( const QModelIndex & index, int role /*= Qt::DisplayRole */ ) const
{
	if (!index.isValid())
    {
		return QVariant();
    }

	// if row is invalid
	if (index.row() < 0 || index.row() >= model_->getModelParameters().count())
		return QVariant();

    QList<QSharedPointer<ModelParameter> > modelParameters = model_->getModelParameters();

    QSharedPointer<ModelParameter> modelParameter = modelParameters.at(index.row());

	if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case ModelParameterColumns::NAME: 
            return modelParameter->getName();
        case ModelParameterColumns::DISPLAY_NAME:
            return modelParameter->getDisplayName();
        case ModelParameterColumns::DATA_TYPE:
            return modelParameter->getDataType();
        case ModelParameterColumns::USAGE_TYPE:
            return modelParameter->getUsageType();
        case ModelParameterColumns::FORMAT:
            return modelParameter->getValueFormat();
        case ModelParameterColumns::BITSTRINGLENGTH:
            return modelParameter->getBitStringLength();
        case ModelParameterColumns::MINIMUM:   
            return modelParameter->getMinimumValue();
        case ModelParameterColumns::MAXIMUM:   
            return modelParameter->getMaximumValue();
        case ModelParameterColumns::CHOICE:
            return modelParameter->getChoiceRef();
        case ModelParameterColumns::VALUE:
            return evaluateValueFor(modelParameter);
        case ModelParameterColumns::DESCRIPTION:
            return modelParameter->getDescription();
        default:
            return QVariant();
        }
	}
    else if (Qt::BackgroundRole == role)
    {
        switch (index.column())
        {
        case ModelParameterColumns::NAME:
        case ModelParameterColumns::USAGE_TYPE:
        case ModelParameterColumns::VALUE:
            {
                return QColor("LemonChiffon");
            }
        case ModelParameterColumns::BITSTRINGLENGTH:
            {
                if (modelParameter->getValueFormat() == "bitString")
                {
                    return QColor("LemonChiffon");
                }
                else
                {
                    return QColor("white");
                }
            }
        default: 
            {
                return QColor("white");
            }       
        }
    }
    else if (Qt::ForegroundRole == role)
    {
        if (modelParameter->isValid())
        {
            if (lockedIndexes_.contains(index) )
            {
                return QColor("gray");
            }
            else if ((index.column() == ModelParameterColumns::CHOICE ||
                index.column() == ModelParameterColumns::VALUE ) && 
                !modelParameter->getChoiceRef().isEmpty() &&
                !findChoice(modelParameter->getChoiceRef())->hasEnumeration(modelParameter->getValue()))
            {
                return QColor("red");
            }
            else
            {
                return QColor("black");
            } 
        }
        else
        {
            return QColor("red");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::headerData()
//-----------------------------------------------------------------------------
QVariant ModelParameterModel::headerData(int section, Qt::Orientation orientation,
										 int role /*= Qt::DisplayRole */ ) const {

	// only horizontal headers are supported
	if (orientation != Qt::Horizontal) 
		return QVariant();

	if (role == Qt::DisplayRole) 
    {	
		switch (section) {
			case ModelParameterColumns::NAME:
				return tr("Name");
            case ModelParameterColumns::DISPLAY_NAME:
                return tr("Display name");
			case ModelParameterColumns::DATA_TYPE:
				return tr("Data type");
			case ModelParameterColumns::USAGE_TYPE:
				return tr("Usage\ntype");
            case  ModelParameterColumns::FORMAT:
                return tr("Format");
            case  ModelParameterColumns::BITSTRINGLENGTH:
                return tr("Bit string\nlength");
            case  ModelParameterColumns::MINIMUM:
                return tr("Minimum\nvalue");
            case ModelParameterColumns::MAXIMUM:   
                return tr("Maximum\nvalue");
            case ModelParameterColumns::CHOICE:
                return tr("Choice");
			case ModelParameterColumns::VALUE:
				return tr("Value");
			case ModelParameterColumns::DESCRIPTION:
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

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::setData()
//-----------------------------------------------------------------------------
bool ModelParameterModel::setData(QModelIndex const& index, QVariant const& value, int role /*= Qt::EditRole */ )
{
	if (!index.isValid())
    {
		return false;
    }
    else if (index.row() < 0 || index.row() >= model_->getModelParameters().count())
    {
		return false;
    }

	if (role == Qt::EditRole)
    {
        if ( isLocked(index) )
        {
            return false;
        }

        QSharedPointer<ModelParameter>  modelParameter = model_->getModelParameters().at(index.row());

        switch (index.column())
        {
        case ModelParameterColumns::NAME:
            {
                modelParameter->setName(value.toString());
                break;				
            }
        case ModelParameterColumns::DISPLAY_NAME:
            {
                modelParameter->setDisplayName(value.toString());
                break;				
            }
        case ModelParameterColumns::DATA_TYPE:
            {
                modelParameter->setDataType(value.toString());
                break;
            }
        case ModelParameterColumns::USAGE_TYPE:
            {
                modelParameter->setUsageType(value.toString());
                break;
            }
        case ModelParameterColumns::FORMAT:
            {
                modelParameter->setValueFormat(value.toString());
                break;
            }
        case ModelParameterColumns::BITSTRINGLENGTH:
            {
                modelParameter->setBitStringLength(value.toString());
                break;
            }
        case ModelParameterColumns::MINIMUM:
            {
                modelParameter->setMinimumValue(value.toString());
                break;
            }
        case ModelParameterColumns::MAXIMUM:
            {
                modelParameter->setMaximumValue(value.toString());
                break;
            }
        case ModelParameterColumns::CHOICE:
            {
                modelParameter->setChoiceRef(value.toString());
                break;
            }
        case ModelParameterColumns::VALUE:
            {
                modelParameter->setValue(value.toString());
                break;
            }
        case ModelParameterColumns::DESCRIPTION:
            {
                modelParameter->setDescription(value.toString());
                break;
            }
        default:
            {
                return false;
            }
        }
		emit dataChanged(index, index);
		return true;
	}
	// if role is not supported
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ModelParameterModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if ( isLocked(index) )
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::onRemoveRow()
//-----------------------------------------------------------------------------
void ModelParameterModel::onRemoveRow( int row )
{
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

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ModelParameterModel::onRemoveItem( const QModelIndex& index )
{
	// don't remove anything if index is invalid
	if (!index.isValid())
    {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >=  model_->getModelParameters().count())
    {
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

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::onAddRow()
//-----------------------------------------------------------------------------
void ModelParameterModel::onAddRow()
{
    int row = model_->getModelParameters().count();
	beginInsertRows(QModelIndex(), row, row);
	
	model_->addModelParameter(QSharedPointer<ModelParameter>(new ModelParameter()));
	
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::onAddItem()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::addModelParameter()
//-----------------------------------------------------------------------------
void ModelParameterModel::addModelParameter( QSharedPointer<ModelParameter> modelParam )
{
    int row = model_->getModelParameters().count();
	beginInsertRows(QModelIndex(), row, row);
	model_->addModelParameter(modelParam);
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::removeModelParameter()
//-----------------------------------------------------------------------------
void ModelParameterModel::removeModelParameter(QSharedPointer<ModelParameter> removedParam ) {

    // find the index for the model parameter
    QModelIndex paramIndex = index(removedParam);

    // if the model parameter was found
    if (paramIndex.isValid()) {
        unlockModelParameter(removedParam);    
        onRemoveItem(paramIndex);
    }

}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::isValid()
//-----------------------------------------------------------------------------
bool ModelParameterModel::isValid() const
{	
    foreach (QSharedPointer<ModelParameter> modelParameter, model_->getModelParameters())
    {
        if (!modelParameter->isValid())
        {
            return false;
        }

        if (!modelParameter->getChoiceRef().isEmpty())
        {
            QSharedPointer<Choice> referencedChoice = findChoice(modelParameter->getChoiceRef());
            if(!referencedChoice->hasEnumeration(modelParameter->getValue()))
            {
                return false;
            }
        }
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::index()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::getParameter()
//-----------------------------------------------------------------------------
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
// Function: ModelParameterModel::evaluateValueFor()
//-----------------------------------------------------------------------------
QString ModelParameterModel::evaluateValueFor(QSharedPointer<ModelParameter> modelParameter) const
{
    if (modelParameter->getChoiceRef().isEmpty())
    {
        return modelParameter->getValue();
    }
    else
    {
        QSharedPointer<Choice> choice = findChoice(modelParameter->getChoiceRef());
        return findDisplayValueForEnumeration(choice, modelParameter->getValue());
    }
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ModelParameterModel::findChoice(QString const& choiceName) const
{
    foreach (QSharedPointer<Choice> choice, *choices_)
    {
        if (choice->getName() == choiceName)
        {
            return choice;
        }
    }	

    return QSharedPointer<Choice>(new Choice(QDomNode()));
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::findDisplayValueForEnumeration()
//-----------------------------------------------------------------------------
QString ModelParameterModel::findDisplayValueForEnumeration(QSharedPointer<Choice> choice,
    QString const& enumerationValue) const
{
    foreach (QSharedPointer<Enumeration> enumeration, *choice->enumerations())
    {
        if (enumeration->getValue() == enumerationValue && !enumeration->getText().isEmpty())
        {
            return enumeration->getText();
        }
    }

    return enumerationValue;
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
