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

#include <editors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>
#include <editors/common/ComponentInstanceEditor/ComponentInstanceConfigurableElementsFilter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::ConfigurableElementEditor()
//-----------------------------------------------------------------------------
ConfigurableElementEditor::ConfigurableElementEditor(
    ExpressionSet parameterExpressions,
    ExpressionSet defaultExpressions, 
     QAbstractItemModel* completionModel,
    QWidget *parent) :
QGroupBox(tr("Configurable element values"), parent),
view_(this),
delegate_(new ConfigurableElementDelegate(completionModel, parameterExpressions.finder, parameterExpressions.formatter, this)),
filter_(new ConfigurableElementsFilter(this)),
model_(new ConfigurableElementsModel(parameterExpressions.finder, parameterExpressions.formatter, defaultExpressions.formatter,
    parameterExpressions.parser, defaultExpressions.parser, this)),
filterSelection_(new QCheckBox(tr("Show immediate values"), this))
{
    setFlat(true);

	// set options for the view
	view_.setSortingEnabled(true);
    view_.setSelectionBehavior(QAbstractItemView::SelectItems);
    view_.setSelectionMode(QAbstractItemView::SingleSelection);

    view_.setItemDelegate(delegate_);

    view_.setModel(filter_);

    filter_->setSourceModel(model_);
    filter_->setSortCaseSensitivity(Qt::CaseInsensitive);
    
    hideUnnecessaryColumns();

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&view_);
    topLayout->addWidget(filterSelection_);

    connect(&view_, SIGNAL(removeItem(QModelIndex const&)),
        delegate_, SLOT(onCreateRemoveElementCommands(QModelIndex const&)), Qt::UniqueConnection);

    connect(delegate_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(delegate_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    
    connect(filterSelection_, SIGNAL(clicked(bool)),
        filter_, SLOT(setShowImmediateValues(bool)), Qt::UniqueConnection);

    connect(delegate_, SIGNAL(removeConfigurableElement(QString const&, int)),
        this, SLOT(sendSignalForElementRemoval(QString const&, int)), Qt::UniqueConnection);
    connect(delegate_, SIGNAL(dataChangedInID(QString const&, QString const&)),
        model_, SLOT(emitDataChangeForID(QString const&, QString const&)), Qt::UniqueConnection);

    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createElementChangeCommand(
        QString const&, QString const&, QModelIndex const&, QAbstractItemModel*)),
        delegate_, SLOT(createElementChangeCommand(
        QString const&, QString const&, QModelIndex const&, QAbstractItemModel*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::setParameters()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::setParameters(QString const& containingItemName,
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
    QSharedPointer<QList<QSharedPointer<Choice> > > choices,
    Document::Revision docRevision,
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > storedConfigurableElements)
{
    model_->setParameters(containingItemName, parameters, choices, docRevision, storedConfigurableElements);
    delegate_->setStdRevision(docRevision);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::clear()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::clear()
{
    model_->clear();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::setEditProvider()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::setEditProvider(QSharedPointer<IEditProvider> editProvider)
{
    delegate_->setEditProvider(editProvider);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::hideUnnecessaryColumns()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::hideUnnecessaryColumns()
{
    view_.setAlternatingRowColors(false);
    view_.setColumnHidden(ConfigurableElementsColumns::CHOICE, true);
    view_.setColumnHidden(ConfigurableElementsColumns::ARRAY_LEFT, true);
    view_.setColumnHidden(ConfigurableElementsColumns::ARRAY_RIGHT, true);
}

//-----------------------------------------------------------------------------
// Function: InstantiationConfigurableElementEditor::sendSignalForElementRemoval()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::sendSignalForElementRemoval(QString const& elementID,
    int elementRow)
{
    model_->onRemoveItem(elementID, elementRow);
    filter_->invalidate();
}
