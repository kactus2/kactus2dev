//-----------------------------------------------------------------------------
// File: FramedTreeItemEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 6.9.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#include "FramedTreeItemEditor.h"

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <QPushButton>
#include <QScrollArea>

//-----------------------------------------------------------------------------
// Function: FramedTreeItemEditor::FramedTreeItemEditor()
//-----------------------------------------------------------------------------
FramedTreeItemEditor::FramedTreeItemEditor(MemoryTreeFactory const* factory, QWidget* parent):
QFrame(parent),
editor_(factory, this)
{
    setFrameStyle(QFrame::StyledPanel);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_NoMousePropagation);
    setAutoFillBackground(true);

    okButton_->setIcon(QIcon(":/icons/common/graphics/checkMark.png"));
    okButton_->setToolTip("Accept");
    okButton_->setEnabled(false);

    cancelButton_->setIcon(QIcon(":/icons/common/graphics/cross.png"));
    cancelButton_->setToolTip("Cancel");

    connect(okButton_, SIGNAL(clicked()), this, SIGNAL(finishEditing()), Qt::UniqueConnection);
    connect(cancelButton_, SIGNAL(clicked()), this, SIGNAL(cancelEditing()), Qt::UniqueConnection);

    connect(&editor_, SIGNAL(selectionChanged()), this, SLOT(validateSelection()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: FramedTreeItemEditor::selectPath()
//-----------------------------------------------------------------------------
void FramedTreeItemEditor::selectPath(QStringList const& path)
{
    editor_.selectPath(path);
    validateSelection();
}

//-----------------------------------------------------------------------------
// Function: FramedTreeItemEditor::selectedPath()
//-----------------------------------------------------------------------------
QStringList FramedTreeItemEditor::selectedPath() const
{
    return editor_.selectedPath();
}

//-----------------------------------------------------------------------------
// Function: FramedTreeItemEditor::validateSelection()
//-----------------------------------------------------------------------------
void FramedTreeItemEditor::validateSelection()
{
    auto path = editor_.selectedPath();

    okButton_->setEnabled(path.isEmpty() == false && path.last().startsWith("Field"));
}

//-----------------------------------------------------------------------------
// Function: FramedTreeItemEditor::setupLayout()
//-----------------------------------------------------------------------------
void FramedTreeItemEditor::setupLayout()
{
    editor_.setContentsMargins(0, 0, 0, 0);

    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(10);
    buttonLayout->addWidget(okButton_);
    buttonLayout->addWidget(cancelButton_);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(&editor_, 10);
    mainLayout->addLayout(buttonLayout, 0);
    mainLayout->setSpacing(1);
    mainLayout->setContentsMargins(0, 0, 0, 0);

}
