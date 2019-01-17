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

#include "VendorExtensionsModel.h"

#include <common/views/EditableTableView/editabletableview.h>
#include <common/widgets/tabDocument/TabDocument.h>

#include <editors/common/VendorExtensionEditor/VendorExtensionsDelegate.h>

#include <IPXACTmodels/common/Extendable.h>

#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: VendorExtensionsEditor::VendorExtensionsEditor()
//-----------------------------------------------------------------------------
VendorExtensionsEditor::VendorExtensionsEditor(QWidget *parent) :
    QWidget(parent),
    extensionsView_(new EditableTableView(this)),
    extensionsModel_(new VendorExtensionsModel(this)),
    summaryLable_(new QLabel())
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLable_, 0, Qt::AlignCenter);
    layout->addWidget(extensionsView_);

    extensionsView_->setItemDelegate(new VendorExtensionsDelegate(this));
    extensionsView_->setModel(extensionsModel_);

    connect(extensionsView_, SIGNAL(addItem(QModelIndex const&)), 
        extensionsModel_, SLOT(onAddItem(QModelIndex const&)), Qt::UniqueConnection);
    connect(extensionsView_, SIGNAL(removeItem(QModelIndex const&)),
        extensionsModel_, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(extensionsModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

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

    parentWidget()->setMaximumHeight(20);    
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsEditor::changeVendorExtensions()
//-----------------------------------------------------------------------------
void VendorExtensionsEditor::changeVendorExtensions(QString const& containingID,
    QSharedPointer<Extendable> extensionItem)
{
    parentWidget()->raise();

    summaryLable_->setText(containingID);
    summaryLable_->setAlignment(Qt::AlignCenter);

    extensionsModel_->setVendorExtensions(extensionItem->getVendorExtensions());

    parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}
