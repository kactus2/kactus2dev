//-----------------------------------------------------------------------------
// File: QualifierEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 16.06.2023
//
// Description:
// Editor for qualifiers in 2022 standard Abstraction Definition.
//-----------------------------------------------------------------------------

#include "QualifierEditor.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QScrollBar>

//-----------------------------------------------------------------------------
// Function: QualifierEditor::QualifierEditor()
//-----------------------------------------------------------------------------
QualifierEditor::QualifierEditor(QWidget* parent):
    QFrame(parent),
    enumerationEditor_(new EnumCollectionEditor(parent))
{
    setFrameStyle(QFrame::StyledPanel);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_NoMousePropagation);
    setAutoFillBackground(true);

    setupLayout();

    connect(enumerationEditor_, SIGNAL(itemStateChanged(Qt::CheckState)), this, SLOT(onItemCheckChanged(Qt::CheckState)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setResetLevel()
//-----------------------------------------------------------------------------
void QualifierEditor::setResetEnableLevel(QString const& level)
{
    resetLevelLineEdit_.setText(level);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setClockEnableLevel()
//-----------------------------------------------------------------------------
void QualifierEditor::setClockEnableLevel(QString const& level)
{
    clockEnableLevelLineEdit_.setText(level);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setPowerEnableLevel()
//-----------------------------------------------------------------------------
void QualifierEditor::setPowerEnableLevel(QString const& level)
{
    powerEnableLevelLineEdit_.setText(level);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setPowerDomainRef()
//-----------------------------------------------------------------------------
void QualifierEditor::setPowerDomainRef(QString const& ref)
{
    powerDomainLineEdit_.setText(ref);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setFlowType()
//-----------------------------------------------------------------------------
void QualifierEditor::setFlowType(QString const& flowType)
{
    flowTypeLineEdit_.setText(flowType);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setUserFlowType()
//-----------------------------------------------------------------------------
void QualifierEditor::setUserFlowType(QString const& userFlowType)
{
    userFlowTypeLineEdit_.setText(userFlowType);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setUserDefined()
//-----------------------------------------------------------------------------
void QualifierEditor::setUserDefined(QString const& userDefined)
{
    userDefinedLineEdit_.setText(userDefined);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setupEditor()
//-----------------------------------------------------------------------------
void QualifierEditor::setupEditor(QStringList const& allQualifiers, QStringList const& activeQualifiers, QMap<Qualifier::Attribute, QString> attributes)
{
    for (auto const& qualifier : allQualifiers)
    {
        bool selected = false;
        if (activeQualifiers.contains(qualifier))
        {
            selected = true;
        }

        enumerationEditor_->addItem(qualifier, false, selected);
    }

    for (auto attributeType : attributes.keys())
    {
        setQualifierAttribute(QPair<Qualifier::Attribute, QString>(attributeType, attributes[attributeType]));
    }

    updateAttributeList(enumerationEditor_->getSelectedItems());
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::onItemCheckChanged()
//-----------------------------------------------------------------------------
void QualifierEditor::onItemCheckChanged(Qt::CheckState newState)
{
    QStringList const& checkedItems = enumerationEditor_->getSelectedItems();
    updateAttributeList(checkedItems);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setupLayout()
//-----------------------------------------------------------------------------
void QualifierEditor::setupLayout()
{
    enumerationEditor_->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* editorLayout = new QVBoxLayout(this);

    QWidget* qualifiersAndAttributes = new QWidget();

    QGridLayout* qualifierAndAttributelayout = new QGridLayout();

    QScrollArea* scrollingWidget = new QScrollArea();
    scrollingWidget->setWidgetResizable(true);
    scrollingWidget->setWidget(enumerationEditor_);
    scrollingWidget->setFrameShape(QFrame::NoFrame);
    scrollingWidget->setContentsMargins(0, 0, 0, 0);

    QWidget* attributes = new QWidget();
    QFormLayout* attributesLayout = new QFormLayout();
    attributesLayout->addRow(&resetLevelLabel_, &resetLevelLineEdit_);
    attributesLayout->addRow(&clockEnableLevelLabel_, & clockEnableLevelLineEdit_);
    attributesLayout->addRow(&powerEnableLevelLabel_, &powerEnableLevelLineEdit_);
    attributesLayout->addRow(&powerDomainRefLabel_, &powerDomainLineEdit_);
    attributesLayout->addRow(&flowTypeLabel_, &flowTypeLineEdit_);
    attributesLayout->addRow(&userFlowTypeLabel_, &userFlowTypeLineEdit_);
    attributesLayout->addRow(&userDefinedLabel_, &userDefinedLineEdit_);

    attributes->setLayout(attributesLayout);
    
    qualifierAndAttributelayout->addWidget(scrollingWidget, 0, 0);
    qualifierAndAttributelayout->addWidget(attributes, 0, 1);
    qualifierAndAttributelayout->setColumnStretch(0, 1);
    qualifierAndAttributelayout->setColumnStretch(1, 2);

    qualifiersAndAttributes->setLayout(qualifierAndAttributelayout);
    qualifiersAndAttributes->setContentsMargins(0, 0, 0, 0);

    editorLayout->addWidget(qualifiersAndAttributes);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setQualifierAttribute()
//-----------------------------------------------------------------------------
void QualifierEditor::setQualifierAttribute(QPair<Qualifier::Attribute, QString> const& attribute)
{
    auto type = attribute.first;
    auto const& value = attribute.second;

    QLineEdit* editor = getAttributeEditor(type);
    //QLabel* label = getAttributeLabel(type);

    if (editor != nullptr)
    {
        editor->setText(value);
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::getAttributeEditor()
//-----------------------------------------------------------------------------
QLineEdit* QualifierEditor::getAttributeEditor(Qualifier::Attribute attribute)
{
    if (attribute == Qualifier::Attribute::ResetLevel)
    {
        return &resetLevelLineEdit_;
    }
    else if (attribute == Qualifier::Attribute::ClockEnableLevel)
    {
        return &clockEnableLevelLineEdit_;
    }
    else if (attribute == Qualifier::Attribute::PowerEnableLevel)
    {
        return &powerEnableLevelLineEdit_;
    }
    else if (attribute == Qualifier::Attribute::PowerDomainReference)
    {
        return &powerDomainLineEdit_;
    }
    else if (attribute == Qualifier::Attribute::FlowType)
    {
        return &flowTypeLineEdit_;
    }
    else if (attribute == Qualifier::Attribute::UserFlowType)
    {
        return &userFlowTypeLineEdit_;
    }
    else if (attribute == Qualifier::Attribute::UserDefined)
    {
        return &userDefinedLineEdit_;
    }
    else
    {
        return nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::getAttributeLabel()
//-----------------------------------------------------------------------------
QLabel* QualifierEditor::getAttributeLabel(Qualifier::Attribute attribute)
{
    if (attribute == Qualifier::Attribute::ResetLevel)
    {
        return &resetLevelLabel_;
    }
    else if (attribute == Qualifier::Attribute::ClockEnableLevel)
    {
        return &clockEnableLevelLabel_;
    }
    else if (attribute == Qualifier::Attribute::PowerEnableLevel)
    {
        return &powerEnableLevelLabel_;
    }
    else if (attribute == Qualifier::Attribute::PowerDomainReference)
    {
        return &powerDomainRefLabel_;
    }
    else if (attribute == Qualifier::Attribute::FlowType)
    {
        return &flowTypeLabel_;
    }
    else if (attribute == Qualifier::Attribute::UserFlowType)
    {
        return &userFlowTypeLabel_;
    }
    else if (attribute == Qualifier::Attribute::UserDefined)
    {
        return &userDefinedLabel_;
    }
    else
    {
        return nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::updateAttributeList()
//-----------------------------------------------------------------------------
void QualifierEditor::updateAttributeList(QStringList const& selectedItems)
{
    for (auto const& qualifier : Qualifier::QUALIFIER_TYPE_STRING.keys())
    {
        if (!selectedItems.contains(Qualifier::typeToString(qualifier)))
        {
            setQualifierAttributesVisible(qualifier, false);
        }
        else
        {
            setQualifierAttributesVisible(qualifier, true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setQualifierAttributesVisible()
//-----------------------------------------------------------------------------
void QualifierEditor::setQualifierAttributesVisible(Qualifier::Type qualifier, bool visible)
{
    auto qualifierTypeAttributes = getQualifierTypeAttributes(qualifier);

    for (auto attribute : qualifierTypeAttributes)
    {
        QLineEdit* editor = getAttributeEditor(attribute);
        QLabel* label = getAttributeLabel(attribute);

        editor->setVisible(visible);
        editor->setText("");
        label->setVisible(visible);
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::getQualifierTypeAttributes()
//-----------------------------------------------------------------------------
QList<Qualifier::Attribute> QualifierEditor::getQualifierTypeAttributes(Qualifier::Type qualifier)
{
    QList<Qualifier::Attribute> qualifierTypeAttributes;

    if (qualifier == Qualifier::Reset)
    {
        qualifierTypeAttributes.append(Qualifier::Attribute::ResetLevel);
    }
    else if (qualifier == Qualifier::ClockEnable)
    {
        qualifierTypeAttributes.append(Qualifier::Attribute::ClockEnableLevel);
    }
    else if (qualifier == Qualifier::PowerEnable)
    {
        qualifierTypeAttributes.append(Qualifier::Attribute::PowerEnableLevel);
        qualifierTypeAttributes.append(Qualifier::Attribute::PowerDomainReference);
    }
    else if (qualifier == Qualifier::FlowControl)
    {
        qualifierTypeAttributes.append(Qualifier::Attribute::FlowType);
        qualifierTypeAttributes.append(Qualifier::Attribute::UserFlowType);
    }
    else if (qualifier == Qualifier::User)
    {
        qualifierTypeAttributes.append(Qualifier::Attribute::UserDefined);
    }

    return qualifierTypeAttributes;
}
