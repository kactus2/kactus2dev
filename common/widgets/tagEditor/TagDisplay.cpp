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

#include <common/widgets/colorBox/ColorBox.h>
#include <common/widgets/tagEditor/TagLabel.h>
#include <common/widgets/tagEditor/TagManager.h>
#include <common/widgets/tagEditor/TagCompleterModel.h>

#include <QHBoxLayout>
#include <QApplication>
#include <QFocusEvent>

//-----------------------------------------------------------------------------
// Function: TagDisplay::TagDisplay()
//-----------------------------------------------------------------------------
TagDisplay::TagDisplay(TagLabel* tagLabel, QWidget* parent /* = 0 */):
QFrame(parent),
colorButton_(),
deleteButton_(new QPushButton(QIcon(":/icons/common/graphics/cross.png"), QLatin1String(""), this)),
editedLabel_(tagLabel)
{

}

//-----------------------------------------------------------------------------
// Function: TagDisplay::setupNameEditor()
//-----------------------------------------------------------------------------
void TagDisplay::setupEditors(QWidget* nameEditor)
{
    int editorHeight = 19;
    nameEditor->setFixedHeight(editorHeight);
    deleteButton_->setFixedHeight(editorHeight);

    int smallButtonWidth = 20;
    deleteButton_->setFixedWidth(smallButtonWidth);
    deleteButton_->adjustSize();

    colorButton_ = new ColorBox(QSize(smallButtonWidth, editorHeight), this);

    colorButton_->setToolTip(QLatin1String("Select Color"));
    deleteButton_->setToolTip(QLatin1String("Delete Tag"));

    colorButton_->setColor(editedLabel_->palette().color(QPalette::Window));
    colorButton_->update();

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
    QHBoxLayout* editorLayout = new QHBoxLayout(this);
    editorLayout->setContentsMargins(1, 1, 1, 1);

    editorLayout->addWidget(nameEditor);
    editorLayout->addWidget(colorButton_);
    editorLayout->addWidget(deleteButton_);
}

//-----------------------------------------------------------------------------
// Function: TagDisplay::connectSignals()
//-----------------------------------------------------------------------------
void TagDisplay::connectSignals()
{
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
    QString newName = getNewName();
    if (!newName.isEmpty())
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
