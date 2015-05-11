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

#include "ValueFormatter.h"

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/Enumeration.h>

#include <IPXACTmodels/validators/ParameterValidator2014.h>

#include <QColor>
#include <QFont>
#include <QMessageBox>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::AbstractParameterModel()
//-----------------------------------------------------------------------------
AbstractParameterModel::AbstractParameterModel(QSharedPointer<QList<QSharedPointer<Choice> > > choices,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QObject *parent): 
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
choices_(choices), 
validator_(new ParameterValidator2014(expressionParser, parameterFinder)),
expressionFormatter_(expressionFormatter)
{
    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::~AbstractParameterModel()
//-----------------------------------------------------------------------------
AbstractParameterModel::~AbstractParameterModel()
{
    delete validator_;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::data()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::data( QModelIndex const& index, int role /*= Qt::DisplayRole */ ) const 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return QVariant();
    }
    
    if (role == Qt::DisplayRole)
    {
        if (index.column() == valueColumn() || 
            index.column() == bitWidthLeftColumn() || index.column() == bitWidthRightColumn() ||
            index.column() == arrayLeftColumn() || index.column() == arrayRightColumn())
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        if (index.column() == valueColumn() && !index.sibling(index.row(), choiceColumn()).data().toString().isEmpty())
        {
            QString choiceText = index.sibling(index.row(), choiceColumn()).data(Qt::DisplayRole).toString();
            if (!choiceText.isEmpty())
            {
                return expressionOrValueForIndex(index);
            }
        }

        if (index.column() == valueColumn() ||
            index.column() == bitWidthLeftColumn() || index.column() == bitWidthRightColumn() ||
            index.column() == arrayLeftColumn() || index.column() == arrayRightColumn())
        {
            return formattedValueFor(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (Qt::BackgroundRole == role) 
    {
        return backgroundColorForIndex(index);
    }
    else if (Qt::ForegroundRole == role)
    {
        return blackForValidOrRedForInvalidIndex(index);
    }

    else // if unsupported role
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::headerData()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) 
    {
        if (section == nameColumn())
        {
            return tr("Name");
        }
        else if (section == displayNameColumn())
        {
            return tr("Display\nname");
        }
        else if (section == typeColumn())
        {
            return tr("Type");   
        }
        else if (section == bitWidthLeftColumn())
        {
            QString bitWidthLeft = tr("Bit vector\nleft") + getExpressionSymbol();
            return bitWidthLeft;
        }
        else if (section == bitWidthRightColumn())
        {
            QString bitWidthRight = tr("Bit vector\nright") + getExpressionSymbol();
            return bitWidthRight;
        }
        else if (section == minimumColumn())
        {
            return tr("Min");
        }
        else if (section == maximumColumn())
        {
            return tr("Max");
        }
        else if (section == choiceColumn())
        {
            return tr("Choice");
        }     
        else if (section == valueColumn())
        {
            QString valueHeader = tr("Value") + getExpressionSymbol();
            return valueHeader;
        }  
        else if (section == resolveColumn())
        {     
            return tr("Resolve");
        }  
        else if (section == arrayLeftColumn())
        {
            QString arrayLeftHeader = tr("Array\nleft") + getExpressionSymbol();
            return arrayLeftHeader;
        }  
        else if (section == arrayRightColumn())
        {
            QString arrayRightHeader = tr("Array\nright") + getExpressionSymbol();
            return arrayRightHeader;
        } 
        else if (section == descriptionColumn())
        { 
            return tr("Description");
        }
        else if (section == idColumn())
        {
            return tr("Parameter id");
        }
        else if (section == usageCountColumn())
        {
            return tr("Usage\ncount");
        }

        else
        {
            return QVariant();
        }
    }
    else if (orientation == Qt::Vertical && role == Qt::DisplayRole) 
    {
        return QString(" ");
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
bool AbstractParameterModel::setData(QModelIndex const& index, const QVariant& value, int role /*= Qt::EditRole */) 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return false;
    }

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
        else if (index.column() == typeColumn())
        {
            parameter->setType(value.toString());
        }
        else if (index.column() == bitWidthLeftColumn())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(parameter->getBitWidthLeft());

                emit dataChanged(QAbstractTableModel::index(0, usageCountColumn()),
                    QAbstractTableModel::index(rowCount() - 1, usageCountColumn()));
            }

            parameter->setBitWidthLeft(value.toString());

            if (value.isValid() && parameter->getBitWidthRight().isEmpty() && !value.toString().isEmpty())
            {
                parameter->setBitWidthRight(QString::number(0));
            }
        }
        else if (index.column() == bitWidthRightColumn())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(parameter->getBitWidthRight());

                emit dataChanged(QAbstractTableModel::index(0, usageCountColumn()),
                    QAbstractTableModel::index(rowCount() - 1, usageCountColumn()));
            }

            parameter->setBitWidthRight(value.toString());

            if (value.isValid() && parameter->getBitWidthLeft().isEmpty() && !value.toString().isEmpty())
            {
                parameter->setBitWidthLeft(QString::number(0));
            }
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
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(parameter->getValue());

                emit dataChanged(QAbstractTableModel::index(0, usageCountColumn()),
                    QAbstractTableModel::index(rowCount() - 1, usageCountColumn()));
            }

            parameter->setValue(value.toString());
        }
        else if (index.column() == resolveColumn())
        {
            parameter->setValueResolve(value.toString());
        }
        else if (index.column() == arrayLeftColumn())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(parameter->getAttribute("kactus2:arrayLeft"));

                emit dataChanged(QAbstractTableModel::index(0, usageCountColumn()),
                    QAbstractTableModel::index(rowCount() - 1, usageCountColumn()));
            }

            parameter->setAttribute("kactus2:arrayLeft", value.toString());

            if (value.isValid() && parameter->getAttribute("kactus2:arrayRight").isEmpty() &&
                !parameter->getAttribute("kactus2:arrayLeft").isEmpty())
            {
                parameter->setAttribute("kactus2:arrayRight", QString::number(0));
            }
        }
        else if (index.column() == arrayRightColumn())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(parameter->getAttribute("kactus2:arrayRight"));

                emit dataChanged(QAbstractTableModel::index(0, usageCountColumn()),
                    QAbstractTableModel::index(rowCount() - 1, usageCountColumn()));
            }

            parameter->setAttribute("kactus2:arrayRight", value.toString());

            if (value.isValid() && parameter->getAttribute("kactus2:arrayLeft").isEmpty() &&
                !parameter->getAttribute("kactus2:arrayRight").isEmpty())
            {
                parameter->setAttribute("kactus2:arrayLeft", QString::number(0));
            }
        }
        else if (index.column() == descriptionColumn())
        {
            parameter->setDescription(value.toString());
        }
        else if (index.column() == idColumn())
        {
            return false;
        }
        else
        {
            return false;
        }

        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }
    else // is unsupported role
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AbstractParameterModel::flags(QModelIndex const& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

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

        if (!validator_->validate(parameter.data(), choices_)) 
        {
            return false;
        }
	}
	
	// all parameters are valid
	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::isValid()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::isValid(QStringList& errorList, QString const& parentIdentifier) const
{
    bool valid = true;
    for (int i = 0; i < rowCount(); i++)
    {
        QSharedPointer<Parameter> parameter = getParameterOnRow(i);

        errorList.append(validator_->findErrorsIn(parameter.data(), parentIdentifier, choices_));

        // if one parameter is invalid, model is invalid.
        if (!validator_->validate(parameter.data(), choices_))
        {
            valid = false;
        }
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::isMandatoryColumn()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::isMandatoryColumn(int column) const
{
    return column == nameColumn() || column == valueColumn();
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::evaluateValueFor()
//-----------------------------------------------------------------------------
QString AbstractParameterModel::evaluateValueFor(QSharedPointer<Parameter> parameter) const
{
    if (!parameter->getChoiceRef().isEmpty())
    {
        QSharedPointer<Choice> choice = findChoice(parameter->getChoiceRef());

        if(parameter->getValue().contains('{') && parameter->getValue().contains('}'))
        {
            return matchArrayValuesToSelectedChoice(choice, parameter->getValue());
        }

        else
        {
            return findDisplayValueForEnumeration(choice, parameter->getValue());
        }
    }
    else
    {
        return parameter->getValue();
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

    return QSharedPointer<Choice>(new Choice());
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

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::matchArrayValuesToSelectedChoice()
//-----------------------------------------------------------------------------
QString AbstractParameterModel::matchArrayValuesToSelectedChoice(QSharedPointer<Choice> choice,
    QString const& arrayValue) const
{
    QStringList parameterArray = arrayValue.split(',');
    parameterArray.first().remove('{');
    parameterArray.last().remove('}');

    QStringList resultingArray;

    foreach (QString value, parameterArray)
    {
        resultingArray.append(findDisplayValueForEnumeration(choice, value));
    }
    QString choiceValue = resultingArray.join(',');
    choiceValue.prepend('{');
    choiceValue.append('}');

    return choiceValue;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::validateIndex()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer<Parameter> parameter = getParameterOnRow(index.row());

    if (index.column() == nameColumn())
    {
        return validator_->hasValidName(parameter.data());
    }
    else if (index.column() == typeColumn())
    {
        return validator_->hasValidValueForType(parameter->getValue(), parameter->getType());
    }
    else if (index.column() == bitWidthLeftColumn())
    {
        return validator_->validateArrayValues(parameter->getBitWidthLeft(), parameter->getBitWidthRight());
    }
    else if (index.column() == bitWidthRightColumn())
    {
        return validator_->validateArrayValues(parameter->getBitWidthLeft(), parameter->getBitWidthRight());
    }
    else if (index.column() == minimumColumn())
    {
        return validator_->hasValidMinimumValue(parameter.data()) && 
            !validator_->valueIsLessThanMinimum(parameter.data());
    }
    else if (index.column() == maximumColumn())
    {
        return validator_->hasValidMaximumValue(parameter.data()) && 
            !validator_->valueIsGreaterThanMaximum(parameter.data());
    }
    else if (index.column() == choiceColumn())
    {
        return validator_->hasValidChoice(parameter.data(), choices_) &&
            validator_->hasValidValueForChoice(parameter.data(), choices_);
    }
    else if (index.column() == valueColumn())
    {
        return validator_->hasValidValue(parameter.data(), choices_);
    }
    else if (index.column() == resolveColumn())
    {
        return validator_->hasValidResolve(parameter.data());
    }
    else if (index.column() == arrayLeftColumn())
    {
        return validator_->validateArrayValues(parameter->getAttribute("kactus2:arrayLeft"),
            parameter->getAttribute("kactus2:arrayRight"));
    }
    else if (index.column() == arrayRightColumn())
    {
        return validator_->validateArrayValues(parameter->getAttribute("kactus2:arrayLeft"),
            parameter->getAttribute("kactus2:arrayRight"));
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::isValidExpressionColumn(QModelIndex const& index) const
{
     QSharedPointer<Parameter> parameter = getParameterOnRow(index.row());

    if ((index.column() == valueColumn() && parameter->getType() != "string") ||
        index.column() == arrayLeftColumn() || index.column() == arrayRightColumn() ||
        index.column() == bitWidthLeftColumn() || index.column() == bitWidthRightColumn())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::valueForIndex(QModelIndex const& index) const
{
    QSharedPointer<Parameter> parameter = getParameterOnRow(index.row());

    if (index.column() == nameColumn())
    {
        return parameter->getName();
    }
    else if (index.column() == displayNameColumn())
    {
        return parameter->getDisplayName();
    }
    else if (index.column() == typeColumn())
    {
        return parameter->getType();
    }
    else if (index.column() == bitWidthLeftColumn())
    {
        return parameter->getBitWidthLeft();
    }
    else if (index.column() == bitWidthRightColumn())
    {
        return parameter->getBitWidthRight();
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
    else if (index.column() == arrayLeftColumn())
    {
        return parameter->getAttribute("kactus2:arrayLeft");
    }
    else if (index.column() == arrayRightColumn())
    {
        return parameter->getAttribute("kactus2:arrayRight");
    }
    else if (index.column() == descriptionColumn())
    {
        return parameter->getDescription();
    }
    else if (index.column() == idColumn())
    {
        return parameter->getValueId();
    }
    else if (index.column() == usageCountColumn())
    {
        return parameter->getUsageCount();
    }
    else 
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    QSharedPointer<Parameter> parameter = getParameterOnRow(index.row());

    if (index.column() == valueColumn())
    {
        return parameter->getValue();
    }
    else if (index.column() == minimumColumn())
    {
        return parameter->getMinimumValue();
    }
    else if (index.column() == maximumColumn())
    {
        return parameter->getMaximumValue();
    }
    else if (index.column() == arrayLeftColumn())
    {
        return parameter->getAttribute("kactus2:arrayLeft");
    }
    else if (index.column() == arrayRightColumn())
    {
        return parameter->getAttribute("kactus2:arrayRight");
    }
    else if (index.column() == bitWidthLeftColumn())
    {
        return parameter->getBitWidthLeft();
    }
    else if (index.column() == bitWidthRightColumn())
    {
        return parameter->getBitWidthRight();
    }
    else
    {
        return data(index, Qt::DisplayRole);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::canRemoveRow()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::canRemoveRow(int const& row) const
{
    QSharedPointer<Parameter> parameter = getParameterOnRow(row);

    if (parameter->getUsageCount() > 0)
    {
        QMessageBox removeWarning;
        removeWarning.setText("Are you sure you want to remove " + parameter->getName()
            + "? There are " + QString::number(parameter->getUsageCount()) + " references to it." +
            " \n\nTo see where " + parameter->getName() + 
            " has been referenced, double click its usage count.");
        removeWarning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        removeWarning.setDefaultButton(QMessageBox::No);
        removeWarning.setIcon(QMessageBox::Warning);

        QApplication::setOverrideCursor(Qt::ArrowCursor);

        if (removeWarning.exec() == QMessageBox::No)
        {
            QApplication::restoreOverrideCursor();
            return false;
        }

        QApplication::restoreOverrideCursor();
    }

    removeReferencesInItemOnRow(row);

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::backgroundColorForIndex()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::backgroundColorForIndex(QModelIndex const& index) const
{
    QSharedPointer<Parameter> parameter = getParameterOnRow(index.row());

    if (isMandatoryColumn(index.column())) 
    {
        return QColor("lemonChiffon");
    }
    else if (((index.column() == minimumColumn() || index.column() == maximumColumn()) &&
        (parameter->getType().isEmpty() || parameter->getType() == "bit" || parameter->getType() == "string"))
        ||
        ((index.column() == bitWidthLeftColumn() || index.column() == bitWidthRightColumn()) &&
        parameter->getType() != "bit"))
    {
        return QColor("whiteSmoke");
    }
    else
    {
        return QColor("white");
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::getAllReferencesToIdInItemRow()
//-----------------------------------------------------------------------------
int AbstractParameterModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    QSharedPointer<Parameter> parameter = getParameterOnRow(row);

    int referencesInValue = parameter->getValue().count(valueID);
    int referencesInBitWidthLeft = parameter->getBitWidthLeft().count(valueID);
    int referencesInBitWidthRight = parameter->getBitWidthRight().count(valueID);
    int referencesInArrayLeft = parameter->getAttribute("kactus2:arrayLeft").count(valueID);
    int referencesinArrayRight = parameter->getAttribute("kactus2:arrayRight").count(valueID);

    int totalReferencesToParameter = referencesInValue + referencesInBitWidthLeft + referencesInBitWidthRight +
        referencesInArrayLeft + referencesinArrayRight;

    return totalReferencesToParameter;
}
