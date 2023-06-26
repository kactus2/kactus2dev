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
QualifierEditor::QualifierEditor(QWidget* parent):
    QFrame(parent),
    powerDomainLineEdit_(new QLineEdit()),
    userDefinedLineEdit_(new QLineEdit()),
    resetLevelSelector_(new QComboBox()),
    clockEnableLevelSelector_(new QComboBox()),
    powerEnableLevelSelector_(new QComboBox()),
    flowTypeSelector_(new QComboBox())
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

    populateCheckBoxes();
    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setupEditor()
//-----------------------------------------------------------------------------
void QualifierEditor::setupEditor(QStringList const& allQualifiers, QStringList const& activeQualifiers, QMap<QString, QString> attributes)
{
    for (auto const& qualifier : allQualifiers)
    {
        bool selected = activeQualifiers.contains(qualifier);

        for (auto box : qualifierBoxes_)
        {
            if (box->text() == qualifier)
            {
                box->setChecked(selected);
            }
        }

        setQualifierAttributesVisible(Qualifier::stringToType(qualifier), selected);
    }

    for (auto const& attributeName : attributes.keys())
    {
        setQualifierAttribute(Qualifier::stringToAttributeName(attributeName), attributes[attributeName]);
    }
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
    return QualifierData(getSelectedItems(), getAttributes());
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

    auto qualifier = Qualifier::stringToType(sender->text());
    setQualifierAttributesVisible(qualifier, isChecked);
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::populateCheckBoxes()
//-----------------------------------------------------------------------------
void QualifierEditor::populateCheckBoxes()
{
    for (auto const& attribute : Qualifier::QUALIFIER_TYPE_STRING.values())
    {
        QCheckBox* qualifierBox = new QCheckBox(attribute, this);
        qualifierBoxes_.append(qualifierBox);

        connect(qualifierBox, SIGNAL(clicked(bool)), this, SLOT(onItemClicked(bool)), Qt::UniqueConnection);
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierEditor::setupLayout()
//-----------------------------------------------------------------------------
void QualifierEditor::setupLayout()
{
    QVBoxLayout* editorLayout = new QVBoxLayout(this);

    QWidget* qualifiersAndAttributes = new QWidget();

    QGridLayout* qualifierAndAttributelayout = new QGridLayout();

    QScrollArea* scrollingWidget = new QScrollArea();
    scrollingWidget->setWidgetResizable(true);
    scrollingWidget->setWidget(qualifiersAndAttributes);
    scrollingWidget->setFrameShape(QFrame::NoFrame);
    scrollingWidget->setContentsMargins(0, 0, 0, 0);
    
    for (int i = 0; i < qualifierBoxes_.size(); ++i)
    {
        qualifierAndAttributelayout->addWidget(qualifierBoxes_.at(i), i, 0);
    }

    qualifierAndAttributelayout->addWidget(resetLevelSelector_, 3, 1);
    qualifierAndAttributelayout->addWidget(clockEnableLevelSelector_, 6, 1);
    qualifierAndAttributelayout->addWidget(powerEnableLevelSelector_, 7, 1);
    qualifierAndAttributelayout->addWidget(flowTypeSelector_, 10, 1);
    qualifierAndAttributelayout->addWidget(userDefinedLineEdit_, 11, 1);
    
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

    qualifiersAndAttributes->setLayout(qualifierAndAttributelayout);
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
    if (attributeType == Qualifier::UserDefined)
    {
        userDefinedLineEdit_->setText(attributeValue);
    }
    else if (attributeType == Qualifier::UserFlowType && !attributeValue.isEmpty())
    {
        flowTypeSelector_->setCurrentText(attributeValue);
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
// Function: QualifierEditor::setQualifierAttributesVisible()
//-----------------------------------------------------------------------------
void QualifierEditor::setQualifierAttributesVisible(Qualifier::Type qualifier, bool visible)
{
    if (qualifier == Qualifier::User)
    {
        userDefinedLineEdit_->setVisible(visible);

        if (!visible)
        {
            userDefinedLineEdit_->setText("");
        }

        return;
    }
    
    auto qualifierTypeAttributes = getQualifierTypeAttributes(qualifier);

    for (auto attribute : qualifierTypeAttributes)
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
