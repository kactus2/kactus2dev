//-----------------------------------------------------------------------------
// File: FieldAccessPoliciesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 8.8.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#include "FieldAccessPoliciesEditor.h"
#include "FieldAccessPolicyColumns.h"
#include "FieldAccessPoliciesDelegate.h"
#include "FieldAccessPoliciesModel.h"

#include <QVBoxLayout>
#include <QSortFilterProxyModel>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesEditor::FieldAccessPoliciesEditor()
//-----------------------------------------------------------------------------
FieldAccessPoliciesEditor::FieldAccessPoliciesEditor(QString const& fieldName, FieldInterface* fieldInterface, QWidget* parent):
    QGroupBox(QStringLiteral("Access policies"), parent),
    view_(new EditableTableView(this))
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(view_);

    auto model = new FieldAccessPoliciesModel(fieldName, fieldInterface, this);
    auto proxy = new QSortFilterProxyModel(this);
    auto delegate = new FieldAccessPoliciesDelegate(this);

    view_->setSortingEnabled(true);
    proxy->setSourceModel(model);
    view_->setModel(proxy);
    view_->setItemDelegate(delegate);
    view_->horizontalHeader()->setStretchLastSection(false);
}
