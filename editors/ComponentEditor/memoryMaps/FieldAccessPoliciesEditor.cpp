//-----------------------------------------------------------------------------
// File: FieldAccessPoliciesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Editor for field access policies.
//-----------------------------------------------------------------------------

#include "FieldAccessPoliciesEditor.h"
#include "FieldAccessPolicyColumns.h"
#include "FieldAccessPoliciesDelegate.h"
#include "FieldAccessPoliciesModel.h"

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>

#include <QVBoxLayout>
#include <QSortFilterProxyModel>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesEditor::FieldAccessPoliciesEditor()
//-----------------------------------------------------------------------------
FieldAccessPoliciesEditor::FieldAccessPoliciesEditor(QString const& fieldName, FieldInterface* fieldInterface, 
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
    QWidget* parent):
QGroupBox(QStringLiteral("Access policies"), parent),
view_(new EditableTableView(this))
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(view_);

    auto model = new FieldAccessPoliciesModel(fieldName, parameterFinder, fieldInterface, this);
    auto proxy = new QSortFilterProxyModel(this);

    ComponentParameterModel* componentParameterModel = new ComponentParameterModel(parameterFinder, this);
    componentParameterModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterNameCompleter = new ParameterCompleter(this);
    parameterNameCompleter->setModel(componentParameterModel);

    auto delegate = new FieldAccessPoliciesDelegate(parameterNameCompleter, parameterFinder, this);

    view_->setSortingEnabled(true);
    proxy->setSourceModel(model);
    view_->setModel(proxy);
    view_->setItemDelegate(delegate);
    view_->horizontalHeader()->setStretchLastSection(false);

    connect(view_, SIGNAL(addItem(QModelIndex const&)), model, SLOT(onAddRow(QModelIndex const&)), Qt::UniqueConnection);
    connect(view_, SIGNAL(removeItem(QModelIndex const&)), model, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(model, SIGNAL(invalidateFilter()), proxy, SLOT(invalidate()), Qt::UniqueConnection);
    connect(model, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}
