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

#include <common/widgets/colorBox/ColorBox.h>
#include <common/widgets/tagEditor/TagLabel.h>
#include <KactusAPI/include/TagManager.h>
#include <common/widgets/tagEditor/TagCompleterModel.h>

#include <QColorDialog>
#include <QCompleter>
#include <QAction>

//-----------------------------------------------------------------------------
// Function: TagEditor::TagEditor()
//-----------------------------------------------------------------------------
TagEditor::TagEditor(TagLabel* tagLabel, QWidget* parent /* = 0 */):
TagDisplay(tagLabel, parent),
nameEdit_(new QLineEdit(this))
{
    setupEditors(nameEdit_);
}

//-----------------------------------------------------------------------------
// Function: TagEditor::connectSignals()
//-----------------------------------------------------------------------------
void TagEditor::connectSignals()
{
    connect(getColorButton(), SIGNAL(clicked()), this, SLOT(changeColor()), Qt::UniqueConnection);

    TagDisplay::connectSignals();
}

//-----------------------------------------------------------------------------
// Function: TagEditor::setupEditors()
//-----------------------------------------------------------------------------
void TagEditor::setupEditors(QWidget* nameEditor)
{
    TagDisplay::setupEditors(nameEditor);

    setFocusProxy(nameEdit_);
    nameEdit_->installEventFilter(this);

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
    nameEdit_->setText(getEditedLabel()->text());
}

//-----------------------------------------------------------------------------
// Function: TagEditor::changeColor()
//-----------------------------------------------------------------------------
void TagEditor::changeColor()
{
    ColorBox* colorButton = getColorButton();

    QColorDialog dialog(this);
    dialog.setCurrentColor(colorButton->getColor());
    if (dialog.exec() == QDialog::Accepted)
    {
        colorButton->setColor(dialog.currentColor());
        colorButton->update();
    }
}

//-----------------------------------------------------------------------------
// Function: TagEditor::getNewName()
//-----------------------------------------------------------------------------
QString TagEditor::getNewName() const
{
    return nameEdit_->text();
}
