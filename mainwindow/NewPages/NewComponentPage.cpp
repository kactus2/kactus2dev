//-----------------------------------------------------------------------------
// File: NewComponentPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 9.6.2011
//
// Description:
// New component property page.
//-----------------------------------------------------------------------------

#include "NewComponentPage.h"

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>

#include <IPXACTmodels/common/VLNV.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: NewComponentPage()
//-----------------------------------------------------------------------------
NewComponentPage::NewComponentPage(LibraryInterface* libInterface, QWidget* parentDlg):
NewPage(libInterface, VLNV::COMPONENT, tr("New HW Component"),
    tr("Creates a flat (non-hierarchical) HW component"), parentDlg),
attributeEditor_(new KactusAttributeEditor(this))
{
    connect(attributeEditor_, SIGNAL(productHierarchyChanged()), this, SLOT(onProductHierarchyChanged()));

    vlnvEditor_->setImplementationFilter(true, KactusAttribute::HW);

    // Setup the layout.
    QVBoxLayout* widgetLayout = dynamic_cast<QVBoxLayout*>(layout());
    Q_ASSERT(widgetLayout);
    widgetLayout->addSpacing(12);
    widgetLayout->insertWidget(3, attributeEditor_);
    widgetLayout->addStretch(1);

    onProductHierarchyChanged();
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
    emit createComponent(attributeEditor_->getProductHierarchy(), attributeEditor_->getFirmness(),
        attributeEditor_->getTags(), vlnvEditor_->getVLNV(), selectedRevision() ,selectedPath());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NewComponentPage::onPageChange()
{
    // Discard the VLNV and reset the attributes.
    attributeEditor_->setAttributes(KactusAttribute::FLAT, KactusAttribute::TEMPLATE, QVector<TagData>());
    onProductHierarchyChanged();
    return NewPage::onPageChange();
}

//-----------------------------------------------------------------------------
// Function: NewComponentPage::getProductHierarchy()
//-----------------------------------------------------------------------------
KactusAttribute::ProductHierarchy NewComponentPage::getProductHierarchy() const
{
    return attributeEditor_->getProductHierarchy();
}
