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
// Function: KactusAttributeEditor::KactusAttributeEditor()
//-----------------------------------------------------------------------------
KactusAttributeEditor::KactusAttributeEditor(QWidget* parent) : QGroupBox(tr("Kactus Attributes"), parent),
    prodHierLabel_(new QLabel(tr("Product Hierarchy:"), this)), 
    prodHierCombo_(new QComboBox(this)),
    firmnessLabel_(new QLabel(tr("Firmness:"), this)),
    firmnessCombo_(new QComboBox(this)),
    implementationLabel_(new QLabel(tr("Implementation:"), this)), 
    implementationValue_(new QLabel(tr(""), this))
{
    // The implementation label and value label are set to invisible by default.
    implementationLabel_->setVisible(false);
    implementationValue_->setVisible(false);

    for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
    {
        KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(i);
        prodHierCombo_->addItem(KactusAttribute::hierarchyToString(val));
    }

    for (unsigned int i = 0; i < KactusAttribute::KTS_REUSE_LEVEL_COUNT; ++i)
    {
        KactusAttribute::Firmness val = static_cast<KactusAttribute::Firmness>(i);
        firmnessCombo_->addItem(KactusAttribute::firmnessToString(val));
    }

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
// Function: KactusAttributeEditor::~KactusAttributeEditor()
//-----------------------------------------------------------------------------
KactusAttributeEditor::~KactusAttributeEditor()
{
}

//-----------------------------------------------------------------------------
// Function: KactusAttributeEditor::setAttributes()
//-----------------------------------------------------------------------------
void KactusAttributeEditor::setAttributes(KactusAttribute::ProductHierarchy prodHier,
                                          KactusAttribute::Firmness firmness)
{
    prodHierCombo_->setCurrentIndex(prodHier);
    firmnessCombo_->setCurrentIndex(firmness);
}

//-----------------------------------------------------------------------------
// Function: KactusAttributeEditor::getProductHierarchy()
//-----------------------------------------------------------------------------
KactusAttribute::ProductHierarchy KactusAttributeEditor::getProductHierarchy() const
{
    return static_cast<KactusAttribute::ProductHierarchy>(prodHierCombo_->currentIndex());
}

//-----------------------------------------------------------------------------
// Function: KactusAttributeEditor::getFirmness()
//-----------------------------------------------------------------------------
KactusAttribute::Firmness KactusAttributeEditor::getFirmness() const
{
    return static_cast<KactusAttribute::Firmness>(firmnessCombo_->currentIndex());
}

//-----------------------------------------------------------------------------
// Function: KactusAttributeEditor::setImplementation()
//-----------------------------------------------------------------------------
void KactusAttributeEditor::setImplementation(KactusAttribute::Implementation implementation)
{
    implementationValue_->setText(KactusAttribute::implementationToString(implementation));
    implementationLabel_->setVisible(true);
    implementationValue_->setVisible(true);
}

//-----------------------------------------------------------------------------
// Function: KactusAttributeEditor::hideAttributes()
//-----------------------------------------------------------------------------
void KactusAttributeEditor::hideAttributes()
{
    prodHierLabel_->setVisible(false);
    prodHierCombo_->setVisible(false);
    firmnessLabel_->setVisible(false);
    firmnessCombo_->setVisible(false);
}
