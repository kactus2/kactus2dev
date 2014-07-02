//-----------------------------------------------------------------------------
// File: NewComponentPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.6.2011
//
// Description:
// New component property page.
//-----------------------------------------------------------------------------

#include "NewComponentPage.h"

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>

#include <library/LibraryManager/vlnv.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: NewComponentPage()
//-----------------------------------------------------------------------------
NewComponentPage::NewComponentPage(LibraryInterface* libInterface, QWidget* parentDlg):
    NewPage(libInterface, VLNV::COMPONENT, tr("New HW Component"), tr("Creates a flat (non-hierarchical) HW component"), parentDlg) 
{

    // Create the attribute editor.
    attributeEditor_ = new KactusAttributeEditor(this);
    connect(attributeEditor_, SIGNAL(productHierarchyChanged()), this, SLOT(onProductHierarchyChanged()));

    vlnvEditor_->setImplementationFilter(true, KactusAttribute::KTS_HW);

    // Setup the layout.
    QVBoxLayout* widgetLayout = dynamic_cast<QVBoxLayout*>(layout());
    Q_ASSERT(widgetLayout);
    widgetLayout->addSpacing(12);
    widgetLayout->insertWidget(3, attributeEditor_);

    onProductHierarchyChanged();
}

//-----------------------------------------------------------------------------
// Function: ~NewComponentPage()
//-----------------------------------------------------------------------------
NewComponentPage::~NewComponentPage()
{

}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NewComponentPage::validate()
{
    bool valid = NewPage::validate();

    // Check if the VLNV already exists.
    if (!valid)
    {        
        showErrorForReservedVLVN(vlnvEditor_->getVLNV());     
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NewComponentPage::apply()
{
    emit createComponent(attributeEditor_->getProductHierarchy(),
                         attributeEditor_->getFirmness(),
                         vlnvEditor_->getVLNV(), selectedPath());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NewComponentPage::onPageChange()
{
    // Discard the VLNV and reset the attributes.
    attributeEditor_->setAttributes(KactusAttribute::KTS_GLOBAL, KactusAttribute::KTS_TEMPLATE);
    onProductHierarchyChanged();
    return NewPage::onPageChange();
}

//-----------------------------------------------------------------------------
// Function: onProductHierarchyChanged()
//-----------------------------------------------------------------------------
void NewComponentPage::onProductHierarchyChanged()
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
