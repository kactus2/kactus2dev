//-----------------------------------------------------------------------------
// File: AbstractParameterModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.11.2014
//
// Description:
// Base class for models editing parameters and model parameters.
//-----------------------------------------------------------------------------

#include "AbstractParameterModel.h"

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/Enumeration.h>

#include <QColor>

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::AbstractParameterModel()
//-----------------------------------------------------------------------------
AbstractParameterModel::AbstractParameterModel(QSharedPointer<QList<QSharedPointer<Choice> > > choices,
    QObject *parent): 
QAbstractTableModel(parent), choices_(choices)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::~AbstractParameterModel()
//-----------------------------------------------------------------------------
AbstractParameterModel::~AbstractParameterModel()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::data()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole */ ) const 
{
	if (!index.isValid())
		return QVariant();

	// if row is invalid
	else if (index.row() < 0 || index.row() >= rowCount())
		return QVariant();

    QSharedPointer<Parameter> parameter = getParameterOnRow(index.row());

    if (role == Qt::DisplayRole)
    {
        if (index.column() == nameColumn())
        {
            return parameter->getName();
        }
        else if (index.column() == displayNameColumn())
        {
            return parameter->getDisplayName();
        }
        else if (index.column() == formatColumn())
        {
            return parameter->getValueFormat();
        }
        else if (index.column() == bitwidthColumn())
        {
            return parameter->getBitStringLength();
        }
        else if (index.column() == minimumColumn())
        {
            return parameter->getMinimumValue();
        }
        else if (index.column() == maximumColumn())
        {
            return parameter->getMaximumValue();
        }
        else if (index.column() == choiceColumn())
        {
            return parameter->getChoiceRef();
        }
        else if (index.column() == valueColumn())
        {
            return evaluateValueFor(parameter);
        }
        else if (index.column() == resolveColumn())
        {
            return parameter->getValueResolve();
        }
        else if (index.column() == arraySizeColumn())
        {
            return parameter->getAttribute("arraySize");
        }
        else if (index.column() == arrayOffsetColumn())
        {
            return parameter->getAttribute("arrayOffset");
        }
        else if (index.column() == descriptionColumn())
        {
            return parameter->getDescription();
        }
        else 
        {
            return QVariant();
        }
    }
    else if (Qt::BackgroundRole == role) 
    {
        if (index.column() == nameColumn() ||
            index.column() == valueColumn()) 
        {
            return QColor("LemonChiffon");
        }
        else if (index.column() == bitwidthColumn())
        {
            if (parameter->getValueFormat() == "bitString")
            {
                return QColor("LemonChiffon");
            }
            else
            {
                return QColor("white");
            }
        }
        else
        {
            return QColor("white");
        }
    }
    else if (Qt::ForegroundRole == role)
    {
        if ((index.column() == choiceColumn() ||
            index.column() == valueColumn() ) && 
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
// Function: AbstractParameterModel::headerData()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
        return QVariant();

    if (role == Qt::DisplayRole) 
    {
        if (section == nameColumn())
        {
            return tr("Name");
        }
        else if (section == displayNameColumn())
        {
            return tr("Display\nname");
        }
        else if (section == formatColumn())
        {
            return tr("Format");   
        }
        else if (section == bitwidthColumn())
        {
            return tr("Bit string\nlength");   
        }
        else if (section == minimumColumn())
        {
            return tr("Minimum\nvalue");
        }
        else if (section == maximumColumn())
        {        
            return tr("Maximum\nvalue");
        }
        else if (section == choiceColumn())
        {        
            return tr("Choice");
        }     
        else if (section == valueColumn())
        {        
            return tr("Value");
        }  
        else if (section == resolveColumn())
        {        
            return tr("Resolve");
        }  
        else if (section == arraySizeColumn())
        {        
            return tr("Array\nsize");
        }  
        else if (section == arrayOffsetColumn())
        {        
            return tr("Array\noffset");
        } 
        else if (section == descriptionColumn())
        { 
            return tr("Description");
        }
        else
        {
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
// Function: AbstractParameterModel::setData()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole */) 
{
	if (!index.isValid())
		return false;

    // if row is invalid
    else if (index.row() < 0 || index.row() >= rowCount())
        return false;

    if (role == Qt::EditRole)
    {
        QSharedPointer<Parameter> parameter = getParameterOnRow(index.row());

        if (index.column() == nameColumn())
        {
            parameter->setName(value.toString());
        }
        else if (index.column() == displayNameColumn())
        {
            parameter->setDisplayName(value.toString());
        }
        else if (index.column() == formatColumn())
        {
            parameter->setValueFormat(value.toString());
        }
        else if (index.column() == bitwidthColumn())
        {
            parameter->setBitStringLength(value.toString());
        }
        else if (index.column() == minimumColumn())
        {
            parameter->setMinimumValue(value.toString());
        }
        else if (index.column() == maximumColumn())
        {
            parameter->setMaximumValue(value.toString());
        }
        else if (index.column() == choiceColumn())
        {
            parameter->setChoiceRef(value.toString());
        }
        else if (index.column() == valueColumn())
        {
            parameter->setValue(value.toString());
        }
        else if (index.column() == resolveColumn())
        {
            parameter->setValueResolve(value.toString());
        }
        else if (index.column() == arraySizeColumn())
        {
            parameter->setAttribute("arraySize", value.toString());
        }
        else if (index.column() == arrayOffsetColumn())
        {
            parameter->setAttribute("arrayOffset", value.toString());
        }
        else if (index.column() == descriptionColumn())
        {
            parameter->setDescription(value.toString());
        }
        else
        {
            return false;
        }

        emit dataChanged(index, index);
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
// Function: AbstractParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AbstractParameterModel::flags(const QModelIndex& index ) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::isValid()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::isValid() const
{
	// check all parameters
	for (int i = 0; i < rowCount(); i++)
	{
        QSharedPointer<Parameter> parameter = getParameterOnRow(i);

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
// Function: AbstractParameterModel::isValid()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::isValid(QStringList& errorList, const QString& parentIdentifier) const
{
    bool valid = true;

    for (int i = 0; i < rowCount(); i++)
    {
        QSharedPointer<Parameter> parameter = getParameterOnRow(i);

        // if one parameter is invalid, model is invalid.
        if (!parameter->isValid(errorList, parentIdentifier))
        {
            valid = false;
        }

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
// Function: AbstractParameterModel::evaluateValueFor()
//-----------------------------------------------------------------------------
QString AbstractParameterModel::evaluateValueFor(QSharedPointer<Parameter> parameter) const
{
    if (parameter->getChoiceRef().isEmpty())
    {
        return parameter->getValue();
    }
    else
    {
        QSharedPointer<Choice> choice = findChoice(parameter->getChoiceRef());
        return findDisplayValueForEnumeration(choice, parameter->getValue());
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> AbstractParameterModel::findChoice(QString const& choiceName) const
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
// Function: AbstractParameterModel::findDisplayValueForEnumeration()
//-----------------------------------------------------------------------------
QString AbstractParameterModel::findDisplayValueForEnumeration(QSharedPointer<Choice> choice,
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
