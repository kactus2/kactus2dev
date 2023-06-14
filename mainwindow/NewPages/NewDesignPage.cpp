//-----------------------------------------------------------------------------
// File: NewDesignPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 9.6.2011
//
// Description:
// New design property page.
//-----------------------------------------------------------------------------

#include "NewDesignPage.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: NewDesignPage()
//-----------------------------------------------------------------------------
NewDesignPage::NewDesignPage(LibraryInterface* libInterface, QWidget* parentDlg):
NewPage(libInterface, VLNV::COMPONENT, tr("New HW Design"), tr("Creates a hierarchical HW design"), parentDlg),
    attributeEditor_(new KactusAttributeEditor(this))
{
    connect(attributeEditor_, SIGNAL(productHierarchyChanged()), this, SLOT(onProductHierarchyChanged()));

    // Create the VLNV editor.
    vlnvEditor_->setImplementationFilter(true, KactusAttribute::HW);

    // Setup the layout.
    QVBoxLayout* widgetLayout = dynamic_cast<QVBoxLayout*>(layout());
    Q_ASSERT(widgetLayout);
    widgetLayout->insertWidget(3, attributeEditor_);   
    widgetLayout->addStretch(1);

    onProductHierarchyChanged();
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewDesignPage::validate()
{
    VLNV vlnv = vlnvEditor_->getVLNV();
    bool validVLNV = NewPage::validate();

    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());
    VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".designcfg", vlnv.getVersion());

    // Check if any of the VLNVs already exists.
    if (!validVLNV)
    {
        showErrorForReservedVLVN(vlnv);
        return false;
    }

    if (!isUnusedVLNV(designVLNV))
    {
        showErrorForReservedVLVN(designVLNV);
        return false;
    }

    if (!isUnusedVLNV(desConfVLNV))
    {
        showErrorForReservedVLVN(desConfVLNV);
        return false;
    }    

    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NewDesignPage::apply()
{
    emit createDesign(attributeEditor_->getProductHierarchy(), attributeEditor_->getFirmness(),
        attributeEditor_->getTags(), vlnvEditor_->getVLNV(), selectedRevision(), selectedPath());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NewDesignPage::onPageChange()
{
    // Discard the VLNV and reset the attributes.
    attributeEditor_->setAttributes(KactusAttribute::FLAT, KactusAttribute::TEMPLATE, QVector<TagData>());
    onProductHierarchyChanged();
    return NewPage::onPageChange();
}

//-----------------------------------------------------------------------------
// Function: NewDesignPage::getProductHierarchy()
//-----------------------------------------------------------------------------
KactusAttribute::ProductHierarchy NewDesignPage::getProductHierarchy() const
{
    return attributeEditor_->getProductHierarchy();
}
