//-----------------------------------------------------------------------------
// File: TagEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.08.2019
//
// Description:
// Editor for tags.
//-----------------------------------------------------------------------------

#include "TagEditor.h"

#include <IPXACTmodels/common/TagData.h>

#include <common/widgets/colorBox/ColorBox.h>
#include <common/widgets/tagEditor/TagLabel.h>
#include <common/widgets/tagEditor/TagManager.h>
#include <common/widgets/tagEditor/TagCompleterModel.h>

#include <QHBoxLayout>
#include <QColorDialog>
#include <QApplication>
#include <QFocusEvent>
#include <QCompleter>
#include <QAction>

//-----------------------------------------------------------------------------
// Function: TagEditor::TagEditor()
//-----------------------------------------------------------------------------
TagEditor::TagEditor(TagLabel* tagLabel, QWidget* parent /* = 0 */):
QFrame(parent),
nameEdit_(new QLineEdit(this)),
colorButton_(),
deleteButton_(new QPushButton(QIcon(":/icons/common/graphics/cross.png"), QLatin1String(""), this)),
editedLabel_(tagLabel)
{
    int editorHeight = 19;
    nameEdit_->setFixedHeight(editorHeight);
    deleteButton_->setFixedHeight(editorHeight);

    setupNameEditor();

    int smallButtonWidth = 20;
    deleteButton_->setFixedWidth(smallButtonWidth);
    deleteButton_->adjustSize();

    colorButton_ = new ColorBox(QSize(smallButtonWidth, editorHeight), this);

    colorButton_->setToolTip(QLatin1String("Select Color"));
    deleteButton_->setToolTip(QLatin1String("Delete Tag"));

    colorButton_->setColor(tagLabel->palette().color(QPalette::Window));
    colorButton_->update();

    setupLayout();
    connectSignals();

    setFrameStyle(QFrame::Panel);
    setLineWidth(1);

    setFocusProxy(nameEdit_);

    nameEdit_->installEventFilter(this);
}

//-----------------------------------------------------------------------------
// Function: TagEditor::setupLayout()
//-----------------------------------------------------------------------------
void TagEditor::setupLayout()
{
    QHBoxLayout* editorLayout = new QHBoxLayout(this);
    editorLayout->setContentsMargins(1, 1, 1, 1);

    editorLayout->addWidget(nameEdit_);
    editorLayout->addWidget(colorButton_);
    editorLayout->addWidget(deleteButton_);
}

//-----------------------------------------------------------------------------
// Function: TagEditor::connectSignals()
//-----------------------------------------------------------------------------
void TagEditor::connectSignals()
{
    connect(colorButton_, SIGNAL(clicked()), this, SLOT(changeColor()), Qt::UniqueConnection);
    connect(deleteButton_, SIGNAL(clicked()), this, SLOT(onDeleteItem()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: TagEditor::setupNameEditor()
//-----------------------------------------------------------------------------
void TagEditor::setupNameEditor()
{
    QCompleter* completer = new QCompleter(this);

    TagCompleterModel* completerModel = new TagCompleterModel(TagManager::getInstance().getTags(), completer);
    completer->setModel(completerModel);

    completer->setCaseSensitivity(Qt::CaseInsensitive);

    QAction* action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space));
    action->setShortcutContext(Qt::WidgetShortcut);

    connect(action, SIGNAL(triggered()), completer, SLOT(complete()), Qt::UniqueConnection);
    connect(completer, SIGNAL(activated(QModelIndex const&)),
        completerModel, SLOT(itemSelected(QModelIndex const&)), Qt::UniqueConnection);
    connect(completerModel, SIGNAL(selectedColor(QColor const&)),
        this, SLOT(completerColorChange(QColor const&)), Qt::UniqueConnection);

    nameEdit_->addAction(action);
    nameEdit_->setCompleter(completer);
    nameEdit_->setFixedWidth(80);
    nameEdit_->setText(editedLabel_->text());
}

//-----------------------------------------------------------------------------
// Function: TagEditor::completerColorChange()
//-----------------------------------------------------------------------------
void TagEditor::completerColorChange(QColor const& newColor)
{
    colorButton_->setColor(newColor);
    colorButton_->update();
}

//-----------------------------------------------------------------------------
// Function: TagEditor::changeColor()
//-----------------------------------------------------------------------------
void TagEditor::changeColor()
{
    QColorDialog dialog(this);
    dialog.setCurrentColor(colorButton_->getColor());
    if (dialog.exec() == QDialog::Accepted)
    {
        colorButton_->setColor(dialog.currentColor());
        colorButton_->update();
    }
}

//-----------------------------------------------------------------------------
// Function: TagEditor::onAcceptChanges()
//-----------------------------------------------------------------------------
void TagEditor::onAcceptChanges()
{
    QString newName = nameEdit_->text();
    if (!newName.isEmpty())
    {
        editedLabel_->setText(newName);
    }

    editedLabel_->setPalette(colorButton_->palette());

    emit acceptChanges(editedLabel_, this);
}

//-----------------------------------------------------------------------------
// Function: TagEditor::onDeleteItem()
//-----------------------------------------------------------------------------
void TagEditor::onDeleteItem()
{
    emit deleteItem(editedLabel_, this);
}

//-----------------------------------------------------------------------------
// Function: TagEditor::focusOutEvent()
//-----------------------------------------------------------------------------
void TagEditor::focusOutEvent(QFocusEvent* /*event*/)
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
// Function: TagEditor::eventFilter()
//-----------------------------------------------------------------------------
bool TagEditor::eventFilter(QObject *watched, QEvent *event)
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
// Function: TagEditor::keyPressEvent()
//-----------------------------------------------------------------------------
void TagEditor::keyPressEvent(QKeyEvent *event)
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
