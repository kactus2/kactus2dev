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

#include <IPXACTmodels/Component/AccessPolicy.h>

#include <common/views/EditableTableView/editabletableview.h>

#include <QVBoxLayout>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: AccessPoliciesEditor::AccessPoliciesEditor()
//-----------------------------------------------------------------------------
AccessPoliciesEditor::AccessPoliciesEditor(QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies, 
    AccessPolicyInterface* accessPolicyInterface, QWidget* parent) :
    QGroupBox(tr("Access policies"), parent),
    accessPolicies_(accessPolicies),
    view_(new EditableTableView(this)),
    interface_(accessPolicyInterface)
{
    interface_->setAccessPolicies(accessPolicies);

    auto topLayout = new QVBoxLayout(this);
    topLayout->addWidget(view_);

    auto model = new AccessPoliciesModel(accessPolicyInterface, this);
    auto delegate = new AccessPoliciesDelegate(this);
    auto proxy = new QSortFilterProxyModel(this);

    proxy->setSourceModel(model);
    view_->setModel(proxy);
    view_->setItemDelegate(delegate);
    view_->setSortingEnabled(true);

    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model, SIGNAL(invalidateFilter()), proxy, SLOT(invalidate()), Qt::UniqueConnection);

    connect(view_, SIGNAL(addItem(QModelIndex const&)),
        model, SLOT(onAddRow(QModelIndex const&)), Qt::UniqueConnection);
    connect(view_, SIGNAL(removeItem(QModelIndex const&)),
        model, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesEditor::refresh()
//-----------------------------------------------------------------------------
void AccessPoliciesEditor::refresh()
{
    view_->update();

    interface_->setAccessPolicies(accessPolicies_);
}
