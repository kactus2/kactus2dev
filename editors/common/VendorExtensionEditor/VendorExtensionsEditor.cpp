//-----------------------------------------------------------------------------
// File: VendorExtensionsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 10.10.2011
//
// Description:
// Editor to display/edit vendor extensions.
//-----------------------------------------------------------------------------

#include "VendorExtensionsEditor.h"

#include <common/views/EditableTreeView/EditableTreeView.h>
#include <common/widgets/tabDocument/TabDocument.h>

#include <editors/common/VendorExtensionEditor/VendorExtensionsModel.h>
#include <editors/common/VendorExtensionEditor/VendorExtensionsFilter.h>
#include <editors/common/VendorExtensionEditor/VendorExtensionsDelegate.h>
#include <editors/common/VendorExtensionEditor/VendorExtensionColumns.h>

#include <IPXACTmodels/common/Extendable.h>

#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: VendorExtensionsEditor::VendorExtensionsEditor()
//-----------------------------------------------------------------------------
VendorExtensionsEditor::VendorExtensionsEditor(QWidget *parent) :
QWidget(parent),
extensionsView_(new EditableTreeView(true, QString(tr("Add vendor extension")), QString(tr("Add child element")),
    QString(tr("Remove row")), QString(tr("Remove all child elements")), this)),
extensionsModel_(new VendorExtensionsModel(this)),
summaryLabel_(new QLabel(this))
{
    extensionsView_->setSortingEnabled(false);

    extensionsView_->setSelectionMode(QAbstractItemView::ContiguousSelection);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel_, 0, Qt::AlignCenter);
    layout->addWidget(extensionsView_);

    VendorExtensionsDelegate* extensionDelegate = new VendorExtensionsDelegate(this);
    extensionsView_->setItemDelegate(extensionDelegate);

    VendorExtensionsFilter* newFilter(new VendorExtensionsFilter(this));
    newFilter->setSourceModel(extensionsModel_);
    extensionsView_->setModel(newFilter);

    connect(extensionsView_, SIGNAL(addItems(QModelIndexList const&)),
        newFilter, SLOT(onAddItem(QModelIndexList const&)), Qt::UniqueConnection);
    connect(extensionsView_, SIGNAL(addSubItem(QModelIndexList const&)),
        newFilter, SLOT(onAddSubItem(QModelIndexList const&)), Qt::UniqueConnection);
    connect(extensionsView_, SIGNAL(removeSelectedItems(QModelIndexList const&)),
        newFilter, SLOT(onRemoveSelectedRows(QModelIndexList const&)), Qt::UniqueConnection);
    connect(extensionsView_, SIGNAL(removeAllSubItems(QModelIndexList const&)),
        newFilter, SLOT(onRemoveAllSubItemsFromIndexes(QModelIndexList const&)), Qt::UniqueConnection);

    connect(newFilter, SIGNAL(addItem(QModelIndex const&)),
        extensionsModel_, SLOT(onAddItem(QModelIndex const&)), Qt::UniqueConnection);
    connect(newFilter, SIGNAL(addSubItem(QModelIndex const&)),
        extensionsModel_, SLOT(onAddSubItem(QModelIndex const&)), Qt::UniqueConnection);
    connect(newFilter, SIGNAL(removeItem(QModelIndex const&)),
        extensionsModel_, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);
    connect(newFilter, SIGNAL(removeAllSubItemsFromIndex(QModelIndex const&)),
        extensionsModel_, SLOT(onRemoveAllSubItems(QModelIndex const&)), Qt::UniqueConnection);

    connect(extensionsModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(extensionDelegate, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    extensionsView_->header()->setStretchLastSection(true);

	setDisabled(true);
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsEditor::setLocked()
//-----------------------------------------------------------------------------
void VendorExtensionsEditor::setLocked(bool locked)
{
    setDisabled(locked);
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsEditor::setContext()
//-----------------------------------------------------------------------------
void VendorExtensionsEditor::setContext(TabDocument* doc)
{
    setLocked(doc->isProtected());

    disconnect(this, SIGNAL(contentChanged()), 0, 0);
    connect(this, SIGNAL(contentChanged()), doc, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsEditor::clear()
//-----------------------------------------------------------------------------
void VendorExtensionsEditor::clear()
{
    disconnect(this, SIGNAL(contentChanged()), 0, 0);

    extensionsModel_->clear();

   // parentWidget()->setMaximumHeight(20);    
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsEditor::changeVendorExtensions()
//-----------------------------------------------------------------------------
void VendorExtensionsEditor::changeVendorExtensions(QString const& containingID,
    QSharedPointer<Extendable> extensionItem)
{
    //parentWidget()->raise();

    summaryLabel_->setText(containingID);
    summaryLabel_->setAlignment(Qt::AlignCenter);

    extensionsModel_->setVendorExtensions(extensionItem->getVendorExtensions());

    //parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);

    for (int i = 0; i < VendorExtensionColumns::COLUMN_COUNT; ++i)
    {
        extensionsView_->resizeColumnToContents(i);
    }
}
