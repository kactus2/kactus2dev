//-----------------------------------------------------------------------------
// File: ComponentInstanceConfigurableElementsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.08.2017
//
// Description:
// Model class to manage the configurable element values of component instances.
//-----------------------------------------------------------------------------

#include "ComponentInstanceConfigurableElementsModel.h"

#include <common/KactusColors.h>

#include <editors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>
#include <editors/common/ComponentInstanceEditor/ConfigurableElementsModel.h>
#include <editors/common/ComponentInstanceEditor/ParameterConfigurableElementsFilter.h>
#include <editors/common/ComponentInstanceEditor/MissingConfigurableElementsFilter.h>
#include <editors/common/ComponentInstanceEditor/EditorConfigurableElement.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ModuleParameter.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/designConfiguration/ViewConfiguration.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::ComponentInstanceConfigurableElementsModel()
//-----------------------------------------------------------------------------
ComponentInstanceConfigurableElementsModel::ComponentInstanceConfigurableElementsModel(
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ConfigurableElementFinder> elementFinder,
    QSharedPointer<ExpressionFormatter> elementExpressionFormatter,
    QSharedPointer<ExpressionFormatter> defaultValueFormatter,
    QSharedPointer<ExpressionParser> elementExpressionParser, QSharedPointer<ExpressionParser> defaultValueParser,
    QObject* parent):
