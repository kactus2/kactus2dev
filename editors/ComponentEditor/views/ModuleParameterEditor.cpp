//-----------------------------------------------------------------------------
// File: ModuleParameterEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.02.2015
//
// Description:
// Editor for module parameters.
//-----------------------------------------------------------------------------

#include "ModuleParameterEditor.h"

#include "ModuleParameterFactoryImplementation.h"

#include <common/views/EditableTableView/ColumnFreezableTable.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <editors/ComponentEditor/modelParameters/ModelParameterColumns.h>
#include <editors/ComponentEditor/modelParameters/ModelParameterDelegate.h>
#include <editors/ComponentEditor/modelParameters/ModelParameterEditorHeaderView.h>
#include <editors/ComponentEditor/modelParameters/modelparametermodel.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>

#include <IPXACTmodels/component.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ModuleParameterEditor::ModuleParameterEditor()
//-----------------------------------------------------------------------------
ModuleParameterEditor::ModuleParameterEditor(QSharedPointer<QList<QSharedPointer<ModelParameter> > > parameters,
    QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QWidget *parent)
    : QGroupBox(tr("Module parameters"), parent), 
    model_(0)
{
    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));
    
    model_ = new ModelParameterModel(parameters, component->getChoices(), expressionParser, 
        parameterFinder, expressionFormatter, this);
    
    model_->setParameterFactory(QSharedPointer<ModelParameterFactory>(new ModuleParameterFactoryImplementation()));

    QSharedPointer<EditableTableView> moduleParameterView(new EditableTableView(this));
    moduleParameterView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    moduleParameterView->verticalHeader()->show();

    ColumnFreezableTable* view = new ColumnFreezableTable(1, moduleParameterView, this);

    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(model_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(view, SIGNAL(addItem(const QModelIndex&)), 
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(view, SIGNAL(removeItem(const QModelIndex&)),
        model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    ModelParameterEditorHeaderView* parameterHorizontalHeader = new ModelParameterEditorHeaderView(Qt::Horizontal, this);
    view->setHorizontalHeader(parameterHorizontalHeader);
    view->horizontalHeader()->setSectionsClickable(true);
    view->horizontalHeader()->setStretchLastSection(true);

    // set view to be sortable
    view->setSortingEnabled(true);

    // items can not be dragged
    view->setItemsDraggable(false);

    ComponentParameterModel* parameterModel = new ComponentParameterModel(parameterFinder, this);
    parameterModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(parameterModel);

    view->setDelegate(new ModelParameterDelegate(component->getChoices(), parameterCompleter, parameterFinder,
        expressionFormatter, this));

    connect(view->itemDelegate(), SIGNAL(increaseReferences(QString)), 
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(view->itemDelegate(), SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

    // set source model for proxy
    QSortFilterProxyModel* proxy_ = new QSortFilterProxyModel(this);
    proxy_->setSourceModel(model_);
    // set proxy to be the source for the view
    view->setModel(proxy_);

    // sort the view
    view->sortByColumn(0, Qt::AscendingOrder);

    view->setColumnHidden(ModelParameterColumns::ID, true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterEditor::~ModuleParameterEditor()
//-----------------------------------------------------------------------------
ModuleParameterEditor::~ModuleParameterEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterEditor::isValid()
//-----------------------------------------------------------------------------
bool ModuleParameterEditor::isValid() const
{
    return model_->isValid();
}
