//-----------------------------------------------------------------------------
// File: parametersmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 4.4.2011
//
// Description:
// Table model that can be used to display parameters to be edited.
//-----------------------------------------------------------------------------

#include "parametersmodel.h"

#include "ParameterColumns.h"

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/Enumeration.h>

#include <QColor>


//-----------------------------------------------------------------------------
// Function: ParametersModel::ParametersModel()
//-----------------------------------------------------------------------------
ParametersModel::ParametersModel(QList<QSharedPointer<Parameter> >& parameters,
    QSharedPointer<QList<QSharedPointer<Choice> > > choices,
    QObject *parent): 
QAbstractTableModel(parent), 
parameters_(parameters), choices_(choices)
{

}

//-----------------------------------------------------------------------------
// Function: ParametersModel::~ParametersModel()
//-----------------------------------------------------------------------------
ParametersModel::~ParametersModel()
{

}

//-----------------------------------------------------------------------------
// Function: ParametersModel::rowCount()
//-----------------------------------------------------------------------------
int ParametersModel::rowCount(const QModelIndex& parent /*= QModelIndex() */ ) const
{
	if (parent.isValid()) {
		return 0;
	}

	return parameters_.size();
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::columnCount()
//-----------------------------------------------------------------------------
int ParametersModel::columnCount( const QModelIndex& parent /*= QModelIndex() */ ) const {

	if (parent.isValid()) {
		return 0;
	}

	return ParameterColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::data()
//-----------------------------------------------------------------------------
QVariant ParametersModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole */ ) const 
{
	if (!index.isValid())
		return QVariant();

	// if row is invalid
	else if (index.row() < 0 || index.row() >= parameters_.size())
		return QVariant();

    QSharedPointer<Parameter> parameter = parameters_.at(index.row());

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case ParameterColumns::NAME: 
            {
                return parameter->getName();
            }
        case ParameterColumns::FORMAT:
            {
                return parameter->getValueFormat();
            }
        case ParameterColumns::MINIMUM:
            {
                return parameter->getMinimumValue();
            }
        case ParameterColumns::CHOICE: 
            {
                return parameter->getChoiceRef();
            }
        case ParameterColumns::VALUE:
            {
                return evaluateValueFor(parameter);
            }
        case ParameterColumns::DESCRIPTION: 
            {
                return parameter->getDescription();
            }
        default: 
            {
                return QVariant();
            }
        }
    }
    else if (Qt::BackgroundRole == role) 
    {
        switch (index.column()) 
        {
        case ParameterColumns::NAME:
        case ParameterColumns::VALUE: 
            {
                return QColor("LemonChiffon");
            }
        default:
            return QColor("white");
        }
    }
    else if (Qt::ForegroundRole == role)
    {
        if ((index.column() == ParameterColumns::CHOICE ||
            index.column() == ParameterColumns::VALUE ) && 
            !parameter->getChoiceRef().isEmpty() &&
            !findChoice(parameter->getChoiceRef())->hasEnumeration(parameter->getValue()))
        {
            return QColor("red");
        }
        else if (parameter->isValid()) 
        {
            return QColor("black");
        }
        else 
        {
            return QColor("red");
        }
    }

	// if unsupported role
	else 
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::headerData()
//-----------------------------------------------------------------------------
QVariant ParametersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
        return QVariant();

    if (role == Qt::DisplayRole) 
    {
        switch (section)
        {
        case ParameterColumns::NAME:
            return tr("Name");
        case ParameterColumns::FORMAT:
            return tr("Format");   
        case ParameterColumns::MINIMUM:
            return tr("Minimum value");
        case ParameterColumns::CHOICE:
            return tr("Choice");
        case ParameterColumns::VALUE:
            return tr("Value");
        case ParameterColumns::DESCRIPTION:
            return tr("Description");
        default:
            return QVariant();
        }
    }

    // if unsupported role
    else 
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::setData()
//-----------------------------------------------------------------------------
bool ParametersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole */) 
{
	if (!index.isValid())
		return false;

    // if row is invalid
    else if (index.row() < 0 || index.row() >= parameters_.size())
        return false;

    if (role == Qt::EditRole)
    {
        QSharedPointer<Parameter> parameter = parameters_.value(index.row());
        switch (index.column())
        {
        case ParameterColumns::NAME: 
            {
                parameter->setName(value.toString());
                break;
            }
        case ParameterColumns::FORMAT: 
            {
                parameter->setValueFormat(value.toString());
                break;
            }
        case ParameterColumns::MINIMUM:
            {
                parameter->setMinimumValue(value.toString());
                break;
            }
        case ParameterColumns::CHOICE: 
            {
                parameter->setChoiceRef(value.toString());
                break;
            }
        case ParameterColumns::VALUE:
            {
                parameter->setValue(value.toString());
                break;
            }
        case ParameterColumns::DESCRIPTION: 
            {
                parameter->setDescription(value.toString());
                break;
            }
        default: 
            return false;
        }
        emit contentChanged();
        return true;
    }

    // is unsupported role
    else 
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ParametersModel::flags(const QModelIndex& index ) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::isValid()
//-----------------------------------------------------------------------------
bool ParametersModel::isValid() const {

	// check all parameters
	foreach (QSharedPointer<Parameter> parameter, parameters_)
    {
		// if one parameter is invalid
		if (!parameter->isValid())
			return false;

        if (!parameter->getChoiceRef().isEmpty())
        {
            QSharedPointer<Choice> referencedChoice = findChoice(parameter->getChoiceRef());
            if(!referencedChoice->hasEnumeration(parameter->getValue()))
            {
                return false;
            }
        }

	}

	// all parameters are valid
	return true;
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::isValid()
//-----------------------------------------------------------------------------
bool ParametersModel::isValid(QStringList& errorList, const QString& parentIdentifier) const
{
    bool valid = true;
    // check all parameters.
    foreach (QSharedPointer<Parameter> parameter, parameters_) 
    {
        // if one parameter is invalid, model is invalid.
        if (!parameter->isValid(errorList, parentIdentifier))
            valid = false;

        if (!parameter->getChoiceRef().isEmpty())
        {
            QSharedPointer<Choice> referencedChoice = findChoice(parameter->getChoiceRef());
            if(!referencedChoice->hasEnumeration(parameter->getValue()))
            {
                errorList.append(QObject::tr("Parameter %1 references unknown choice value %2 "
                    "for choice %3 within %4").arg(parameter->getName(), 
                    parameter->getValue(), parameter->getChoiceRef(), parentIdentifier));
                valid = false;
            }
        }
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ParametersModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
 	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= parameters_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	parameters_.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::onAddItem()
//-----------------------------------------------------------------------------
void ParametersModel::onAddItem( const QModelIndex& index ) {
	int row = parameters_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	parameters_.insert(row, QSharedPointer<Parameter>(new Parameter()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ParametersModel::evaluateValueFor()
//-----------------------------------------------------------------------------
QString ParametersModel::evaluateValueFor(QSharedPointer<Parameter> modelParameter) const
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
// Function: ParametersModel::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ParametersModel::findChoice(QString const& choiceName) const
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
// Function: ParametersModel::findDisplayValueForEnumeration()
//-----------------------------------------------------------------------------
QString ParametersModel::findDisplayValueForEnumeration(QSharedPointer<Choice> choice,
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
