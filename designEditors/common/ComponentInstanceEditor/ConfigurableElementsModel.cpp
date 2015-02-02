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

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::ConfigurableElementsModel()
//-----------------------------------------------------------------------------
ConfigurableElementsModel::ConfigurableElementsModel(QSharedPointer<ExpressionFormatter> expressionFormatter,
    QObject *parent):
QAbstractTableModel(parent),
ParameterizableTable(),
component_(0),
currentElementValues_(),
visibleConfigurableElements_(),
editProvider_(0),
expressionFormatter_(expressionFormatter)
{

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
	visibleConfigurableElements_.clear();
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
	return visibleConfigurableElements_.size();
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

	return 3;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::data()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= visibleConfigurableElements_.size())
    {
		return QVariant();
    }

    if (role == Qt::ForegroundRole)
    {
        if (visibleConfigurableElements_.at(index.row()).defaultValue_.isEmpty())
        {
            return QColor(Qt::red);
        }
        
        else if (visibleConfigurableElements_.at(index.row()).isEditable_ &&
            index.column() == ConfigurableElementsColumns::VALUE)
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
	if (!index.isValid())
    {
		return false;
    }
	else if (index.row() < 0 || index.row() >= visibleConfigurableElements_.size())
    {
		return false;
    }

	if (role == Qt::EditRole) 
    {
        if (index.column() == ConfigurableElementsColumns::VALUE)
        {
            visibleConfigurableElements_[index.row()].value_ = value.toString();
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

    if (!visibleConfigurableElements_.at(index.row()).isEditable_)
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
        return visibleConfigurableElements_.at(index.row()).name_;
    }
    else if (index.column() == ConfigurableElementsColumns::VALUE)
    {
        return formattedValueFor(visibleConfigurableElements_.at(index.row()).value_);
    }
    else if (index.column() == ConfigurableElementsColumns::DEFAULT_VALUE)
    {
        return formattedValueFor(visibleConfigurableElements_.at(index.row()).defaultValue_);
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant ConfigurableElementsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (index.column() == ConfigurableElementsColumns::NAME)
    {
        return visibleConfigurableElements_.at(index.row()).name_;
    }
    else if (index.column() == ConfigurableElementsColumns::VALUE)
    {
        return visibleConfigurableElements_.at(index.row()).value_;
    }
    else if (index.column() == ConfigurableElementsColumns::DEFAULT_VALUE)
    {
        return visibleConfigurableElements_.at(index.row()).defaultValue_;
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
    QString value = visibleConfigurableElements_.at(index.row()).value_;

    return isValuePlainOrExpression(value);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::save()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::save() 
{
	// create the map that contains the new values.
	QMap<QString, QString> newValues;
	for (int i = 0; i < visibleConfigurableElements_.size(); ++i) 
    {
		newValues.insert(visibleConfigurableElements_.at(i).uuID_, visibleConfigurableElements_.at(i).value_); 
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
	if (!index.isValid() || !visibleConfigurableElements_.at(index.row()).defaultValue_.isEmpty())
		return;

	// remove the indexed configurable element
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	visibleConfigurableElements_.removeAt(index.row());
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
	if (!index.isValid() || !visibleConfigurableElements_.at(index.row()).defaultValue_.isEmpty())
    {
		return;
	}
	// make sure the row number is valid
	else if (index.row() < 0 || index.row() >= visibleConfigurableElements_.size()) {
		return;
	}

	// remove the indexed configurable element
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	visibleConfigurableElements_.removeAt(index.row());
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
	visibleConfigurableElements_.clear();

    readComponentConfigurableElements();

    // Insert any remaining items in the values_ map.
	for (QMap<QString, QString>::iterator i = currentElementValues_.begin(); i != currentElementValues_.end(); ++i)
    {
        QString name = i.key();
        QString value = i.value();

        visibleConfigurableElements_.append(ConfigurableElement(name, value));
	}
    
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::readConfigurableElementsFromComponent()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::readComponentConfigurableElements()
{
    QSharedPointer <Component> componentModel = component_->componentModel();

    QString unEditableResolveValue = "immediate";

    readConfigurableParameters(componentModel, unEditableResolveValue);
    readConfigurableModelParameters(componentModel, unEditableResolveValue);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::readConfigurableParameters()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::readConfigurableParameters(QSharedPointer<Component> componentModel,
    QString unEditableResolve)
{
    QList < QSharedPointer <Parameter> > componentParameters = componentModel->getParameters();

    foreach (QSharedPointer <Parameter> parameterPointer, componentParameters)
    {
        addParameterWithIDToVisibleElements(parameterPointer, unEditableResolve);
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::readConfigurableModelParameters()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::readConfigurableModelParameters(QSharedPointer<Component> componentModel,
    QString unEditableResolve)
{
    QList < QSharedPointer < ModelParameter > > componentModelParameters = componentModel->getModelParameters();

    foreach (QSharedPointer < Parameter > modelParameterPointer, componentModelParameters)
    {
        addParameterWithIDToVisibleElements(modelParameterPointer, unEditableResolve);
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::addParameterWithIdToVisibleElements()
//-----------------------------------------------------------------------------
void ConfigurableElementsModel::addParameterWithIDToVisibleElements(
    QSharedPointer<Parameter> parameterPointer, QString unEditableResolveValue)
{
    if (!parameterPointer->getValueId().isEmpty())
    {
        QString name = parameterPointer->getName();
        QString value = parameterPointer->getValue();
        QString uuId = parameterPointer->getValueId();
        QString defaultValue = parameterPointer->getValue();
        
        bool isEditable = true;

        if (parameterPointer->getValueResolve() == unEditableResolveValue ||
            parameterPointer->getValueResolve().isEmpty())
        {
            isEditable = false;
        }

        if (currentElementValues_.contains(uuId))
        {
            value = currentElementValues_.take(uuId);
        }
        visibleConfigurableElements_.append(ConfigurableElement(name, uuId, value, defaultValue, isEditable));
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::ConfigurableElement::ConfigurableElements()
//-----------------------------------------------------------------------------
ConfigurableElementsModel::ConfigurableElement::ConfigurableElement(
    const QString& name, const QString& uuID,
    const QString& value, const QString& defaultValue, const bool& isEditable):
name_(name),
uuID_(uuID),
value_(value),
defaultValue_(defaultValue),
isEditable_(isEditable)
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::ConfigurableElement::ConfigurableElements()
//-----------------------------------------------------------------------------
ConfigurableElementsModel::ConfigurableElement::ConfigurableElement(const QString& name, const QString& value):
name_(name),
uuID_(name),
value_(value),
defaultValue_(),
isEditable_(true)
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsModel::ConfigurableElement::operator==
//-----------------------------------------------------------------------------
bool ConfigurableElementsModel::ConfigurableElement::operator==(const ConfigurableElement& other ) const 
{
    return uuID_ == other.uuID_;
}
