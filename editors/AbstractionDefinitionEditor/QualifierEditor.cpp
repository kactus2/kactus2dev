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
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: QualifierEditor::QualifierEditor()
//-----------------------------------------------------------------------------
QualifierEditor::QualifierEditor(Document::Revision stdRevision, QWidget *parent) :
    QFrame(parent),
    qualifierAndAttributelayout_(new QGridLayout()),
    powerDomainLineEdit_(new QLineEdit()),
    userDefinedLineEdit_(new QLineEdit()),
    resetLevelSelector_(new QComboBox()),
    clockEnableLevelSelector_(new QComboBox()),
    powerEnableLevelSelector_(new QComboBox()),
    flowTypeSelector_(new QComboBox()),
    stdRevision_(stdRevision)
{
    setFrameStyle(QFrame::StyledPanel);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_NoMousePropagation);
    setAutoFillBackground(true);

    resetLevelSelector_->addItems({ QStringLiteral(""), QStringLiteral("low"), QStringLiteral("high")});
    clockEnableLevelSelector_->addItems({ QStringLiteral(""), QStringLiteral("low"), QStringLiteral("high") });
    powerEnableLevelSelector_->addItems({ QStringLiteral(""), QStringLiteral("low"), QStringLiteral("high") });

    flowTypeSelector_->setEditable(true);
    flowTypeSelector_->setInsertPolicy(QComboBox::InsertAtTop);
    flowTypeSelector_->addItems({
        QStringLiteral("creditReturn"),
        QStringLiteral("ready"),
        QStringLiteral("busy")
    });

    resetLevelSelector_->setToolTip(QStringLiteral("Reset level"));
    clockEnableLevelSelector_->setToolTip(QStringLiteral("Clock enable level"));
    powerEnableLevelSelector_->setToolTip(QStringLiteral("Power enable level"));
    flowTypeSelector_->setToolTip(QStringLiteral("Flow type"));
    userDefinedLineEdit_->setToolTip(QStringLiteral("User defined"));

    flowTypeSelector_->lineEdit()->setPlaceholderText(QStringLiteral("Flow type"));
    userDefinedLineEdit_->setPlaceholderText(QStringLiteral("User defined attribute"));

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setupEditor()
//-----------------------------------------------------------------------------
void QualifierEditor::setupEditor(QStringList const& qualifierOptions)
{    
    for (auto const& qualifier : qualifierOptions)
    {
        auto qualifierAsType = Qualifier::stringToType(qualifier);
        auto attributeEditor = getAttributeEditor(qualifierAsType);

        QCheckBox* qualifierBox = new QCheckBox(qualifier, this);
        qualifierBoxes_.insert(qualifierAsType, qualifierBox);

        connect(qualifierBox, SIGNAL(clicked(bool)), this, SLOT(onItemClicked(bool)), Qt::UniqueConnection);
        
        qualifierAndAttributelayout_->addWidget(qualifierBox, qualifierAndAttributelayout_->rowCount(), 0);

        if (attributeEditor)
        {
            qualifierAndAttributelayout_->addWidget(attributeEditor, qualifierAndAttributelayout_->rowCount() - 1, 1);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setupEditorData()
//-----------------------------------------------------------------------------
void QualifierEditor::setupEditorData(QStringList const& activeQualifiers, QMap<QString, QString> const& attributes)
{
    for (auto const& qualifier : activeQualifiers)
    {
        auto qualifierAsType = Qualifier::stringToType(qualifier);

        if (qualifierBoxes_.contains(qualifierAsType))
        {
            qualifierBoxes_[qualifierAsType]->setChecked(true);            
            setQualifierAttributesVisible(qualifierAsType, true);
        }
    }

    for (auto const& attributeName : attributes.keys())
    {
        setQualifierAttribute(Qualifier::stringToAttributeName(attributeName), attributes[attributeName]);
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::hideAllAttributes()
//-----------------------------------------------------------------------------
void QualifierEditor::hideAllAttributes()
{
    setQualifierAttributesVisible(Qualifier::Type::ClockEnable, false);
    setQualifierAttributesVisible(Qualifier::Type::FlowControl, false);
    setQualifierAttributesVisible(Qualifier::Type::PowerEnable, false);
    setQualifierAttributesVisible(Qualifier::Type::Reset, false);
    setQualifierAttributesVisible(Qualifier::Type::User, false);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::getSelectedItems()
//-----------------------------------------------------------------------------
QStringList QualifierEditor::getSelectedItems() const
{
    QStringList selected;
    for (auto const& item : qualifierBoxes_)
    {
        if (item->isChecked())
        {
            selected.append(item->text());
        }
    }

    return selected;
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::getAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> QualifierEditor::getAttributes() const
{
    QMap<QString, QString> attributes;

    auto resetLevel = resetLevelSelector_->isVisible() ? resetLevelSelector_->currentText() : QString();
    auto clockEnableLevel = clockEnableLevelSelector_->isVisible() ? clockEnableLevelSelector_->currentText() : QString();
    auto powerEnableLevel = powerEnableLevelSelector_->isVisible() ? powerEnableLevelSelector_->currentText() : QString();

    attributes.insert(QStringLiteral("resetLevel"), resetLevel);
    attributes.insert(QStringLiteral("clockEnableLevel"), clockEnableLevel);
    attributes.insert(QStringLiteral("powerEnableLevel"), powerEnableLevel);
    attributes.insert(QStringLiteral("powerDomainReference"), powerDomainLineEdit_->text());
    
    if (QString const& flowTypeText = flowTypeSelector_->currentText(); flowTypeText.isEmpty())
    {
        attributes.insert(QStringLiteral("flowType"), QString());
        attributes.insert(QStringLiteral("userFlowType"), QString());
    }
    else if (flowTypes_.contains(flowTypeText))
    {
        attributes.insert(QStringLiteral("flowType"), flowTypeText);
        attributes.insert(QStringLiteral("userFlowType"), QString());
    }
    else
    {
        attributes.insert(QStringLiteral("flowType"), QStringLiteral("user"));
        attributes.insert(QStringLiteral("userFlowType"), flowTypeText);
    }

    attributes.insert(QStringLiteral("userDefined"), userDefinedLineEdit_->text());
    return attributes;
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::getQualifierData()
//-----------------------------------------------------------------------------
QualifierData QualifierEditor::getQualifierData() const
{
    return QualifierData{ getSelectedItems(), getAttributes() };
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::onItemClicked()
//-----------------------------------------------------------------------------
void QualifierEditor::onItemClicked(bool isChecked)
{
    QCheckBox* sender = qobject_cast<QCheckBox*>(QObject::sender());
    if (!sender)
    {
        return;
    }

    if (stdRevision_ == Document::Revision::Std22)
    {
        auto qualifier = Qualifier::stringToType(sender->text());
        setQualifierAttributesVisible(qualifier, isChecked);
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setupLayout()
//-----------------------------------------------------------------------------
void QualifierEditor::setupLayout()
{
    QVBoxLayout* editorLayout = new QVBoxLayout(this);

    QWidget* qualifiersAndAttributes = new QWidget();

    QScrollArea* scrollingWidget = new QScrollArea();
    scrollingWidget->setWidgetResizable(true);
    scrollingWidget->setWidget(qualifiersAndAttributes);
    scrollingWidget->setFrameShape(QFrame::NoFrame);
    scrollingWidget->setContentsMargins(0, 0, 0, 0);
    
    QPushButton* okButton = new QPushButton();
    okButton->setIcon(QIcon(":/icons/common/graphics/checkMark.png"));
    okButton->setToolTip("Accept");

    QPushButton* cancelButton = new QPushButton();
    cancelButton->setIcon(QIcon(":/icons/common/graphics/grey_cross.png"));
    cancelButton->setToolTip("Cancel");

    connect(okButton, SIGNAL(clicked()), this, SIGNAL(finishEditing()), Qt::UniqueConnection);
    connect(cancelButton, SIGNAL(clicked()), this, SIGNAL(cancelEditing()), Qt::UniqueConnection);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(10);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    qualifiersAndAttributes->setLayout(qualifierAndAttributelayout_);
    qualifiersAndAttributes->setContentsMargins(0, 0, 0, 0);
    setMinimumHeight(sizeHint().height());
    editorLayout->addWidget(scrollingWidget);
    editorLayout->addLayout(buttonLayout, 0);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setQualifierAttribute()
//-----------------------------------------------------------------------------
void QualifierEditor::setQualifierAttribute(Qualifier::Attribute attributeType, QString const& attributeValue)
{
    if (attributeType == Qualifier::Attribute::UserDefined)
    {
        userDefinedLineEdit_->setText(attributeValue);
    }
    else if (attributeType == Qualifier::Attribute::UserFlowType && !attributeValue.isEmpty())
    {
        flowTypeSelector_->setCurrentText(attributeValue); // Display user attribute when flow type is user.
    }
    else
    {
        QComboBox* editor = getAttributeEditor(attributeType);

        if (editor)
        {
            editor->setCurrentText(attributeValue);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::getAttributeEditor()
//-----------------------------------------------------------------------------
QComboBox* QualifierEditor::getAttributeEditor(Qualifier::Attribute attribute)
{
    if (attribute == Qualifier::Attribute::ResetLevel)
    {
        return resetLevelSelector_;
    }
    else if (attribute == Qualifier::Attribute::ClockEnableLevel)
    {
        return clockEnableLevelSelector_;
    }
    else if (attribute == Qualifier::Attribute::PowerEnableLevel)
    {
        return powerEnableLevelSelector_;
    }
    else if (attribute == Qualifier::Attribute::FlowType)
    {
        return flowTypeSelector_;
    }
    else
    {
        return nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::getAttributeEditor()
//-----------------------------------------------------------------------------
QWidget* QualifierEditor::getAttributeEditor(Qualifier::Type qualifierType)
{
    if (qualifierType == Qualifier::Type::Reset)
    {
        return resetLevelSelector_;
    }
    else if (qualifierType == Qualifier::Type::ClockEnable)
    {
        return clockEnableLevelSelector_;
    }
    else if (qualifierType == Qualifier::Type::PowerEnable)
    {
        return powerEnableLevelSelector_;
    }
    else if (qualifierType == Qualifier::Type::FlowControl)
    {
        return flowTypeSelector_;
    }
    else if (qualifierType == Qualifier::Type::User)
    {
        return userDefinedLineEdit_;
    }
    else
    {
        return nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setQualifierAttributesVisible()
//-----------------------------------------------------------------------------
void QualifierEditor::setQualifierAttributesVisible(Qualifier::Type qualifier, bool visible)
{
    if (qualifier == Qualifier::Type::User)
    {
        userDefinedLineEdit_->setVisible(visible);

        if (!visible)
        {
            userDefinedLineEdit_->setText("");
        }

        return;
    }

    for (auto attribute : getQualifierTypeAttributes(qualifier))
    {
        QComboBox* editor = getAttributeEditor(attribute);
            
        if (editor)
        {
            editor->setVisible(visible);
            if (!visible)
            {
                editor->setCurrentText("");
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::getQualifierTypeAttributes()
//-----------------------------------------------------------------------------
QList<Qualifier::Attribute> QualifierEditor::getQualifierTypeAttributes(Qualifier::Type qualifier)
{
    QList<Qualifier::Attribute> qualifierTypeAttributes;

    if (qualifier == Qualifier::Type::Reset)
    {
        qualifierTypeAttributes.append(Qualifier::Attribute::ResetLevel);
    }
    else if (qualifier == Qualifier::Type::ClockEnable)
    {
        qualifierTypeAttributes.append(Qualifier::Attribute::ClockEnableLevel);
    }
    else if (qualifier == Qualifier::Type::PowerEnable)
    {
        qualifierTypeAttributes.append(Qualifier::Attribute::PowerEnableLevel);
        qualifierTypeAttributes.append(Qualifier::Attribute::PowerDomainReference);
    }
    else if (qualifier == Qualifier::Type::FlowControl)
    {
        qualifierTypeAttributes.append(Qualifier::Attribute::FlowType);
        qualifierTypeAttributes.append(Qualifier::Attribute::UserFlowType);
    }
    else if (qualifier == Qualifier::Type::User)
    {
        qualifierTypeAttributes.append(Qualifier::Attribute::UserDefined);
    }

    return qualifierTypeAttributes;
}
