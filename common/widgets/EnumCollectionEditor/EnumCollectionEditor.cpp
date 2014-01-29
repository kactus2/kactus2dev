//-----------------------------------------------------------------------------
// File: EnumCollectionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.04.2013
//
// Description:
// Editor for choosing multiple values from a collection of enumerables.
//-----------------------------------------------------------------------------

#include "EnumCollectionEditor.h"

#include <QEvent>

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::EnumCollectionEditor()
//-----------------------------------------------------------------------------
EnumCollectionEditor::EnumCollectionEditor(QWidget* parent)
    : QFrame(parent),
      layout_(new QVBoxLayout(this))
{
    setFrameStyle(QFrame::StyledPanel);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);
    layout_->addStretch(1);
}

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::~EnumCollectionEditor()
//-----------------------------------------------------------------------------
EnumCollectionEditor::~EnumCollectionEditor()
{
}

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::addItem()
//-----------------------------------------------------------------------------
void EnumCollectionEditor::addItem(QString const& text, bool selected /*= false*/)
{
    QCheckBox* checkBox = new QCheckBox(text, this);
    checkBox->setChecked(selected);
    checkBox->setFocusProxy(this);

    items_.append(checkBox);
    layout_->insertWidget(layout_->count() - 1, checkBox);

    setMinimumHeight(sizeHint().height());
}

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::getSelectedItems()
//-----------------------------------------------------------------------------
QStringList EnumCollectionEditor::getSelectedItems() const
{
    QStringList selectedItems;

    foreach (QCheckBox* checkBox, items_)
    {
        if (checkBox->isChecked())
        {
            selectedItems.append(checkBox->text());
        }
    }

    return selectedItems;
}
