//-----------------------------------------------------------------------------
// File: KactusAttributeEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 23.6.2011
//
// Description:
// Attribute editor for Kactus 2 -specific base attributes.
//-----------------------------------------------------------------------------

#include "KactusAttributeEditor.h"

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <common/widgets/tagEditor/TagEditorContainer.h>

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
    tagEditor_(new TagEditorContainer(this))
{
    setFlat(true);

    // The implementation label and value label are set to invisible by default.
    implementationLabel_->setVisible(false);
    implementationValue_->setVisible(false);

    prodHierCombo_->addItem(QIcon(":icons/common/graphics/square.png"), "Flat");
    prodHierCombo_->addItem(QIcon(":icons/common/graphics/box.png"), "Product");
    prodHierCombo_->addItem(QIcon(":icons/common/graphics/circuit.png"), "Board");
    prodHierCombo_->addItem(QIcon(":icons/common/graphics/chip.png"), "Chip");
    prodHierCombo_->addItem(QIcon(":icons/common/graphics/soc.png"), "SoC");
    prodHierCombo_->addItem(QIcon(":icons/common/graphics/capacitor.png"), "IP");

    firmnessCombo_->addItem(QIcon(":icons/common/graphics/mutable.png"), "Mutable");
    firmnessCombo_->addItem(QIcon(":icons/common/graphics/template.png"), "Template");
    firmnessCombo_->addItem(QIcon(":icons/common/graphics/fixed.png"), "Fixed");

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
    KactusAttribute::Firmness firmness, QVector<TagData> tags)
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
QVector<TagData> KactusAttributeEditor::getTags() const
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
