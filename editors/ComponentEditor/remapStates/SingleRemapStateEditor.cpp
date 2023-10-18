//-----------------------------------------------------------------------------
// File: SingleRemapStateEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.04.2015
//
// Description:
// Editor to edit a remap state within a component.
//-----------------------------------------------------------------------------

#include "SingleRemapStateEditor.h"

#include <IPXACTmodels/Component/RemapState.h>

#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: SingleRemapStateEditor::SingleRemapStateEditor()
//-----------------------------------------------------------------------------
SingleRemapStateEditor::SingleRemapStateEditor(QSharedPointer<Component> component,
    QSharedPointer<RemapState> remapState, LibraryInterface* libHandler,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QWidget* parent /* = 0 */):
ItemEditor(component, libHandler, parent),
remapState_(remapState),
nameEditor_(remapState_, component->getRevision(), this, tr("Remap state name and description")),
remapConditionEditor_(new RemapConditionEditor(remapState->getRemapPorts(), component, parameterFinder,
expressionFormatter, this))
{
    connect(remapConditionEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(remapConditionEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(remapConditionEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: SingleRemapStateEditor::refresh()
//-----------------------------------------------------------------------------
void SingleRemapStateEditor::refresh()
{
    nameEditor_.refresh();
    remapConditionEditor_->refresh();
}

//-----------------------------------------------------------------------------
// Function: SingleRemapStateEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleRemapStateEditor::showEvent(QShowEvent * event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/singleRemapState.html");
}

//-----------------------------------------------------------------------------
// Function: SingleRemapStateEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleRemapStateEditor::setupLayout()
{
    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    auto nameLayout = new QVBoxLayout();
    nameLayout->addWidget(&nameEditor_, 0, Qt::AlignTop);

    auto remapConditionLayout = new QVBoxLayout();
    remapConditionLayout->addWidget(remapConditionEditor_, 0);

    auto topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(topWidget);

    auto topLayout = new QVBoxLayout(topWidget);
    topLayout->addLayout(nameLayout);
    topLayout->addLayout(remapConditionLayout);
    topLayout->setContentsMargins(0,0,0,0);
}
