//-----------------------------------------------------------------------------
// File: configurableelementeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 12.08.2011
//
// Description:
// Editor for configurable elements of a component instance.
//-----------------------------------------------------------------------------

#include "configurableelementeditor.h"

#include <designEditors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>
#include <designEditors/common/ComponentInstanceEditor/ComponentInstanceConfigurableElementsFilter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::ConfigurableElementEditor()
//-----------------------------------------------------------------------------
ConfigurableElementEditor::ConfigurableElementEditor(QSharedPointer<ConfigurableElementFinder> elementFinder,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> configurableElementFormatter,
    QSharedPointer<ExpressionFormatter> instanceExpressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionParser> instanceParser,
    QAbstractItemModel* completionModel, QWidget *parent):
QGroupBox(tr("Configurable element values"), parent),
view_(this),
model_(parameterFinder, elementFinder, configurableElementFormatter, instanceExpressionFormatter, expressionParser,
    instanceParser, this),
delegate_(0),
filterSelection_(new QCheckBox(tr("Show immediate values"), this))
{    
    ComponentInstanceConfigurableElementsFilter* filter (new ComponentInstanceConfigurableElementsFilter(this));
	filter->setSourceModel(&model_);
    filter->setSortCaseSensitivity(Qt::CaseInsensitive);
	view_.setModel(filter);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(completionModel);

	// set options for the view
	view_.setSortingEnabled(true);
    view_.setSelectionBehavior(QAbstractItemView::SelectItems);
    view_.setSelectionMode(QAbstractItemView::SingleSelection);

    delegate_ = new ConfigurableElementDelegate(parameterCompleter, parameterFinder, configurableElementFormatter, this);

    view_.setItemDelegate(delegate_);

    view_.setAlternatingRowColors(false);
    view_.setColumnHidden(ConfigurableElementsColumns::CHOICE, true);
    view_.setColumnHidden(ConfigurableElementsColumns::ARRAY_LEFT, true);
    view_.setColumnHidden(ConfigurableElementsColumns::ARRAY_RIGHT, true);
    view_.setColumnHidden(ConfigurableElementsColumns::TYPE, true);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&view_);
    topLayout->addWidget(filterSelection_);

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
        delegate_, SLOT(onCreateRemoveElementCommand(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeAllSubItems(QModelIndex const&)),
        delegate_, SLOT(onCreateMultipleElementRemoveCommands(QModelIndex const&)), Qt::UniqueConnection);
    connect(delegate_, SIGNAL(addConfigurableElement(QString const&, QString const&, QString const&, int)),
        &model_, SLOT(onAddElement(QString const&, QString const&, QString const&, int)), Qt::UniqueConnection);
    connect(delegate_, SIGNAL(removeConfigurableElement(QString const&, QString const&, int)),
        &model_, SLOT(onRemoveElement(QString const&, QString const&, int)), Qt::UniqueConnection);
    connect(delegate_, SIGNAL(dataChangedInID(QString const&, QString const&)),
        &model_, SLOT(onDataChangedInID(QString const&, QString const&)), Qt::UniqueConnection);
    connect(filterSelection_, SIGNAL(clicked(bool)),
        &model_, SIGNAL(showImmediateValuesInModels(bool)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(invalidateFilter()), filter, SLOT(onInvalidateFilter()), Qt::UniqueConnection);
    connect(filterSelection_, SIGNAL(clicked(bool)),
        filter, SLOT(setShowImmediateValues(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::~ConfigurableElementEditor()
//-----------------------------------------------------------------------------
ConfigurableElementEditor::~ConfigurableElementEditor() 
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::setComponent()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::setComponent(QSharedPointer<Component> component,
    QSharedPointer<ComponentInstance> instance, QSharedPointer<ViewConfiguration> viewConfiguration,
    QSharedPointer<IEditProvider> editProvider)
{
    model_.setParameters(component, instance, viewConfiguration);
    delegate_->setChoices(component->getChoices());
    delegate_->setEditProvider(editProvider);

    view_.expandAll();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::clear()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::clear() 
{
	model_.clear();
}
