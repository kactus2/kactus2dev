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

#include <common/widgets/tagEditor/TagContainer.h>

#include <QFormLayout>
#include <QString>

//-----------------------------------------------------------------------------
// Function: KactusAttributeEditor::KactusAttributeEditor()
//-----------------------------------------------------------------------------
KactusAttributeEditor::KactusAttributeEditor(QWidget* parent) : QGroupBox(tr("Kactus attributes"), parent),
    prodHierLabel_(new QLabel(tr("Product Hierarchy:"), this)), 
    prodHierCombo_(new QComboBox(this)),
    firmnessLabel_(new QLabel(tr("Firmness:"), this)),
    firmnessCombo_(new QComboBox(this)),
    implementationLabel_(new QLabel(tr("Implementation:"), this)), 
    implementationValue_(new QLabel(tr(""), this)),
    tagEditor_(new TagContainer(this))
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

    QFormLayout* layout = new QFormLayout(this);
    layout->addRow(prodHierLabel_, prodHierCombo_);
    layout->addRow(firmnessLabel_, firmnessCombo_);
    layout->addRow(implementationLabel_, implementationValue_);
    layout->addRow(new QLabel(QStringLiteral("Tags:")), tagEditor_);

    connect(prodHierCombo_, SIGNAL(currentIndexChanged(int)), this, SIGNAL(contentChanged()));
    connect(prodHierCombo_, SIGNAL(currentIndexChanged(int)), this, SIGNAL(productHierarchyChanged()));
    connect(firmnessCombo_, SIGNAL(currentIndexChanged(int)), this, SIGNAL(contentChanged()));
    connect(tagEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
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
    KactusAttribute::Firmness firmness, QVector<QPair<QString, QString> > tags)
{
    prodHierCombo_->setCurrentIndex(prodHier);
    firmnessCombo_->setCurrentIndex(firmness);
    tagEditor_->setupTags(tags);
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
// Function: KactusAttributeEditor::getTags()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QString> > KactusAttributeEditor::getTags() const
{
    return tagEditor_->getTags();
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