QAbstractTableModel(parent),
rootItems_(),
parametersRoot_(new rootModel()),
moduleParametersRoot_(new rootModel()),
missingParametersRoot_(new rootModel()),
unknownParametersRoot_(new rootModel()),
elementFinder_(elementFinder)
{
    ConfigurableElementsModel* parametersModel (new ConfigurableElementsModel(parameterFinder,
        elementExpressionFormatter, defaultValueFormatter, elementExpressionParser, defaultValueParser, this));
    ParameterConfigurableElementsFilter* parameterFilter (new ParameterConfigurableElementsFilter(this));
    parameterFilter->setSourceModel(parametersModel);

    parametersRoot_->elementModel_ = parametersModel;
    parametersRoot_->elementFilter_ = parameterFilter;
    parametersRoot_->name_ = QStringLiteral("Parameters");

    ConfigurableElementsModel* missingParametersModel (new ConfigurableElementsModel(parameterFinder,
        elementExpressionFormatter, defaultValueFormatter, elementExpressionParser, defaultValueParser, this));
    MissingConfigurableElementsFilter* missingParametersFilter(new MissingConfigurableElementsFilter(this));
    missingParametersFilter->setSourceModel(missingParametersModel);

    missingParametersRoot_->elementModel_ = missingParametersModel;
    missingParametersRoot_->elementFilter_ = missingParametersFilter;
    missingParametersRoot_->name_ = QStringLiteral("Missing parameters");

    ConfigurableElementsModel* moduleParametersModel (new ConfigurableElementsModel(parameterFinder,
        elementExpressionFormatter, defaultValueFormatter, elementExpressionParser, defaultValueParser, this));
    ParameterConfigurableElementsFilter* moduleParameterFilter (new ParameterConfigurableElementsFilter(this));
    moduleParameterFilter->setSourceModel(moduleParametersModel);

    moduleParametersRoot_->elementModel_ = moduleParametersModel;
    moduleParametersRoot_->elementFilter_ = moduleParameterFilter;
    moduleParametersRoot_->name_ = QStringLiteral("Module parameters");

    ConfigurableElementsModel* unkownParametersModel (new ConfigurableElementsModel(parameterFinder,
        elementExpressionFormatter, defaultValueFormatter, elementExpressionParser, defaultValueParser, this));
    MissingConfigurableElementsFilter* unknownParametersFilter(new MissingConfigurableElementsFilter(this));
    unknownParametersFilter->setSourceModel(unkownParametersModel);

    unknownParametersRoot_->elementModel_ = unkownParametersModel;
    unknownParametersRoot_->elementFilter_ = unknownParametersFilter;
    unknownParametersRoot_->name_ = QStringLiteral("Missing view parameters");

    rootItems_.append(parametersRoot_);
    rootItems_.append(missingParametersRoot_);
    rootItems_.append(moduleParametersRoot_);
    rootItems_.append(unknownParametersRoot_);

    connect(this, SIGNAL(showImmediateValuesInModels(bool)), parameterFilter,
        SLOT(setShowImmediateValues(bool)), Qt::UniqueConnection);
    connect(this, SIGNAL(showImmediateValuesInModels(bool)), moduleParameterFilter,
        SLOT(setShowImmediateValues(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::ComponentInstanceConfigurableElementsModel()
//-----------------------------------------------------------------------------
ComponentInstanceConfigurableElementsModel::~ComponentInstanceConfigurableElementsModel() 
{
    
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::setParameters()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsModel::setParameters(QSharedPointer<Component> component,
    QSharedPointer<ComponentInstance> instance, QSharedPointer<ViewConfiguration> viewConfiguration)
{
    QString instanceName = instance->getInstanceName();
    QSharedPointer<QList<QSharedPointer<Choice> > > choices = component->getChoices();
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters = component->getParameters();
    QSharedPointer<QList<QSharedPointer<Parameter> > > moduleParameters =
        getModuleParameters(component, viewConfiguration);

    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > instanceConfigurableElements;
    if (instance)
    {
        instanceConfigurableElements = instance->getConfigurableElementValues();
    }
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > viewConfigurationElements;
    if (viewConfiguration)
    {
        viewConfigurationElements = viewConfiguration->getViewConfigurableElements();
    }

    beginResetModel();

    parametersRoot_->elementModel_->setParameters(instanceName, parameters, choices, instanceConfigurableElements);
    moduleParametersRoot_->elementModel_->setParameters(
        instanceName, moduleParameters, choices, viewConfigurationElements);

    QSharedPointer<QList<QSharedPointer<Parameter> > > viewConfigurationParameters =
        gatherAllViewConfigurationParameters(moduleParameters);

    missingParametersRoot_->elementModel_->setParameters(
        instanceName, parameters, choices, instanceConfigurableElements);
    unknownParametersRoot_->elementModel_->setParameters(instanceName, viewConfigurationParameters, choices,
        viewConfigurationElements);

    QList<QSharedPointer<EditorConfigurableElement> > constructedElements;

    QVariant elementVariant = parametersRoot_->elementModel_->data(
        QModelIndex(), ConfigurableElementsModel::getConfigurableElementsFromTableRole);
    if (elementVariant.canConvert<QList<QSharedPointer<EditorConfigurableElement> > >())
    {
        constructedElements.append(elementVariant.value<QList<QSharedPointer<EditorConfigurableElement> > >());
    }

    QVariant moduleElementVariant = moduleParametersRoot_->elementModel_->data(
        QModelIndex(), ConfigurableElementsModel::getConfigurableElementsFromTableRole);
    if (moduleElementVariant.canConvert<QList<QSharedPointer<EditorConfigurableElement> > >())
    {
        constructedElements.append(moduleElementVariant.value
            <QList<QSharedPointer<EditorConfigurableElement> > >());
    }

    elementFinder_->setConfigurableElementList(constructedElements);

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::getModuleParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > ComponentInstanceConfigurableElementsModel::
    getModuleParameters(QSharedPointer<Component> component, QSharedPointer<ViewConfiguration> viewConfiguration)
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters (new QList<QSharedPointer<Parameter> > ());

    if (viewConfiguration && !viewConfiguration->getViewReference().isEmpty())
    {
        QString referencedView = viewConfiguration->getViewReference();
        foreach (QSharedPointer<View> view, *component->getViews())
        {
            if (view->name() == referencedView)
            {
                QString referencedComponentInstantiation = view->getComponentInstantiationRef();
                if (!referencedComponentInstantiation.isEmpty())
                {
                    foreach (QSharedPointer<ComponentInstantiation> instantiation,
                        *component->getComponentInstantiations())
                    {
                        if (instantiation->name() == referencedComponentInstantiation)
                        {
                            foreach (QSharedPointer<ModuleParameter> moduleParameter,
                                *instantiation->getModuleParameters())
                            {
                                parameters->append(moduleParameter);
                            }
                            break;
                        }
                    }
                }

                break;
            }
        }
    }

    return parameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::gatherAllViewConfigurationParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > ComponentInstanceConfigurableElementsModel::
    gatherAllViewConfigurationParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > moduleParameters)
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > viewConfigurationParameters(
        new QList<QSharedPointer<Parameter> > ());

    foreach (QSharedPointer<Parameter> parameter, *moduleParameters)
    {
        viewConfigurationParameters->append(parameter);
    }

    return viewConfigurationParameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::clear()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsModel::clear() 
{
	beginResetModel();

    foreach (QSharedPointer<rootModel> rootItem, rootItems_)
    {
        rootItem->elementModel_->clear();
    }

	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::rowCount()
//-----------------------------------------------------------------------------
int ComponentInstanceConfigurableElementsModel::rowCount(QModelIndex const& parent) const 
{
    if (!parent.isValid())
    {
        return rootItems_.size();
    }

    foreach (QSharedPointer<rootModel> root, rootItems_)
    {
        if (root == parent.internalPointer())
        {
            int rootRowCount = 0;

            if (root->elementFilter_ != 0)
            {
                rootRowCount = root->elementFilter_->rowCount();
            }
            else
            {
                rootRowCount = root->elementModel_->rowCount();
            }

            return rootRowCount;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::columnCount()
//-----------------------------------------------------------------------------
int ComponentInstanceConfigurableElementsModel::columnCount(QModelIndex const& /*parent*/) const 
{
    return ConfigurableElementsColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::data()
//-----------------------------------------------------------------------------
QVariant ComponentInstanceConfigurableElementsModel::data(QModelIndex const& index, int role) const 
{
    if (!isIndexValid(index))
    {
        return QVariant();
    }

    if (index.parent().isValid())
    {
        QSharedPointer<rootModel> root = rootItems_.at(index.parent().row());

        if (root->elementFilter_)
        {
            QModelIndex filteredIndex = root->elementFilter_->index(index.row(), index.column(), QModelIndex());
            return root->elementFilter_->data(filteredIndex, role);
        }
        else
        {
            return root->elementModel_->data(index, role);
        }
    }
    else
    {
        QSharedPointer<rootModel> rootItem = rootItems_.at(index.row());

        if (role == Qt::ForegroundRole)
        {
            if (rootItem->name_.contains(QStringLiteral("Missing")))
            {
                return KactusColors::ERROR;
            }
            else
            {
                return KactusColors::REGULAR_TEXT;
            }
        }
        else if ((index.column() == ConfigurableElementsColumns::NAME && role == Qt::DisplayRole) ||
            role == Qt::EditRole || role == Qt::ToolTipRole)
        {
            return rootItem->name_;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::headerData()
//-----------------------------------------------------------------------------
QVariant ComponentInstanceConfigurableElementsModel::headerData(int section, Qt::Orientation orientation, int role)
    const 
{
    return parametersRoot_->elementModel_->headerData(section, orientation, role);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::setData()
//-----------------------------------------------------------------------------
bool ComponentInstanceConfigurableElementsModel::setData(QModelIndex const& index, QVariant const& value, int role) 
{
    if (!isIndexValid(index))
    {
        return false;
    }

    if (index.parent().isValid())
    {
        QSharedPointer<rootModel> root = rootItems_.at(index.parent().row());
        QModelIndex filteredIndex = root->elementFilter_->index(index.row(), index.column());

        if (root->elementFilter_->setData(filteredIndex, value, role))
        {
            emit contentChanged();
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ComponentInstanceConfigurableElementsModel::flags(QModelIndex const& index) const 
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }
    else if (index.parent().isValid())
    {
        QSharedPointer<rootModel> root = rootItems_.at(index.parent().row());
        QModelIndex filteredIndex = root->elementFilter_->index(index.row(), index.column(), QModelIndex());

        return root->elementFilter_->flags(filteredIndex);
    }
    else
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::index()
//-----------------------------------------------------------------------------
QModelIndex ComponentInstanceConfigurableElementsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid() && 0 <= row && row < rootItems_.size())
    {
        return createIndex(row, column, rootItems_.at(row).data());
    }
    else
    {
        QSharedPointer<rootModel> root = rootItems_.at(parent.row());

        QSharedPointer<EditorConfigurableElement> element(0);
        if (root->elementFilter_)
        {
            QModelIndex filteredIndex = root->elementFilter_->index(row, column, QModelIndex());

            QVariant elementVariant = root->elementFilter_->data(
                filteredIndex, ConfigurableElementsModel::getConfigurableElementsFromTableRole);
            if (elementVariant.canConvert<QList<QSharedPointer<EditorConfigurableElement> > >())
            {
                element = elementVariant.value<QList<QSharedPointer<EditorConfigurableElement> > >().at(row);
            }
        }
        else
        {
            element = root->elementModel_->getConfigurableElements().at(row);
        }

        if (element)
        {
            return createIndex(row, column, element.data());
        }
    }

    //! This should not be reached.
    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::parent()
//-----------------------------------------------------------------------------
QModelIndex ComponentInstanceConfigurableElementsModel::parent(const QModelIndex & child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    for (int i = 0; i < rootItems_.size(); ++i)
    {
        QSharedPointer<rootModel> root = rootItems_.at(i);

        if (root.data() == child.internalPointer())
        {
            return QModelIndex();
        }
        else
        {
            EditorConfigurableElement* childElement =
                static_cast<EditorConfigurableElement*>(child.internalPointer());
            if (childElement)
            {
                for (int i = 0; i < rootItems_.size(); ++i)
                {
                    QSharedPointer<rootModel> root = rootItems_.at(i);
                    QList<QSharedPointer<EditorConfigurableElement> > elementList;

                    if (root->elementFilter_)
                    {
                        QVariant elementVariant = root->elementModel_->data(
                            child, ConfigurableElementsModel::getConfigurableElementsFromTableRole);
                        if (elementVariant.canConvert<QList<QSharedPointer<EditorConfigurableElement> > >())
                        {
                            elementList =
                                elementVariant.value<QList<QSharedPointer<EditorConfigurableElement> > >();
                        }
                    }
                    else
                    {
                        elementList = root->elementModel_->getConfigurableElements();
                    }

                    foreach (QSharedPointer<EditorConfigurableElement> element, elementList)
                    {
                        if (element.data() == childElement)
                        {
                            return createIndex(i, 0, root.data());
                        }
                    }
                }
            }
        }
    }

    // This should never be reached.
    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::isIndexValid()
//-----------------------------------------------------------------------------
bool ComponentInstanceConfigurableElementsModel::isIndexValid(QModelIndex const& index) const
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
        QSharedPointer<rootModel> root = rootItems_.at(index.parent().row());
        return root->elementModel_->isIndexValid(index);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::onDataChangedInID()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsModel::onDataChangedInID(QString const& parameterID,
    QString const& newDataValue)
{
    QSharedPointer<rootModel> root = getRootModelContainingReferencedID(parameterID);
    if (root)
    {
        root->elementModel_->emitDataChangeForID(parameterID, newDataValue);

        if (elementListContainsID(parameterID, missingParametersRoot_->elementModel_->getConfigurableElements()))
        {
            missingParametersRoot_->elementModel_->emitDataChangeForID(parameterID, newDataValue);
        }
        else if (elementListContainsID(
            parameterID, unknownParametersRoot_->elementModel_->getConfigurableElements()))
        {
            unknownParametersRoot_->elementModel_->emitDataChangeForID(parameterID, newDataValue);
        }

        QModelIndex startIndex = index(0, ConfigurableElementsColumns::VALUE, QModelIndex());
        QModelIndex endIndex = index(rootItems_.size() - 1, ConfigurableElementsColumns::VALUE, QModelIndex());

        emit dataChanged(startIndex, endIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::getRootModelContainingReferencedID()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstanceConfigurableElementsModel::rootModel> ComponentInstanceConfigurableElementsModel::
    getRootModelContainingReferencedID(QString const& referencedID) const
{
    foreach (QSharedPointer<EditorConfigurableElement> element,
        parametersRoot_->elementModel_->getConfigurableElements())
    {
        if (element->getReferencedParameter() && element->getReferencedParameter()->getValueId() == referencedID)
        {
            return parametersRoot_;
        }
    }

    foreach (QSharedPointer<EditorConfigurableElement> element,
        moduleParametersRoot_->elementModel_->getConfigurableElements())
    {
        if (element->getReferencedParameter() && element->getReferencedParameter()->getValueId() == referencedID)
        {
            return moduleParametersRoot_;
        }
    }

    return QSharedPointer<rootModel>();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::elementListContainsID()
//-----------------------------------------------------------------------------
bool ComponentInstanceConfigurableElementsModel::elementListContainsID(QString const& parameterID,
    QList<QSharedPointer<EditorConfigurableElement> > elementList) const
{
    foreach (QSharedPointer<EditorConfigurableElement> element, elementList)
    {
        if (element->getReferencedParameter() && element->getReferencedParameter()->getValueId() == parameterID)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::onAddElement()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsModel::onAddElement(QString const& elementID,
    QString const& elementValue, QString const& parentName, int elementRow)
{
    foreach (QSharedPointer<rootModel> root, rootItems_)
    {
        if (root->name_ == parentName)
        {
            root->elementModel_->onAddItem(elementID, elementValue, elementRow);
            root->elementFilter_->invalidate();

            emit invalidateFilter();
            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsModel::onRemoveElement()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsModel::onRemoveElement(QString const& elementID,
    QString const& parentName, int elementRow)
{
    foreach (QSharedPointer<rootModel> root, rootItems_)
    {
        if (root->name_ == parentName)
        {
            root->elementModel_->onRemoveItem(elementID, elementRow);
            root->elementFilter_->invalidate();

            emit invalidateFilter();
            emit contentChanged();
        }
    }
}
