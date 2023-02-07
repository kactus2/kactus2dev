//-----------------------------------------------------------------------------
// File: EnumerationEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.01.2023
//
// Description:
// Editor for enumerations.
//-----------------------------------------------------------------------------

#include "EnumerationEditor.h"

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <QPushButton>
#include <QScrollArea>

//-----------------------------------------------------------------------------
// Function: EnumerationEditor::EnumerationEditor()
//-----------------------------------------------------------------------------
EnumerationEditor::EnumerationEditor(QWidget* parent):
QFrame(parent),
enumCollectionEditor_(new EnumCollectionEditor(this)),
selectAllCheck_(new QCheckBox("All"))
{
    selectAllCheck_->setToolTip("Select / deselect all");

    selectAllCheck_->setTristate(true);

    setFrameStyle(QFrame::StyledPanel);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_NoMousePropagation);
    setAutoFillBackground(true);

    setupLayout();

    connect(enumCollectionEditor_, SIGNAL(itemStateChanged(Qt::CheckState)), this, SLOT(onChangeCheckAllState(Qt::CheckState)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditor::setupLayout()
//-----------------------------------------------------------------------------
void EnumerationEditor::setupLayout()
{
    enumCollectionEditor_->setContentsMargins(0, 0, 0, 0);

    QScrollArea* scrollingWidget = new QScrollArea();
    scrollingWidget->setWidgetResizable(true);
    scrollingWidget->setWidget(enumCollectionEditor_);
    scrollingWidget->setFrameShape(QFrame::NoFrame);
    scrollingWidget->setContentsMargins(0, 0, 0, 0);

    connect(selectAllCheck_, SIGNAL(clicked(bool)), this, SLOT(onCheckAllStateChange(bool)), Qt::UniqueConnection);

    QPushButton* okButton = new QPushButton();
    okButton->setIcon(QIcon(":/icons/common/graphics/checkMark.png"));
    okButton->setToolTip("Accept");

    QPushButton* cancelButton = new QPushButton();
    cancelButton->setIcon(QIcon(":/icons/common/graphics/grey_cross.png"));
    cancelButton->setToolTip("Cancel");

    connect(okButton, SIGNAL(clicked()), this, SIGNAL(finishEditing()), Qt::UniqueConnection);
    connect(cancelButton, SIGNAL(clicked()), this, SIGNAL(cancelEditing()), Qt::UniqueConnection);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(selectAllCheck_);
    buttonLayout->addStretch(10);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QFrame* line = new QFrame();
    line->setLineWidth(1);
    line->setMidLineWidth(1);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(scrollingWidget, 10);
    mainLayout->addWidget(line);
    mainLayout->addLayout(buttonLayout, 0);
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditor::onCheckAllStateChange()
//-----------------------------------------------------------------------------
void EnumerationEditor::onCheckAllStateChange(bool newState)
{
    if (newState == true)
    {
        selectAllCheck_->setChecked(true);
        selectAllCheck_->repaint();
    }

    enumCollectionEditor_->changeStatesOfAllItems(newState);
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditor::onChangeCheckAllState()
//-----------------------------------------------------------------------------
void EnumerationEditor::onChangeCheckAllState(Qt::CheckState newState)
{
    selectAllCheck_->setCheckState(newState);
    selectAllCheck_->repaint();
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditor::getSelectedItems()
//-----------------------------------------------------------------------------
QStringList EnumerationEditor::getSelectedItems() const
{
    return enumCollectionEditor_->getSelectedItems();
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditor::setupItems()
//-----------------------------------------------------------------------------
void EnumerationEditor::setupItems(QStringList const& availableItems, QStringList const& selectedItems)
{
    int includedItemCount = 0;

    foreach(QString const& name, availableItems)
    {
        bool nameIsSelected = selectedItems.contains(name);
        if (nameIsSelected)
        {
            includedItemCount++;
        }

        enumCollectionEditor_->addItem(name, nameIsSelected);
    }

    Qt::CheckState selectAllState = Qt::PartiallyChecked;
    if (includedItemCount == 0)
    {
        selectAllState = Qt::Unchecked;
    }
    else if (includedItemCount == availableItems.size())
    {
        selectAllState = Qt::Checked;
    }

    selectAllCheck_->setCheckState(selectAllState);
}
