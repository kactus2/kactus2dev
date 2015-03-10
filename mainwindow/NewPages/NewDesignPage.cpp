//-----------------------------------------------------------------------------
// File: NewDesignPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.6.2011
//
// Description:
// New design property page.
//-----------------------------------------------------------------------------

#include "NewDesignPage.h"

#include <library/LibraryManager/libraryinterface.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: NewDesignPage()
//-----------------------------------------------------------------------------
NewDesignPage::NewDesignPage(LibraryInterface* libInterface, QWidget* parentDlg):
NewPage(libInterface, VLNV::COMPONENT, tr("New HW Design"), tr("Creates a hierarchical HW design"), parentDlg),
attributeEditor_(0)
{
    // Create the attribute editor.
    attributeEditor_ = new KactusAttributeEditor(this);
    connect(attributeEditor_, SIGNAL(productHierarchyChanged()), this, SLOT(onProductHierarchyChanged()));

    // Create the VLNV editor.
    vlnvEditor_->setImplementationFilter(true, KactusAttribute::HW);

   
    // Setup the layout.
    QVBoxLayout* widgetLayout = dynamic_cast<QVBoxLayout*>(layout());
    Q_ASSERT(widgetLayout);
    widgetLayout->insertWidget(3, attributeEditor_);

    onProductHierarchyChanged();
}

//-----------------------------------------------------------------------------
// Function: ~NewDesignPage()
//-----------------------------------------------------------------------------
NewDesignPage::~NewDesignPage()
{
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
    emit createDesign(attributeEditor_->getProductHierarchy(),
                      attributeEditor_->getFirmness(),
                      vlnvEditor_->getVLNV(), selectedPath());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NewDesignPage::onPageChange()
{
    // Discard the VLNV and reset the attributes.
    attributeEditor_->setAttributes(KactusAttribute::FLAT, KactusAttribute::TEMPLATE);
    onProductHierarchyChanged();
    return NewPage::onPageChange();
}

//-----------------------------------------------------------------------------
// Function: onProductHierarchyChanged()
//-----------------------------------------------------------------------------
void NewDesignPage::onProductHierarchyChanged()
{
    // Update the VLNV's library field if it is either empty or any of the predefined ones.
    VLNV vlnv = vlnvEditor_->getVLNV();

    if (vlnv.getLibrary().isEmpty())
    {
        vlnv.setLibrary(KactusAttribute::valueToString(attributeEditor_->getProductHierarchy()).toLower());
    }
    else
    {
        for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
        {
            if (vlnv.getLibrary().toLower() ==
                KactusAttribute::valueToString(static_cast<KactusAttribute::ProductHierarchy>(i)).toLower())
            {
                vlnv.setLibrary(KactusAttribute::valueToString(attributeEditor_->getProductHierarchy()).toLower());
                break;
            }
        }
    }

    vlnvEditor_->setVLNV(vlnv);
}
