//-----------------------------------------------------------------------------
// File: ConfigurableElementsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 12.08.2011
//
// Description:
// Model class to manage the configurable element values being edited.
//-----------------------------------------------------------------------------

#include "ConfigurableElementsModel.h"

#include <common/KactusColors.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <editors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/validators/ValueFormatter.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>
#include <IPXACTmodels/Component/Choice.h>

#include <QIcon>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::ConfigurableElementsModel()
//-----------------------------------------------------------------------------
ConfigurableElementsModel::ConfigurableElementsModel(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> configurableElementExpressionFormatter,
    QSharedPointer<ExpressionFormatter> defaultValueFormatter,
    QSharedPointer<ExpressionParser> configurableElementExpressionParser,
    QSharedPointer<ExpressionParser> defaultValueParser, QObject *parent):
QAbstractItemModel(parent),
ParameterizableTable(parameterFinder),
configurableElements_(),
choices_(),
configurableElementExpressionFormatter_(configurableElementExpressionFormatter),
defaultValueFormatter_(defaultValueFormatter),
defaultValueParser_(defaultValueParser),
validator_(0),
itemConfigurableElementValues_(0),
referableParameters_(new QList<QSharedPointer<Parameter> > ())
{
    setExpressionParser(configurableElementExpressionParser);

    QSharedPointer<QList<QSharedPointer<Choice> > > noChoices(new QList<QSharedPointer<Choice> >());
    validator_ = QSharedPointer<ParameterValidator2014>(
        new ParameterValidator2014(configurableElementExpressionParser, noChoices));
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::ConfigurableElementsModel()
//-----------------------------------------------------------------------------
ConfigurableElementsModel::~ConfigurableElementsModel() 
{
    
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::setParameters()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::setParameters(QString const& containingItemName,
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
    QSharedPointer<QList<QSharedPointer<Choice> > > choices,
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > storedConfigurableElements)
{
    containingItemName_ = containingItemName;
    choices_ = choices;
    itemConfigurableElementValues_ = storedConfigurableElements;

    beginResetModel();

    configurableElements_.clear();

    if (parameters && !parameters->isEmpty())
    {
        foreach (QSharedPointer<Parameter> parameterPointer, *parameters)
        {
            QString configuratedValue = parameterPointer->getValue();
            QString evaluatedDefaultValue = evaluateValue(ConfigurableElementsColumns::DEFAULT_VALUE,
                parameterPointer->getChoiceRef(), parameterPointer->getValue());

            QSharedPointer<EditorConfigurableElement> newElement(new EditorConfigurableElement(
                parameterPointer, evaluatedDefaultValue, configuratedValue));
            configurableElements_.append(newElement);
        }
    }

    restoreStoredConfigurableElements();

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::clear()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::clear() 
{
	beginResetModel();
    configurableElements_.clear();
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::rowCount()
//-----------------------------------------------------------------------------
int ConfigurableElementsModel::rowCount(QModelIndex const& parent) const 
{
    if (!parent.isValid())
    {
        return configurableElements_.size();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::columnCount()
//-----------------------------------------------------------------------------
int ConfigurableElementsModel::columnCount(QModelIndex const& /*parent*/) const 
{
    return ConfigurableElementsColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::data()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::data(QModelIndex const& index, int role) const 
{
    if (role == getItemConfigurableElementValuesRole)
    {
        return QVariant::fromValue(getItemConfigurableElementValues());
    }
    else if (role == getConfigurableElementsFromTableRole)
    {
        return QVariant::fromValue(getConfigurableElements());
    }
    else if (!isIndexValid(index))
    {
        return QVariant();
    }

    QSharedPointer<EditorConfigurableElement> element = configurableElements_.at(index.row());

    if (role == Qt::ForegroundRole)
    {
        if (element->getEvaluatedDefaultValue().isEmpty())
        {
            return KactusColors::ERROR;
        }
        else if (index.column() == ConfigurableElementsColumns::VALUE && isParameterEditable(index))
        {
            return blackForValidOrRedForInvalidIndex(index);
        }
        else
        {
            return KactusColors::DISABLED_TEXT;
        }
    }
	else if (role == Qt::DisplayRole) 
    {
        return valueForIndex(index);
	}
    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        return tooltipForIndex(index);
    }
    else if (role == Qt::FontRole)
    {
        return italicForEvaluatedValue(index);
    }
    else if (role == Qt::DecorationRole)
    {
        if (index.parent().isValid() && index.column() == ConfigurableElementsColumns::NAME &&
            !element->getReferencedParameter()->getChoiceRef().isEmpty())
        {
            return QIcon(":/icons/common/graphics/paginator.png");
        }
    }
    else if (role == deletableElementCheckRole)
    {
        return isElementDeletable(index);
    }
    else if (role == parameterIDRole)
    {
        return parameterIDForIndex(index);
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::headerData()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) 
    {
        if (section == ConfigurableElementsColumns::NAME)
        {
            return tr("Name");
        }
        else if (section == ConfigurableElementsColumns::VALUE)
        {
            QString valueHeader = tr("Value") + getExpressionSymbol();
            return valueHeader;
        }
        else if (section == ConfigurableElementsColumns::DEFAULT_VALUE)
        {
            return tr("Default value");
        }
        else if (section == ConfigurableElementsColumns::CHOICE)
        {
            return tr("Choice");
        }
        else if (section == ConfigurableElementsColumns::ARRAY_LEFT)
        {
            return tr("Array left");
        }
        else if (section == ConfigurableElementsColumns::ARRAY_RIGHT)
        {
            return tr("Array right");
        }
        else if (section == ConfigurableElementsColumns::TYPE)
        {
            return tr("Type");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::setData()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::setData(QModelIndex const& index, QVariant const& value, int role) 
{
    if (!isIndexValid(index))
    {
        return false;
    }
    
    if (role == Qt::EditRole) 
    {
        QSharedPointer<EditorConfigurableElement> element = configurableElements_.at(index.row());

        if (index.column() == ConfigurableElementsColumns::VALUE)
        {
            if (!value.isValid() || value.toString().isEmpty() ||
                value.toString() == element->getReferencedParameter()->getValue())
            {
                element->setConfiguratedValue(element->getReferencedParameter()->getValue());
            }

            else
            {
                element->setConfiguratedValue(value.toString());
            }
        }
        else if (index.column() != ConfigurableElementsColumns::NAME &&
            index.column() != ConfigurableElementsColumns::DEFAULT_VALUE)
        {
            return false;
        }
        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ConfigurableElementsModel::flags(QModelIndex const& index) const 
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

    if (!isParameterEditable(index))
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    if (index.column() == ConfigurableElementsColumns::VALUE)
    {
    	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
    else
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::isParameterEditable()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::isParameterEditable(QModelIndex const& index) const
{
    if (isIndexValid(index))
    {
        QSharedPointer<EditorConfigurableElement> element = configurableElements_.at(index.row());

        return (!element->getReferencedParameter()->getValueResolve().isEmpty() &&
            element->getReferencedParameter()->getValueResolve().compare(QStringLiteral("immediate")) != 0);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == ConfigurableElementsColumns::VALUE ||
        index.column() == ConfigurableElementsColumns::DEFAULT_VALUE;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::valueForIndex(QModelIndex const& index) const
{
    if (!isIndexValid(index))
    {
        return false;
    }

    QSharedPointer<EditorConfigurableElement> element = configurableElements_.at(index.row());

    if (index.column() == ConfigurableElementsColumns::NAME)
    {
        return element->getReferencedParameter()->name();
    }
    else if (index.column() == ConfigurableElementsColumns::VALUE)
    {
        return evaluateValue(index.column(), element->getReferencedParameter()->getChoiceRef(),
            element->getConfiguratedValue());
    }
    else if (index.column() == ConfigurableElementsColumns::DEFAULT_VALUE)
    {
        return element->getEvaluatedDefaultValue();
    }
    else if (index.column() == ConfigurableElementsColumns::CHOICE)
    {
        return element->getReferencedParameter()->getChoiceRef();
    }
    else if (index.column() == ConfigurableElementsColumns::ARRAY_LEFT)
    {
        return element->getReferencedParameter()->getArrayLeft();
    }
    else if (index.column() == ConfigurableElementsColumns::ARRAY_RIGHT)
    {
        return element->getReferencedParameter()->getArrayRight();
    }
    else if (index.column() == ConfigurableElementsColumns::TYPE)
    {
        return element->getReferencedParameter()->getType();
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::evaluateValueForIndex()
//-----------------------------------------------------------------------------
QString ConfigurableElementsModel::evaluateValue(int column, QString const& choiceName,
    QString const& value) const
{
    if (!choiceName.isEmpty())
    {
        QSharedPointer<Choice> choice = findChoice(choiceName);

        if (value.contains('{') && value.contains('}'))
        {
            return matchArrayValuesToSelectedChoice(choice, value);
        }
        else
        {
            return findDisplayValueForEnumeration(choice, value);
        }
    }
    else if (column == ConfigurableElementsColumns::VALUE)
    {
        return ParameterizableTable::formattedValueFor(value);
    }
    else
    {
        return formattedValueFor(value);
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::formattedValueFor()
//-----------------------------------------------------------------------------
QString ConfigurableElementsModel::formattedValueFor(QString const& expression) const
{
    if (defaultValueParser_->isValidExpression(expression))
    {
        ValueFormatter formatter;
        return formatter.format(defaultValueParser_->parseExpression(expression),
            defaultValueParser_->baseForExpression(expression));
    }
    else if (defaultValueParser_->isPlainValue(expression))
    {
        return expression;
    }
    else
    {
        return "n/a";
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ConfigurableElementsModel::findChoice(QString const& choiceName) const
{
    foreach (QSharedPointer<Choice> choice, *choices_)
    {
        if (choice->name().compare(choiceName) == 0)
        {
            return choice;
        }
    }

    return QSharedPointer<Choice>(new Choice());
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::matchArrayValuesToSelectedChoice()
//-----------------------------------------------------------------------------
QString ConfigurableElementsModel::matchArrayValuesToSelectedChoice(QSharedPointer<Choice> choice,
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
// Function: ConfigurableElementsModel::findDisplayValueForEnumeration()
//-----------------------------------------------------------------------------
QString ConfigurableElementsModel::findDisplayValueForEnumeration(QSharedPointer<Choice> choice,
    QString const& enumerationValue) const
{
    QString value = parseExpressionToDecimal(enumerationValue);
    foreach (QSharedPointer<Enumeration> enumeration, *choice->enumerations())
    {
        if (enumeration->getValue().compare(value) == 0 && !enumeration->getText().isEmpty())
        {
            return enumeration->getText();
        }
    }

    return value;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (!isIndexValid(index))
    {
        return false;
    }

    QSharedPointer<EditorConfigurableElement> element = configurableElements_.at(index.row());

    if (index.column() == ConfigurableElementsColumns::NAME)
    {
        return element->getReferencedParameter()->name();
    }
    else if (index.column() == ConfigurableElementsColumns::VALUE)
    {
        return element->getConfiguratedValue();
    }
    else if (index.column() == ConfigurableElementsColumns::DEFAULT_VALUE)
    {
        return element->getReferencedParameter()->getValue();
    }

    return data(index, Qt::DisplayRole);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::tooltipForIndex()
//-----------------------------------------------------------------------------
QString ConfigurableElementsModel::tooltipForIndex(QModelIndex const& index) const
{
    if (!isIndexValid(index))
    {
        return QString();
    }

    QSharedPointer<EditorConfigurableElement> element = configurableElements_.at(index.row());

    if (element->getEvaluatedDefaultValue().isEmpty())
    {
        return QString("This parameter was not found in " + containingItemName_ + ".");
    }
    else if (index.column() == ConfigurableElementsColumns::NAME)
    {
        QString tooltip(element->getReferencedParameter()->name());
        if (!element->getReferencedParameter()->getChoiceRef().isEmpty())
        {
            QSharedPointer<Choice> choice = findChoice(element->getReferencedParameter()->getChoiceRef());
            tooltip.append("<br>Possible values are:");

            foreach (QSharedPointer<Enumeration> enumeration, *choice->enumerations())
            {
                tooltip.append("<br>" + enumeration->getValue());
                if (!enumeration->getText().isEmpty())
                {
                    tooltip.append(" (" + enumeration->getText() + ")");
                }
            }
        }

        return tooltip;
    }

    else if (index.column() == ConfigurableElementsColumns::VALUE)
    {
        QString context = containingItemName_;

        QVector<QString> errorList;
        validator_->findErrorsIn(errorList, element->getReferencedParameter(), context);

        if (!errorList.isEmpty())
        {
            return QStringList(errorList.toList()).join("\n");
        }
    }
    else if (index.column() == ConfigurableElementsColumns::DEFAULT_VALUE)
    {
        return defaultValueFormatter_->formatReferringExpression(expressionOrValueForIndex(index).toString());
    }

    return configurableElementExpressionFormatter_->
        formatReferringExpression(expressionOrValueForIndex(index).toString());
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::validateIndex()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::validateIndex(QModelIndex const& index) const
{
    if (!isIndexValid(index))
    {
        return false;
    }

    QSharedPointer<EditorConfigurableElement> element = configurableElements_.at(index.row());

    if (index.column() == ConfigurableElementsColumns::NAME)
    {
        return validator_->hasValidName(element->getReferencedParameter());
    }
    else if (index.column() == ConfigurableElementsColumns::VALUE)
    {
        return validator_->hasValidValue(element->getReferencedParameter());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::onRemoveItem(QString const& elementID, int elementRow)
{
    foreach (QSharedPointer<EditorConfigurableElement> element, configurableElements_)
    {
        if (element->getReferencedParameter() && element->getReferencedParameter()->getValueId() == elementID)
        {
            beginRemoveRows(QModelIndex(), elementRow, elementRow);

            configurableElements_.removeOne(element);

            endRemoveRows();

            emit contentChanged();
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::onAddItem()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::onAddItem(QString const& elementID, QString const& elementValue, int elementRow)
{
    QSharedPointer<Parameter> unknownParameter = createUnknownParameter(elementID);

    QSharedPointer<EditorConfigurableElement> unknownElement
        (new EditorConfigurableElement(unknownParameter, QString(), elementValue));

    beginInsertRows(QModelIndex(), elementRow, elementRow);

    configurableElements_.insert(elementRow, unknownElement);

    endInsertRows();

    QModelIndex firstItemIndex = index(0, ConfigurableElementsColumns::NAME);
    QModelIndex lastItemIndex = index(configurableElements_.size() - 1, ConfigurableElementsColumns::NAME);

    emit dataChanged(firstItemIndex, lastItemIndex);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::readStoredConfigurableElements()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::restoreStoredConfigurableElements()
{
    if (itemConfigurableElementValues_)
    {
        foreach (QSharedPointer<ConfigurableElementValue> element, *itemConfigurableElementValues_)
        {
            QSharedPointer<EditorConfigurableElement> storedElement = getStoredConfigurableElement(
                element->getReferenceId());
            if (storedElement)
            {
                storedElement->setConfiguratedValue(element->getConfigurableValue());
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::getStoredConfigurableElement()
//-----------------------------------------------------------------------------
QSharedPointer<EditorConfigurableElement> ConfigurableElementsModel::getStoredConfigurableElement(
    QString const& elementID)
{
    foreach (QSharedPointer<EditorConfigurableElement> element, configurableElements_)
    {
        if (element->getReferencedParameter() && element->getReferencedParameter()->getValueId() == elementID)
        {
            return element;
        }
    }

    QSharedPointer<Parameter> newReferencedParameter = createUnknownParameter(elementID);
    QSharedPointer<EditorConfigurableElement> newConfigurableElement(new EditorConfigurableElement(
        newReferencedParameter, QString(), QString()));
    configurableElements_.append(newConfigurableElement);

    return newConfigurableElement;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::createUnknownParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ConfigurableElementsModel::createUnknownParameter(QString const& parameterID)
{
    QSharedPointer<Parameter> newReferencedParameter(new Parameter());
    newReferencedParameter->setName(QStringLiteral("Unknown referenced ID"));
    newReferencedParameter->setValueId(parameterID);
    newReferencedParameter->setValueResolve(QStringLiteral("user"));

    return newReferencedParameter;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::index()
//-----------------------------------------------------------------------------
QModelIndex ConfigurableElementsModel::index(int row, int column, const QModelIndex& ) const
{
    QSharedPointer<EditorConfigurableElement> element = configurableElements_.at(row);
    return createIndex(row, column, element.data());
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::parent()
//-----------------------------------------------------------------------------
QModelIndex ConfigurableElementsModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::isIndexValid()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::isIndexValid(QModelIndex const& index) const
{
    if (index.isValid() && index.row() >= 0 && index.row() < configurableElements_.size())
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::isElementDeletable()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::isElementDeletable(QModelIndex const& index) const
{
    QSharedPointer<EditorConfigurableElement> elemenet = configurableElements_.at(index.row());
    return elemenet->getEvaluatedDefaultValue().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::getConfigurableElements()
//-----------------------------------------------------------------------------
QList<QSharedPointer<EditorConfigurableElement> > ConfigurableElementsModel::getConfigurableElements() const
{
    return configurableElements_;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::parameterIDForIndex()
//-----------------------------------------------------------------------------
QString ConfigurableElementsModel::parameterIDForIndex(QModelIndex const& index) const
{
    QSharedPointer<Parameter> referencedParamter = configurableElements_.at(index.row())->getReferencedParameter();
    if (referencedParamter)
    {
        return referencedParamter->getValueId();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::getConfigurableElementValues()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > ConfigurableElementsModel::
    getItemConfigurableElementValues() const
{
    return itemConfigurableElementValues_;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::emitDataChangeForID()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::emitDataChangeForID(QString const& parameterID, QString const& newValue)
{
    for (int i = 0; i < configurableElements_.size(); ++i)
    {
        QSharedPointer<EditorConfigurableElement> element = configurableElements_.at(i);
        if (element->getReferencedParameter() && element->getReferencedParameter()->getValueId() == parameterID)
        {
            element->setConfiguratedValue(newValue);

            QModelIndex currentIndex = index(i, ConfigurableElementsColumns::VALUE, QModelIndex());

            emit(dataChanged(currentIndex, currentIndex));
            return;
        }
    }
}
