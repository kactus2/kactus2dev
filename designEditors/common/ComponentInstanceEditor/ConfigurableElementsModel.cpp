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
#include <editors/ComponentEditor/common/ValueFormatter.h>

#include <IPXACTmodels/Enumeration.h>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::ConfigurableElementsModel()
//-----------------------------------------------------------------------------
ConfigurableElementsModel::ConfigurableElementsModel(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ListParameterFinder> listParameterFinder,
    QSharedPointer<ExpressionFormatter> configurableElementExpressionFormatter,
    QSharedPointer<ExpressionFormatter> componentInstanceExpressionFormatter,
    QSharedPointer<ExpressionParser> configurableElementExpressionParser,
    QSharedPointer<ExpressionParser> componentInstanceExpressionParser,
    QObject *parent):
QAbstractItemModel(parent),
ParameterizableTable(parameterFinder),
component_(0),
currentElementValues_(),
configurableElements_(new QList<QSharedPointer<Parameter> > ()),
editProvider_(0),
configurableElementExpressionFormatter_(configurableElementExpressionFormatter),
componentInstanceExpressionFormatter_(componentInstanceExpressionFormatter),
componentInstanceExpressionParser_(componentInstanceExpressionParser),
validator_(new ParameterValidator2014(configurableElementExpressionParser, parameterFinder)),
designConfiguration_(new DesignConfiguration()),
rootItems_()
{
    QString* parameters (new QString("Parameters"));
    QString* modelParameters (new QString("Model parameters"));
    QString* viewParameters (new QString("View parameters"));
    QString* moduleParameters (new QString("Module parameters"));
    QString* unknownElements (new QString("Unknown"));

    rootItems_.append(parameters);
    rootItems_.append(modelParameters);
    rootItems_.append(viewParameters);
    rootItems_.append(moduleParameters);
    rootItems_.append(unknownElements);

    listParameterFinder->setParameterList(configurableElements_);
    setExpressionParser(configurableElementExpressionParser);
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
	setupConfigurableElements();

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
    configurableElements_.data()->clear();
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::rowCount()
//-----------------------------------------------------------------------------
int ConfigurableElementsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const 
{
    if (!parent.isValid())
    {
        return rootItems_.size();
    }

    else
    {
        foreach (QString* rootItem, rootItems_)
        {
            if (rootItem == parent.internalPointer())
            {
                QString parentItemName = *rootItem;
                int numberOfRows = 0;

                foreach (QSharedPointer<Parameter> parameter, *configurableElements_)
                {
                    if (parameter->getAttribute("kactus2:rootItem") == parentItemName)
                    {
                        ++numberOfRows;
                    }
                }

                return numberOfRows;
            }
        }
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::columnCount()
//-----------------------------------------------------------------------------
int ConfigurableElementsModel::columnCount( const QModelIndex& /*parent = QModelIndex()*/ ) const 
{
    return ConfigurableElementsColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::data()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const 
{
    if (!isIndexValid(index))
    {
        return QVariant();
    }

    if (role == Qt::ForegroundRole)
    {
        if (index.parent().isValid())
        {
            QSharedPointer<Parameter> element = getIndexedConfigurableElement(index.parent(), index.row());

            if (element->getValueAttribute("kactus2:defaultValue").isEmpty())
            {
                return QColor(Qt::red);
            }
            else if (index.column() == ConfigurableElementsColumns::VALUE && isParameterEditable(index))
            {
                return blackForValidOrRedForInvalidIndex(index);
            }
            else
            {
                return QColor(Qt::gray);
            }
        }
        else
        {
            return QColor(Qt::black);
        }
    }

    else if (role == Qt::UserRole)
    {
        return isElementDeletable(index);
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

    else if (role == Qt::BackgroundRole)
    {
        if (!index.parent().isValid())
        {
            return QColor(230, 230, 230);
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::headerData()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::headerData( int section, Qt::Orientation orientation,
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
        else if (section == ConfigurableElementsColumns::ARRAY_LEFT)
        {
            return tr("Array left");
        }
        else if (section == ConfigurableElementsColumns::TYPE)
        {
            return tr("Type");
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
bool ConfigurableElementsModel::setData( const QModelIndex& index, const QVariant& value, 
    int role /*= Qt::EditRole */ ) 
{
    if (!isIndexValid(index))
    {
        return false;
    }

    if (index.parent().isValid())
    {
        if (role == Qt::EditRole) 
        {
            QSharedPointer<Parameter> element = getIndexedConfigurableElement(index.parent(), index.row());

            if (index.column() == ConfigurableElementsColumns::VALUE)
            {
                if (!value.isValid() || value.toString().isEmpty())
                {
                    element->setValue(element->getValueAttribute("kactus2:defaultValue"));
                    element->setAttribute("kactus2:editedInConfigurableElements", "");
                }

                else
                {
                    element->setAttribute("kactus2:editedInConfigurableElements", "true");
                    element->setValue(value.toString());
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
    }

    return false;
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
    if (index.parent().isValid())
    {
        QSharedPointer<Parameter> element = getIndexedConfigurableElement(index.parent(), index.row());

        if (element->getValueResolve() != "immediate" && !element->getValueResolve().isEmpty())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::setComponentActiveView()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::setDesignConfigurationModel(QSharedPointer<DesignConfiguration> designConfiguration)
{
    designConfiguration_ = designConfiguration.data();
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
    if (index.parent().isValid())
    {
        QSharedPointer<Parameter> targetElement = getIndexedConfigurableElement(index.parent(), index.row());

        if (index.column() == ConfigurableElementsColumns::NAME)
        {
            return targetElement->getName();
        }
        else if (index.column() == ConfigurableElementsColumns::VALUE)
        {
            return evaluateValue(index.column(), targetElement->getChoiceRef(), targetElement->getValue());
        }
        else if (index.column() == ConfigurableElementsColumns::DEFAULT_VALUE)
        {
            return evaluateValue(index.column(), targetElement->getChoiceRef(),
                targetElement->getValueAttribute("kactus2:defaultValue"));
        }
        else if (index.column() == ConfigurableElementsColumns::CHOICE)
        {
            return targetElement->getChoiceRef();
        }
        else if (index.column() == ConfigurableElementsColumns::ARRAY_LEFT)
        {
            return targetElement->getAttribute("kactus2:arrayLeft");
        }
        else if (index.column() == ConfigurableElementsColumns::TYPE)
        {
            return targetElement->getValueFormat();
        }
    }

    else if (!index.parent().isValid() && index.column() == ConfigurableElementsColumns::NAME)
    {
        return QString(*rootItems_.at(index.row()));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::getIndexedParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ConfigurableElementsModel::getIndexedConfigurableElement(QModelIndex const& parentIndex,
    int row) const
{
    foreach (QString* rootItem, rootItems_)
    {
        if (rootItem == parentIndex.internalPointer())
        {
            int childRow = 0;

            foreach (QSharedPointer<Parameter> element, *configurableElements_)
            {
                if (element->getAttribute("kactus2:rootItem") == *rootItem)
                {
                    if (childRow == row)
                    {
                        return element;
                    }
                    else
                    {
                        ++childRow;
                    }
                }
            }
        }
    }

    return QSharedPointer<Parameter> (new Parameter());
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
    if (componentInstanceExpressionParser_->isValidExpression(expression))
    {
        ValueFormatter formatter;
        return formatter.format(componentInstanceExpressionParser_->parseExpression(expression),
            componentInstanceExpressionParser_->baseForExpression(expression));
    }
    else if (componentInstanceExpressionParser_->isPlainValue(expression))
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
// Function: ConfigurableElementsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (index.parent().isValid())
    {
        QSharedPointer<Parameter> element = getIndexedConfigurableElement(index.parent(), index.row());

        if (index.column() == ConfigurableElementsColumns::NAME)
        {
            return element->getName();
        }
        else if (index.column() == ConfigurableElementsColumns::VALUE)
        {
            return element->getValue();
        }
        else if (index.column() == ConfigurableElementsColumns::DEFAULT_VALUE)
        {
            return element->getValueAttribute("kactus2:defaultValue");
        }
    }

    return data(index, Qt::DisplayRole);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::tooltipForIndex()
//-----------------------------------------------------------------------------
QString ConfigurableElementsModel::tooltipForIndex(QModelIndex const& index) const
{
    if (index.parent().isValid())
    {
        QSharedPointer<Parameter> element = getIndexedConfigurableElement(index.parent(), index.row());

        if (element->getValueAttribute("kactus2:defaultValue").isEmpty())
        {
            return QString("This parameter was not found in " + component_->name() + ".");
        }

        else if (index.column() == ConfigurableElementsColumns::VALUE)
        {
            QString context = component_->name();

            QStringList errorList = validator_->findErrorsIn(
                element.data(), context, component_->componentModel()->getChoices());

            if (!errorList.isEmpty())
            {
                QString errors = errorList.join("\n");
                return errors;
            }
        }
        else if (index.column() == ConfigurableElementsColumns::DEFAULT_VALUE)
        {
            return componentInstanceExpressionFormatter_->
                formatReferringExpression(expressionOrValueForIndex(index).toString());
        }
    }

    return configurableElementExpressionFormatter_->
        formatReferringExpression(expressionOrValueForIndex(index).toString());
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::validateColumnForParameter()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::validateColumnForParameter(QModelIndex const& index) const
{
    if (index.parent().isValid())
    {
        QSharedPointer<Parameter> element = getIndexedConfigurableElement(index.parent(), index.row());

        if (index.column() == ConfigurableElementsColumns::NAME)
        {
            return validator_->hasValidName(element.data());
        }
        else if (index.column() == ConfigurableElementsColumns::VALUE)
        {
            return validator_->hasValidValue(element.data(), component_->componentModel()->getChoices());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::save()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::save() 
{
	QMap<QString, QString> newValues;
    for (int i = 0; i < configurableElements_->size(); ++i)
    {
        if (configurableElements_->at(i)->hasAttribute("kactus2:editedInConfigurableElements"))
        {
            newValues.insert(configurableElements_->at(i)->getValueId(), configurableElements_->at(i)->getValue());
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

	currentElementValues_ = newValues;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::changeElements()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::changeElements( const QMap<QString, QString>& confElements ) 
{
    currentElementValues_ = confElements;
    setupConfigurableElements();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::onRemoveItem( const QModelIndex& index ) 
{
    if (!isIndexValid(index))
    {
        return;
    }

    if (!index.parent().isValid())
    {
        return;
    }
    
    QSharedPointer<Parameter> element = getIndexedConfigurableElement(index.parent(), index.row());

    if (element && element->getValueAttribute("kactus2:defaultValue").isEmpty())
    {
        beginRemoveRows(index.parent(), index.row(), index.row());
        configurableElements_->removeOne(element);
        endRemoveRows();
        save();
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::setupConfigurableElements()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::setupConfigurableElements() 
{
	beginResetModel();

    configurableElements_.data()->clear();

    readComponentConfigurableElements();

    restoreStoredConfigurableElements();
    
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::readComponentConfigurableElements()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::readComponentConfigurableElements()
{
    QSharedPointer<Component> componentModel = component_->componentModel();

    if (!componentModel->getParameters()->isEmpty())
    {
        foreach (QSharedPointer<Parameter> parameterPointer, *componentModel->getParameters())
        {
            addParameterToConfigurableElements(parameterPointer, "Parameters");
        }
    }

    if (!componentModel->getModelParameters()->isEmpty())
    {
        foreach (QSharedPointer<Parameter> parameterPointer, *componentModel->getModelParameters())
        {
            addParameterToConfigurableElements(parameterPointer, "Model parameters");
        }
    }

    if (designConfiguration_)
    {
        QString componentInstanceName = component_->name();

        if (designConfiguration_->hasActiveView(componentInstanceName))
        {
            QString activeViewName = designConfiguration_->getActiveView(componentInstanceName);
            View* activeView = component_->componentModel()->findView(activeViewName);

            if (!activeView->getParameters()->isEmpty())
            {
                foreach (QSharedPointer<Parameter> parameterPointer, *activeView->getParameters())
                {
                    addParameterToConfigurableElements(parameterPointer, "View parameters");
                }
            }

            if (!activeView->getModuleParameters()->isEmpty())
            {
                foreach (QSharedPointer<Parameter> parameterPointer, *activeView->getModuleParameters())
                {
                    addParameterToConfigurableElements(parameterPointer, "Module parameters");
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::addParameterToConfigurableElements()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::addParameterToConfigurableElements(QSharedPointer<Parameter> parameterPointer,
    QString const& rootItemName)
{
    QSharedPointer<Parameter> newConfigurableElement (new Parameter(*(parameterPointer)));
    newConfigurableElement->setValueAttribute("kactus2:defaultValue", parameterPointer->getValue());

    newConfigurableElement->setAttribute("kactus2:rootItem", rootItemName);

    configurableElements_->append(newConfigurableElement);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::readStoredConfigurableElements()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::restoreStoredConfigurableElements()
{
    for (QMap<QString, QString>::iterator elementIterator = currentElementValues_.begin();
        elementIterator != currentElementValues_.end(); ++elementIterator)
    {
        QString elementID = elementIterator.key();
        QString elementValue = elementIterator.value();

        QSharedPointer<Parameter> storedConfigurableElement = getStoredConfigurableElement(elementID);
        storedConfigurableElement->setValue(elementValue);
        storedConfigurableElement->setAttribute("kactus2:editedInConfigurableElements", "true");
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::getStoredConfigurableElement()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ConfigurableElementsModel::getStoredConfigurableElement(QString const& elementID)
{
    foreach(QSharedPointer<Parameter> parameterElement, *configurableElements_.data())
    {
        if (parameterElement->getValueId() == elementID)
        {
            return parameterElement;
        }
    }

    QSharedPointer<Parameter> newConfigurableElement(new Parameter);
    newConfigurableElement->setName(elementID);
    newConfigurableElement->setValueId(elementID);
    newConfigurableElement->setValueResolve("user");
    newConfigurableElement->setAttribute("kactus2:rootItem", "Unknown");
    configurableElements_->append(newConfigurableElement);

    return newConfigurableElement;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::index()
//-----------------------------------------------------------------------------
QModelIndex ConfigurableElementsModel::index(int row, int column, const QModelIndex &parent /* = QModelIndex() */)
    const
{
    if (!parent.isValid() && 0 <= row && row < rootItems_.size() )
    {
        return createIndex(row, column, rootItems_.at(row));
    }

    else 
    {
        QSharedPointer<Parameter> element = getIndexedConfigurableElement(parent, row);

        return createIndex(row, column, element.data());
    }

    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::parent()
//-----------------------------------------------------------------------------
QModelIndex ConfigurableElementsModel::parent(const QModelIndex & child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    foreach (QString* rootItem, rootItems_)
    {
        if (rootItem == child.internalPointer())
        {
            return QModelIndex();
        }
    }

    Parameter* childItem = static_cast<Parameter*>(child.internalPointer());
    QString parentName = childItem->getAttribute("kactus2:rootItem");

    for (int i = 0; i < rootItems_.size(); ++i)
    {
        if (parentName == *rootItems_.at(i))
        {
            return createIndex(i, 0, rootItems_.at(i));
        }
    }

    // This should never be reached.
    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::isIndexValid()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::isIndexValid(QModelIndex const& index) const
{
    if (!index.parent().isValid())
    {
        if (index.isValid() && 0 <= index.row() && index.row() < rootItems_.size())
        {
            return true;
        }
    }

    else
    {
        QString parentName = *static_cast<QString*>(index.parent().internalPointer());
        
        int elementsWithThisParent = 0;
        foreach (QSharedPointer<Parameter> element, *configurableElements_)
        {
            if (element->getAttribute("kactus2:rootItem") == parentName)
            {
                ++elementsWithThisParent;
            }
        }

        if (index.isValid() && 0 <= index.row() && index.row() < elementsWithThisParent)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::isElementDeletable()
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::isElementDeletable(QModelIndex const& index) const
{
    QSharedPointer<Parameter> element = getIndexedConfigurableElement(index.parent(), index.row());

    if (element->getValueAttribute("kactus2:defaultValue").isEmpty())
    {
        return true;
    }
    else
    {
        return false;
    }
}