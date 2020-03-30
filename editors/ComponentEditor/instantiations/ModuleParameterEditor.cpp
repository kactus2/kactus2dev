//-----------------------------------------------------------------------------
// File: ModuleParameterEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 19.02.2015
//
// Description:
// Editor for module parameters.
//-----------------------------------------------------------------------------

#include "ModuleParameterEditor.h"

#include "ModuleParameterModel.h"
#include "ModuleParameterDelegate.h"
#include "ModuleParameterHeaderView.h"
#include "ModuleParameterColumns.h"
#include "ModuleParameterHeaderView.h"

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ParametersView.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/parameters/ParametersInterface.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ModuleParameterEditor::ModuleParameterEditor()
//-----------------------------------------------------------------------------
ModuleParameterEditor::ModuleParameterEditor(QSharedPointer<QList<QSharedPointer<ModuleParameter>>> parameters,
    QSharedPointer<QList<QSharedPointer<Choice>>> componentChoices,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QWidget* parent):
QGroupBox(tr("Module parameters"), parent),
proxy_(new QSortFilterProxyModel(this)),
model_(0),
parameterInterface_(new ParametersInterface()),
view_(0)
{
    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    parameterInterface_->setExpressionParser(expressionParser);
    parameterInterface_->setExpressionFormatter(expressionFormatter);
    parameterInterface_->setValidator(
        QSharedPointer<ParameterValidator>(new ParameterValidator(expressionParser, componentChoices)));
    parameterInterface_->setChoices(componentChoices);

    model_ = new ModuleParameterModel(parameterInterface_, expressionParser, parameterFinder, this);

    view_ = new ParametersView(this);

    ModuleParameterHeaderView* horizontalHeader = new ModuleParameterHeaderView(Qt::Horizontal, view_);
    view_->setHorizontalHeader(horizontalHeader);

    view_->verticalHeader()->setMaximumWidth(300);
    view_->verticalHeader()->setMinimumWidth(horizontalHeader->fontMetrics().width(tr("Name"))*2);
    view_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    view_->verticalHeader()->show();
  
    ComponentParameterModel* parameterModel = new ComponentParameterModel(parameterFinder, this);
    parameterModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(parameterModel);

    view_->setItemDelegate(new ModuleParameterDelegate(componentChoices, parameterCompleter, parameterFinder,
        expressionFormatter, this));

    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(model_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(view_, SIGNAL(addItem(const QModelIndex&)), 
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(view_, SIGNAL(removeItem(const QModelIndex&)),
        model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)), 
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(view_->itemDelegate(), SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

    connect(view_, SIGNAL(recalculateReferenceToIndexes(QModelIndexList)),
        model_, SLOT(onGetParametersMachingIndexes(QModelIndexList)), Qt::UniqueConnection);
    connect(model_,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, QSharedPointer<ParametersInterface>)),
        this,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, QSharedPointer<ParametersInterface>)),
        Qt::UniqueConnection);

    view_->setSortingEnabled(true);
    view_->setItemsDraggable(false);

    proxy_->setSourceModel(model_);
    view_->setModel(proxy_);

    view_->sortByColumn(0, Qt::AscendingOrder);
    
    view_->setColumnHidden(ModuleParameterColumns::ID, true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view_);

    setModuleParameters(parameters);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterEditor::~ModuleParameterEditor()
//-----------------------------------------------------------------------------
ModuleParameterEditor::~ModuleParameterEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterEditor::refresh()
//-----------------------------------------------------------------------------
void ModuleParameterEditor::refresh()
{
    proxy_->invalidate();

    if (!view_->isColumnHidden(ModuleParameterColumns::ID))
    {
        view_->setColumnHidden(ModuleParameterColumns::ID, true);
    }
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterEditor::enableEditing()
//-----------------------------------------------------------------------------
void ModuleParameterEditor::enableEditing()
{
    model_->enableEditing();
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterEditor::disableEditing()
//-----------------------------------------------------------------------------
void ModuleParameterEditor::disableEditing()
{
    model_->disableEditing();
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterEditor::setModuleParameters()
//-----------------------------------------------------------------------------
void ModuleParameterEditor::setModuleParameters(QSharedPointer<QList<QSharedPointer<ModuleParameter> > > 
    moduleParameters)
{
    parameterInterface_->setModuleParameters(moduleParameters);

    model_->resetModelItems();
}
