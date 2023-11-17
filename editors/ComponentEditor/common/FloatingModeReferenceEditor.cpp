//-----------------------------------------------------------------------------
// File: FloatingModeReferenceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 16.10.2023
//
// Description:
// A mode reference editor for tables containing mode references.
//-----------------------------------------------------------------------------

#include "FloatingModeReferenceEditor.h"
#include "ModeReferenceEditor.h"

#include <QVBoxLayout>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: FloatingModeReferenceEditor::FloatingModeReferenceEditor()
//-----------------------------------------------------------------------------
FloatingModeReferenceEditor::FloatingModeReferenceEditor(ModeReferenceInterface* modeRefInterface, QWidget* parent):
    QFrame(parent)
{
    modeReferencesEditor_ = new ModeReferenceEditor(modeRefInterface, this);

    setFrameStyle(QFrame::StyledPanel);
    setAutoFillBackground(true);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: FloatingModeReferenceEditor::getTableSizeHint()
//-----------------------------------------------------------------------------
QSize FloatingModeReferenceEditor::getTableSizeHint() const
{
    return modeReferencesEditor_->sizeHint();
}

//-----------------------------------------------------------------------------
// Function: FloatingModeReferenceEditor::getTableMargins()
//-----------------------------------------------------------------------------
QMargins FloatingModeReferenceEditor::getTableMargins() const
{
    return layout()->contentsMargins();
}

//-----------------------------------------------------------------------------
// Function: FloatingModeReferenceEditor::setupLayout()
//-----------------------------------------------------------------------------
void FloatingModeReferenceEditor::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->setContentsMargins(4, 4, 4, 4);
    
    QPushButton* okButton = new QPushButton();
    okButton->setIcon(QIcon(":/icons/common/graphics/checkMark.png"));
    okButton->setToolTip("Accept");

    QPushButton* cancelButton = new QPushButton();
    cancelButton->setIcon(QIcon(":/icons/common/graphics/grey_cross.png"));
    cancelButton->setToolTip("Cancel");

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(10);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    topLayout->addWidget(modeReferencesEditor_);
    topLayout->addLayout(buttonLayout, 0);

    connect(okButton, SIGNAL(clicked()), this, SIGNAL(finishEditing()), Qt::UniqueConnection);
    connect(cancelButton, SIGNAL(clicked()), this, SIGNAL(cancelEditing()), Qt::UniqueConnection);

}
