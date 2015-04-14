//-----------------------------------------------------------------------------
// File: RemapConditionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.04.2015
//
// Description:
// Editor used to edit the details of remap conditions in a remap state.
//-----------------------------------------------------------------------------

#include "RemapConditionEditor.h"
#include "RemapConditionColumns.h"
#include "RemapConditionDelegate.h"

#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: RemapConditionEditor::RemapConditionEditor()
//-----------------------------------------------------------------------------
RemapConditionEditor::RemapConditionEditor(QSharedPointer<QList<QSharedPointer<RemapPort> > > remapPorts,
    QSharedPointer<Component> component, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget* parent):
QGroupBox(tr("Remap State Condition"), parent),
view_(new EditableTableView(this)),
model_(0),
delegate_(0),
proxy_(new QSortFilterProxyModel(this)),
component_(component)
{
    proxy_->setDynamicSortFilter(true);

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));
    model_ = new RemapConditionModel(remapPorts, component->getPorts(), expressionParser, parameterFinder,
        expressionFormatter, parent);

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

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    view_->setSortingEnabled(true);
    view_->setItemsDraggable(false);

    ComponentParameterModel* parameterModel = new ComponentParameterModel(parameterFinder, this);
    parameterModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(parameterModel);

    delegate_ = new RemapConditionDelegate(component->getPortNames(), parameterCompleter, parameterFinder,
        expressionFormatter, this);

    view_->setItemDelegate(delegate_);

    connect(delegate_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(delegate_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(newAvailablePorts(QStringList)),
        delegate_, SLOT(setNewPortNames(QStringList)), Qt::UniqueConnection);

    proxy_->setSourceModel(model_);
    view_->setModel(proxy_);

    view_->sortByColumn(RemapConditionColumns::NAME_COLUMN, Qt::AscendingOrder);
    view_->setAlternatingRowColors(false);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(view_);
}

//-----------------------------------------------------------------------------
// Function: RemapConditionEditor::~RemapConditionEditor()
//-----------------------------------------------------------------------------
RemapConditionEditor::~RemapConditionEditor()
{

}

//-----------------------------------------------------------------------------
// Function: RemapConditionEditor::isValid()
//-----------------------------------------------------------------------------
bool RemapConditionEditor::isValid() const
{
    return model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: RemapConditionEditor::refresh()
//-----------------------------------------------------------------------------
void RemapConditionEditor::refresh()
{
    delegate_->setComponentPorts(component_->getPortNames());
    model_->updatePorts(component_->getPorts());
    proxy_->invalidate();
    view_->update();
}
