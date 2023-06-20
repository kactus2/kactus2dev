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
void QualifierEditor::setupEditor(QStringList const& allQualifiers, QStringList const& activeQualifiers)
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
    attributesLayout->addRow(QStringLiteral("Reset level:"), &resetLevelLineEdit_);
    attributesLayout->addRow(QStringLiteral("Clock enable level:"), &clockEnableLevelLineEdit_);
    attributesLayout->addRow(QStringLiteral("Power enable level:"), &powerEnableLevelLineEdit_);
    attributesLayout->addRow(QStringLiteral("Power domain:"), &powerDomainLineEdit_);
    attributesLayout->addRow(QStringLiteral("Flow type:"), &flowTypeLineEdit_);
    attributesLayout->addRow(QStringLiteral("User flow type:"), &userFlowTypeLineEdit_);
    attributesLayout->addRow(QStringLiteral("User defined:"), &userDefinedLineEdit_);

    attributes->setLayout(attributesLayout);
    
    qualifierAndAttributelayout->addWidget(scrollingWidget, 0, 0);
    qualifierAndAttributelayout->addWidget(attributes, 0, 1);
    qualifierAndAttributelayout->setColumnStretch(0, 1);
    qualifierAndAttributelayout->setColumnStretch(1, 2);

    qualifiersAndAttributes->setLayout(qualifierAndAttributelayout);
    qualifiersAndAttributes->setContentsMargins(0, 0, 0, 0);

    editorLayout->addWidget(qualifiersAndAttributes);
}
