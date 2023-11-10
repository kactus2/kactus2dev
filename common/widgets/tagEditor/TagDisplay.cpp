//-----------------------------------------------------------------------------
// File: TagDisplay.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.09.2019
//
// Description:
// Display for tags.
//-----------------------------------------------------------------------------

#include "TagDisplay.h"

#include <IPXACTmodels/common/TagData.h>

#include <common/widgets/tagEditor/TagLabel.h>
#include <KactusAPI/include/TagManager.h>
#include <common/widgets/tagEditor/TagCompleterModel.h>

#include <QHBoxLayout>
#include <QApplication>
#include <QFocusEvent>

//-----------------------------------------------------------------------------
// Function: TagDisplay::TagDisplay()
//-----------------------------------------------------------------------------
TagDisplay::TagDisplay(TagLabel* tagLabel, QWidget* parent):
QFrame(parent),
editedLabel_(tagLabel)
{

}

//-----------------------------------------------------------------------------
// Function: TagDisplay::setupNameEditor()
//-----------------------------------------------------------------------------
void TagDisplay::setupEditors(QWidget* nameEditor)
{
    nameEditor->setFixedHeight(BUTTON_SIDE);

    okButton_->setFixedHeight(BUTTON_SIDE);
    okButton_->setFixedWidth(BUTTON_SIDE);

    deleteButton_->setFixedHeight(BUTTON_SIDE);
    deleteButton_->setFixedWidth(BUTTON_SIDE);

    deleteButton_->adjustSize();

    colorButton_->setToolTip(tr("Select Color"));
    okButton_->setToolTip(tr("Ok"));
    deleteButton_->setToolTip(tr("Delete Tag"));

    colorButton_->setColor(editedLabel_->palette().color(QPalette::Window));
    colorButton_->update();
    colorButton_->setVisible(showColorButton());

    setupLayout(nameEditor);
    connectSignals();

    setFrameStyle(QFrame::Panel);
    setLineWidth(1);
}

//-----------------------------------------------------------------------------
// Function: TagDisplay::setupLayout()
//-----------------------------------------------------------------------------
void TagDisplay::setupLayout(QWidget* nameEditor)
{
    auto editorLayout = new QHBoxLayout(this);
    editorLayout->setContentsMargins(1, 1, 1, 1);
    editorLayout->setSpacing(2);

    editorLayout->addWidget(nameEditor);
    editorLayout->addWidget(colorButton_);
    editorLayout->addWidget(okButton_);
    editorLayout->addWidget(deleteButton_);
}

//-----------------------------------------------------------------------------
// Function: TagDisplay::connectSignals()
//-----------------------------------------------------------------------------
void TagDisplay::connectSignals()
{
    connect(okButton_, SIGNAL(clicked()), this, SLOT(onAcceptChanges()), Qt::UniqueConnection);
    connect(deleteButton_, SIGNAL(clicked()), this, SLOT(onDeleteItem()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: TagDisplay::completerColorChange()
//-----------------------------------------------------------------------------
void TagDisplay::completerColorChange(QColor const& newColor)
{
    colorButton_->setColor(newColor);
    colorButton_->update();
}

//-----------------------------------------------------------------------------
// Function: TagDisplay::onAcceptChanges()
//-----------------------------------------------------------------------------
void TagDisplay::onAcceptChanges()
{
    if (QString newName = getNewName(); !newName.isEmpty())
    {
        editedLabel_->setText(newName);
    }

    editedLabel_->setPalette(colorButton_->palette());

    emit acceptChanges(editedLabel_, this);
}

//-----------------------------------------------------------------------------
// Function: TagDisplay::onDeleteItem()
//-----------------------------------------------------------------------------
void TagDisplay::onDeleteItem()
{
    emit deleteItem(editedLabel_, this);
}

//-----------------------------------------------------------------------------
// Function: TagDisplay::focusOutEvent()
//-----------------------------------------------------------------------------
void TagDisplay::focusOutEvent(QFocusEvent* /*event*/)
{
    if ((QApplication::focusWidget() && QApplication::focusWidget()->parentWidget() == this) ||
        (QApplication::activeWindow() && QApplication::activeWindow()->parentWidget() == this))
    {
        return;
    }
    else
    {
        onAcceptChanges();
    }
}

//-----------------------------------------------------------------------------
// Function: TagDisplay::eventFilter()
//-----------------------------------------------------------------------------
bool TagDisplay::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FocusOut)
    {
        focusOutEvent(dynamic_cast<QFocusEvent*>(event));
        return true;
    }
    else
    {
        return QFrame::eventFilter(watched, event);
    }
}

//-----------------------------------------------------------------------------
// Function: TagDisplay::showColorButton()
//-----------------------------------------------------------------------------
bool TagDisplay::showColorButton() const noexcept
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: TagDisplay::keyPressEvent()
//-----------------------------------------------------------------------------
void TagDisplay::keyPressEvent(QKeyEvent *event)
{
    QFrame::keyPressEvent(event);

    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        onAcceptChanges();
    }
    else if (event->key() == Qt::Key_Escape)
    {
        emit acceptChanges(editedLabel_, this);
    }
}

//-----------------------------------------------------------------------------
// Function: TagDisplay::getColorButton()
//-----------------------------------------------------------------------------
ColorBox* TagDisplay::getColorButton()
{
    return colorButton_;
}

//-----------------------------------------------------------------------------
// Function: TagDisplay::getEditedLabel()
//-----------------------------------------------------------------------------
TagLabel* TagDisplay::getEditedLabel() const
{
    return editedLabel_;
}
