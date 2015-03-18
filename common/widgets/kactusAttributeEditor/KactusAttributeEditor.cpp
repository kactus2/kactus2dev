//-----------------------------------------------------------------------------
// File: KactusAttributeEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.6.2011
//
// Description:
// Attribute editor for Kactus 2 -specific base attributes.
//-----------------------------------------------------------------------------

#include "KactusAttributeEditor.h"

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QGridLayout>
#include <QString>

//-----------------------------------------------------------------------------
// Function: KactusAttributeEditor()
//-----------------------------------------------------------------------------
KactusAttributeEditor::KactusAttributeEditor(QWidget* parent) : QGroupBox(tr("Kactus Attributes"), parent),
                                                                prodHierLabel_(0), prodHierCombo_(0),
                                                                firmnessLabel_(0), firmnessCombo_(0),
                                                                implementationLabel_(0), implementationValue_(0)
{
    // Create the product hierarchy label and combo box.
    prodHierLabel_ = new QLabel(tr("Product Hierarchy:"), this);
    prodHierCombo_ = new QComboBox(this);
    
    for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
    {
        KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(i);
        prodHierCombo_->addItem(KactusAttribute::valueToString(val));
    }

    // Create the firmness label and combo box.
    firmnessLabel_ = new QLabel(tr("Firmness:"), this);
    firmnessCombo_ = new QComboBox(this);

    for (unsigned int i = 0; i < KactusAttribute::KTS_REUSE_LEVEL_COUNT; ++i)
    {
        KactusAttribute::Firmness val = static_cast<KactusAttribute::Firmness>(i);
        firmnessCombo_->addItem(KactusAttribute::valueToString(val));
    }
    
    // Create the implementation label and value label (set to invisible by default).
    implementationLabel_ = new QLabel(tr("Implementation:"), this);
    implementationLabel_->setVisible(false);

    implementationValue_ = new QLabel(tr(""), this);
    implementationValue_->setVisible(false);

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(prodHierLabel_, 0, 0, 1, 1);
    layout->addWidget(prodHierCombo_, 0, 1, 1, 1);
    layout->addWidget(firmnessLabel_, 1, 0, 1, 1);
    layout->addWidget(firmnessCombo_, 1, 1, 1, 1);
    layout->addWidget(implementationLabel_, 2, 0, 1, 1);
    layout->addWidget(implementationValue_, 2, 1, 1, 1);
    layout->setColumnStretch(3, 1);

    connect(prodHierCombo_, SIGNAL(currentIndexChanged(int)), this, SIGNAL(contentChanged()));
    connect(prodHierCombo_, SIGNAL(currentIndexChanged(int)), this, SIGNAL(productHierarchyChanged()));
    connect(firmnessCombo_, SIGNAL(currentIndexChanged(int)), this, SIGNAL(contentChanged()));
}

//-----------------------------------------------------------------------------
// Function: ~KactusAttributeEditor()
//-----------------------------------------------------------------------------
KactusAttributeEditor::~KactusAttributeEditor()
{
}

//-----------------------------------------------------------------------------
// Function: setAttributes()
//-----------------------------------------------------------------------------
void KactusAttributeEditor::setAttributes(KactusAttribute::ProductHierarchy prodHier,
                                          KactusAttribute::Firmness firmness)
{
    prodHierCombo_->setCurrentIndex(prodHier);
    firmnessCombo_->setCurrentIndex(firmness);
}

//-----------------------------------------------------------------------------
// Function: getProductHierarchy()
//-----------------------------------------------------------------------------
KactusAttribute::ProductHierarchy KactusAttributeEditor::getProductHierarchy() const
{
    return static_cast<KactusAttribute::ProductHierarchy>(prodHierCombo_->currentIndex());
}

//-----------------------------------------------------------------------------
// Function: getFirmness()
//-----------------------------------------------------------------------------
KactusAttribute::Firmness KactusAttributeEditor::getFirmness() const
{
    return static_cast<KactusAttribute::Firmness>(firmnessCombo_->currentIndex());
}

//-----------------------------------------------------------------------------
// Function: setImplementation()
//-----------------------------------------------------------------------------
void KactusAttributeEditor::setImplementation(KactusAttribute::Implementation implementation)
{
    implementationValue_->setText(KactusAttribute::valueToString(implementation));
    implementationLabel_->setVisible(true);
    implementationValue_->setVisible(true);
}

//-----------------------------------------------------------------------------
// Function: hideAttributes()
//-----------------------------------------------------------------------------
void KactusAttributeEditor::hideAttributes()
{
    prodHierLabel_->setVisible(false);
    prodHierCombo_->setVisible(false);
    firmnessLabel_->setVisible(false);
    firmnessCombo_->setVisible(false);
}
