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

#include <QScrollArea>

//-----------------------------------------------------------------------------
// Function: EnumerationEditor::EnumerationEditor()
//-----------------------------------------------------------------------------
EnumerationEditor::EnumerationEditor(bool hideCheckAll, QWidget* parent):
QFrame(parent),
enumCollectionEditor_(new EnumCollectionEditor(this)),
selectAllCheck_(new QCheckBox("All")),
okButton_(new QPushButton()),
cancelButton_(new QPushButton())
{
    if (hideCheckAll)
    {
        selectAllCheck_->hide();
    }

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

    okButton_->setIcon(QIcon(":/icons/common/graphics/checkMark.png"));
    okButton_->setToolTip("Accept");

    cancelButton_->setIcon(QIcon(":/icons/common/graphics/grey_cross.png"));
    cancelButton_->setToolTip("Cancel");

    connect(okButton_, SIGNAL(clicked()), this, SIGNAL(finishEditing()), Qt::UniqueConnection);
    connect(cancelButton_, SIGNAL(clicked()), this, SIGNAL(cancelEditing()), Qt::UniqueConnection);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(selectAllCheck_);
    buttonLayout->addStretch(10);
    buttonLayout->addWidget(okButton_);
    buttonLayout->addWidget(cancelButton_);

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
void EnumerationEditor::setupItems(QStringList const& availableItems, QStringList const& exclusiveItems, QStringList const& selectedItems)
{
    int includedItemCount = 0;

    foreach(QString const& name, availableItems)
    {
        bool nameIsSelected = selectedItems.contains(name);
        if (nameIsSelected)
        {
            includedItemCount++;
        }

        bool portIsExclusive = false;
        if (exclusiveItems.contains(name))
        {
            portIsExclusive = true;
        }

        enumCollectionEditor_->addItem(name, portIsExclusive, nameIsSelected);
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

//-----------------------------------------------------------------------------
// Function: EnumerationEditor::hideOkAndCancelButtons()
//-----------------------------------------------------------------------------
void EnumerationEditor::hideOkAndCancelButtons()
{
    okButton_->hide();
    cancelButton_->hide();
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditor::showOkAndCancelButtons()
//-----------------------------------------------------------------------------
void EnumerationEditor::showOkAndCancelButtons()
{
    okButton_->show();
    cancelButton_->show();
}
