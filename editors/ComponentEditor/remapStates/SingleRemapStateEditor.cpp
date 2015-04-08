//-----------------------------------------------------------------------------
// File: SingleRemapStateEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.04.2015
//
// Description:
// Editor to edit a remap state within a component.
//-----------------------------------------------------------------------------

#include "SingleRemapStateEditor.h"

#include <QScrollArea>

//-----------------------------------------------------------------------------
// Function: SingleRemapStateEditor::SingleRemapStateEditor()
//-----------------------------------------------------------------------------
SingleRemapStateEditor::SingleRemapStateEditor(QSharedPointer<Component> component,
    QSharedPointer<RemapState> remapState, LibraryInterface* libHandler, QWidget* parent /* = 0 */):
ItemEditor(component, libHandler, parent),
remapState_(remapState),
nameEditor_(remapState_->getNamegroup(), this, tr("Remap state name and description"))
{
    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: SingleRemapStateEditor::~SingleRemapStateEditor()
//-----------------------------------------------------------------------------
SingleRemapStateEditor::~SingleRemapStateEditor()
{

}

//-----------------------------------------------------------------------------
// Function: SingleRemapStateEditor::isValid()
//-----------------------------------------------------------------------------
bool SingleRemapStateEditor::isValid() const
{
    return nameEditor_.isValid();
}

//-----------------------------------------------------------------------------
// Function: SingleRemapStateEditor::refresh()
//-----------------------------------------------------------------------------
void SingleRemapStateEditor::refresh()
{
    nameEditor_.refresh();
}

//-----------------------------------------------------------------------------
// Function: SingleRemapStateEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleRemapStateEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* nameLayout = new QVBoxLayout();
    nameLayout->addWidget(&nameEditor_, 0, Qt::AlignTop);

    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(topWidget);

    QVBoxLayout* topLayout = new QVBoxLayout(topWidget);
    topLayout->addLayout(nameLayout);
    topLayout->setContentsMargins(0,0,0,0);
}
