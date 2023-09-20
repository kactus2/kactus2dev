//-----------------------------------------------------------------------------
// File: AccessPoliciesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 28.8.2023
//
// Description:
// Editor for access policies found in address blocks, registers, register files etc.
//-----------------------------------------------------------------------------

#include "AccessPoliciesEditor.h"
#include "AccessPoliciesDelegate.h"
#include "AccessPoliciesModel.h"

#include <KactusAPI/include/AccessPolicyInterface.h>

#include <common/views/EditableTableView/editabletableview.h>

#include <QVBoxLayout>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: AccessPoliciesEditor::AccessPoliciesEditor()
//-----------------------------------------------------------------------------
AccessPoliciesEditor::AccessPoliciesEditor(AccessPolicyInterface* accessPolicyInterface, QString const& registerName, QWidget* parent) :
    QGroupBox(tr("Access policies"), parent)
{
    auto topLayout = new QVBoxLayout(this);
    auto view = new EditableTableView(this);
    topLayout->addWidget(view);

    auto model = new AccessPoliciesModel(accessPolicyInterface, this);
    auto delegate = new AccessPoliciesDelegate(this);
    auto proxy = new QSortFilterProxyModel(this);

    proxy->setSourceModel(model);
    view->setModel(proxy);
    view->setItemDelegate(delegate);
    view->setSortingEnabled(true);

    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model, SIGNAL(invalidateFilter()), proxy, SLOT(invalidate()), Qt::UniqueConnection);

    connect(view, SIGNAL(addItem(QModelIndex const&)),
        model, SLOT(onAddRow(QModelIndex const&)), Qt::UniqueConnection);
    connect(view, SIGNAL(removeItem(QModelIndex const&)),
        model, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);
}
