/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: componentinstancemodel.cpp
 *		Project: Kactus 2
 */

#include "ConfigurableElementsModel.h"
#include "ConfigurableElementsColumns.h"

#include <designEditors/common/DesignDiagram.h>
#include <designEditors/HWDesign/HWChangeCommands.h>

#include <IPXACTmodels/Enumeration.h>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::ConfigurableElementsModel()
//-----------------------------------------------------------------------------
ConfigurableElementsModel::ConfigurableElementsModel(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<ExpressionParser> expressionParser,
    QObject *parent):
ParameterizableTable(parameterFinder, parent),
component_(0),
currentElementValues_(),
configurableElements_(),
editProvider_(0),
expressionFormatter_(expressionFormatter),
validator_(new ParameterValidator2014(expressionParser, parameterFinder))
{
    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::ConfigurableElementsModel()
//-----------------------------------------------------------------------------
ConfigurableElementsModel::~ConfigurableElementsModel() 
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::setComponent()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::setComponent( ComponentItem* component ) 
{
	Q_ASSERT(component);

	// if there is previously selected component
	if (component_) 
    {
		component_->disconnect(this);
	}

	component_ = component;

    // get the edit provider that manages the undo/redo stack
    DesignDiagram* diagram = static_cast<DesignDiagram*>(component->scene());
    editProvider_ = &diagram->getEditProvider();
    	
	currentElementValues_ = component->getConfigurableElements();
	readValues();

	connect(component_, SIGNAL(confElementsChanged(const QMap<QString, QString>&)),
		this, SLOT(changeElements(const QMap<QString, QString>&)), Qt::UniqueConnection);

	// if the connected component is destroyed then clear this editor
	connect(component_, SIGNAL(destroyed(ComponentItem*)),
		    this, SLOT(clear()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::clear()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::clear() 
{
	// if previous component has been specified
	if (component_) 
    {
		component_->disconnect(this);
	}

	component_ = 0;
	currentElementValues_.clear();

	beginResetModel();
    configurableElements_.clear();
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::rowCount()
//-----------------------------------------------------------------------------
int ConfigurableElementsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const 
{
	if (parent.isValid())
    {
        return 0;
    }
    return configurableElements_.size();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::columnCount()
//-----------------------------------------------------------------------------
int ConfigurableElementsModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const 
{
	if (parent.isValid())
    {
		return 0;
    }

	return ConfigurableElementsColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::data()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const 
{
    if (!index.isValid() || index.row() < 0 || index.row() >= configurableElements_.size())
    {
		return QVariant();
    }

    if (role == Qt::ForegroundRole)
    {
        if (configurableElements_.at(index.row())->getValueAttribute("spirit:defaultValue").isEmpty())
        {
            return QColor(Qt::red);
        }
        
        else if (index.column() == ConfigurableElementsColumns::VALUE && isParameterEditable(index.row()))
        {
            return blackForValidOrRedForInvalidIndex(index);
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
        return expressionFormatter_->formatReferringExpression(expressionOrValueForIndex(index).toString());
    }

    else if (role == Qt::FontRole)
    {
        return italicForEvaluatedValue(index);
    }

	else 
    {
		return QVariant();
    }
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::headerData()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::headerData( int section,
											Qt::Orientation orientation, 
											int role /*= Qt::DisplayRole */ ) const 
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
    }

	if (role == Qt::DisplayRole) 
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
        else
        {
            return QVariant();
        }
	}
	else
    {
		return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::setData()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::setData( const QModelIndex& index,
									 const QVariant& value, 
									 int role /*= Qt::EditRole */ ) 
{
    if (!index.isValid() || index.row() < 0 || index.row() >= configurableElements_.size())
    {
		return false;
    }

	if (role == Qt::EditRole) 
    {
        if (index.column() == ConfigurableElementsColumns::VALUE)
        {
            if (!value.isValid())
            {
                configurableElements_.at(index.row())->setValue(configurableElements_.at(index.row())->
                    getValueAttribute("spirit:defaultValue"));
                configurableElements_.at(index.row())->setAttribute("spirit:editedInConfigurableElements", "");
            }

            else
            {
                configurableElements_.at(index.row())->setAttribute("spirit:editedInConfigurableElements", "true");
                configurableElements_.at(index.row())->setValue(value.toString());
            }
        }
        else if (index.column() != ConfigurableElementsColumns::NAME &&
            index.column() != ConfigurableElementsColumns::DEFAULT_VALUE)
        {
            return false;
        }
		save();
		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	else
    {
		return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ConfigurableElementsModel::flags( const QModelIndex& index ) const 
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

    if (!isParameterEditable(index.row()))
    {
        return Qt::ItemIsSelectable;
    }

    if (index.column() == ConfigurableElementsColumns::VALUE)
    {
    	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
    else
    {
        return Qt::ItemIsSelectable;
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::isValidExpressionColumn(QModelIndex const& index) const
{
    if (index.column() == ConfigurableElementsColumns::VALUE ||
        index.column() == ConfigurableElementsColumns::DEFAULT_VALUE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == ConfigurableElementsColumns::NAME)
    {
        return configurableElements_.at(index.row())->getName();
    }
    else if (index.column() == ConfigurableElementsColumns::VALUE)
    {
        return evaluateValueForIndex(index, configurableElements_.at(index.row())->getValue());
    }
    else if (index.column() == ConfigurableElementsColumns::DEFAULT_VALUE)
    {
        return evaluateValueForIndex(index, configurableElements_.at(index.row())->getValueAttribute
            ("spirit:defaultValue"));
    }
    else if (index.column() == ConfigurableElementsColumns::CHOICE)
    {
        return configurableElements_.at(index.row())->getChoiceRef();
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::evaluateValueForIndex()
//-----------------------------------------------------------------------------
QString ConfigurableElementsModel::evaluateValueForIndex(QModelIndex const& index, QString const& value) const
{
    if (!configurableElements_.at(index.row())->getChoiceRef().isEmpty())
    {
        QSharedPointer<Choice> choice = findChoice(configurableElements_.at(index.row())->getChoiceRef());

        if (value.contains('{') && value.contains('}'))
        {
            return matchArrayValuesToSelectedChoice(choice, value);
        }
        else
        {
            return findDisplayValueForEnumeration(choice, value);
        }
    }
    else
    {
        return formattedValueFor(value);
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ConfigurableElementsModel::findChoice(QString const& choiceName) const
{
    foreach (QSharedPointer<Choice> choice, *component_->componentModel()->getChoices())
    {
        if (choice->getName() == choiceName)
        {
            return choice;
        }
    }

    return QSharedPointer<Choice> (new Choice());
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::findDisplayValueForEnumeration()
//-----------------------------------------------------------------------------
QString ConfigurableElementsModel::findDisplayValueForEnumeration(QSharedPointer<Choice> choice,
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
// Function: ConfigurableElementsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (index.column() == ConfigurableElementsColumns::NAME)
    {
        return configurableElements_.at(index.row())->getName();
    }
    else if (index.column() == ConfigurableElementsColumns::VALUE)
    {
        return configurableElements_.at(index.row())->getValue();
    }
    else if (index.column() == ConfigurableElementsColumns::DEFAULT_VALUE)
    {
        return configurableElements_.at(index.row())->getValueAttribute("spirit:defaultValue");
    }
    else
    {
        return data(index, Qt::DisplayRole);
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::validateColumnForParameter()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::validateColumnForParameter(QModelIndex const& index) const
{
    QSharedPointer<Parameter> parameter = configurableElements_.at(index.row());

    if (index.column() == ConfigurableElementsColumns::NAME)
    {
        return validator_->hasValidName(parameter.data());
    }
    else if (index.column() == ConfigurableElementsColumns::VALUE)
    {
        return validator_->hasValidValue(parameter.data(), component_->componentModel()->getChoices());
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int ConfigurableElementsModel::getAllReferencesToIdInItemOnRow(const int& row, QString valueID) const
{
    int valueReferences = configurableElements_.at(row)->getValue().count(valueID);
    int defaultReferences = configurableElements_.at(row)->getValueAttribute("spirit:defaultValue").count(valueID);

    int totalReferences = valueReferences + defaultReferences;
    return totalReferences;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::save()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::save() 
{
	// create the map that contains the new values.
	QMap<QString, QString> newValues;
    for (int i = 0; i < configurableElements_.size(); ++i)
    {
        if (configurableElements_.at(i)->hasAttribute("spirit:editedInConfigurableElements"))
        {
            newValues.insert(configurableElements_.at(i)->getValueId(), configurableElements_.at(i)->getValue());
        }
	}

	QSharedPointer<ComponentConfElementChangeCommand> cmd(new ComponentConfElementChangeCommand(component_,
		newValues));

	// disconnect the signal to avoid infinite loops of changes.
	disconnect(component_, SIGNAL(confElementsChanged(const QMap<QString, QString>&)),
		this, SLOT(changeElements(const QMap<QString, QString>&)));
	editProvider_->addCommand(cmd);
    cmd->redo();

	connect(component_, SIGNAL(confElementsChanged(const QMap<QString, QString>&)),
		this, SLOT(changeElements(const QMap<QString, QString>&)), Qt::UniqueConnection);

	// update the current data structure.
	currentElementValues_ = newValues;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::onRemove()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::onRemove( const QModelIndex& index ) 
{
    if (!index.isValid() ||
        !configurableElements_.at(index.row())->getValueAttribute("spirit:defaultValue").isEmpty())
    {
		return;
    }

	// remove the indexed configurable element
	beginRemoveRows(QModelIndex(), index.row(), index.row());
    configurableElements_.removeAt(index.row());
	endRemoveRows();
	
	// save the changes to the diagram component
	save();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::onRemoveItem( const QModelIndex& index ) 
{
	// don't remove anything if index is invalid
    if (!index.isValid() ||
        !configurableElements_.at(index.row())->getValueAttribute("spirit:defaultValue").isEmpty())
    {
		return;
	}
	// make sure the row number is valid
    else if (index.row() < 0 || index.row() >= configurableElements_.size())
    {
		return;
	}

	// remove the indexed configurable element
	beginRemoveRows(QModelIndex(), index.row(), index.row());
    configurableElements_.removeAt(index.row());
	endRemoveRows();

	// save the changes to the diagram component
	save();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::changeElements()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::changeElements( const QMap<QString, QString>& confElements ) 
{
	currentElementValues_ = confElements;
	readValues();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::readValues()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::readValues() 
{
	beginResetModel();

	// remove the items after the previous component instance
    configurableElements_.clear();

    readComponentConfigurableElements();

    // Insert any remaining items in the values_ map.
	for (QMap<QString, QString>::iterator elementIterator = currentElementValues_.begin();
        elementIterator != currentElementValues_.end(); ++elementIterator)
    {
        QString name = elementIterator.key();
        QString value = elementIterator.value();

        bool elementWasFound = false;

        foreach (QSharedPointer<Parameter> parameterElement, configurableElements_)
        {
            if (parameterElement->getValueId() == name)
            {
                parameterElement->setValue(value);
                parameterElement->setAttribute("spirit:editedInConfigurableElements", "true");
                elementWasFound = true;
                break;
            }
        }

        if (!elementWasFound)
        {
            QSharedPointer<Parameter> newParameter (new Parameter);
            newParameter->setName(name);
            newParameter->setValueId(name);
            newParameter->setValue(value);
            newParameter->setValueResolve("user");

            configurableElements_.append(newParameter);
        }
	}
    
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::readConfigurableElementsFromComponent()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::readComponentConfigurableElements()
{
    QSharedPointer<Component> componentModel = component_->componentModel();

    foreach (QSharedPointer<Parameter> parameterPointer, *componentModel->getParameters())
    {
        setParameterToConfigurableElements(parameterPointer);
    }

    foreach (QSharedPointer<Parameter> parameterPointer, *componentModel->getModelParameters())
    {
        setParameterToConfigurableElements(parameterPointer);
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::isParameterEditable()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::isParameterEditable(const int& parameterIndex) const
{
    if (configurableElements_.at(parameterIndex)->getValueResolve() == "immediate" ||
        configurableElements_.at(parameterIndex)->getValueResolve().isEmpty())
    {
        return false;
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::setParameterToConfigurableElements()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::setParameterToConfigurableElements(QSharedPointer<Parameter> parameterPointer)
{
    QSharedPointer<Parameter> newConfigurableElement (new Parameter(*(parameterPointer)));
    newConfigurableElement->setValueAttribute("spirit:defaultValue", newConfigurableElement->getValue());

    configurableElements_.append(newConfigurableElement);
}