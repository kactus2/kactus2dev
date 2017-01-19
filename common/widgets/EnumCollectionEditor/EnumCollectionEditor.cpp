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
EnumCollectionEditor::EnumCollectionEditor(QWidget* parent):
QFrame(parent),
      layout_(new QVBoxLayout(this)),
      items_()
{
    setFrameStyle(QFrame::StyledPanel);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_NoMousePropagation);
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
void EnumCollectionEditor::addItem(QString const& text, bool selected)
{
    QCheckBox* checkBox = new QCheckBox(text, this);
    checkBox->setChecked(selected);
    
    items_.append(checkBox);
    layout_->addWidget(checkBox);

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

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::moveEvent()
//-----------------------------------------------------------------------------
void EnumCollectionEditor::moveEvent(QMoveEvent* event)
{
    rePosition();

    QFrame::moveEvent(event);
}

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::rePosition()
//-----------------------------------------------------------------------------
void EnumCollectionEditor::rePosition()
{
    int availableHeight = parentWidget()->height() - parentWidget()->contentsMargins().bottom() -
        parentWidget()->contentsMargins().top();

    int yLeftBottom = pos().y() + height();
    if (yLeftBottom > availableHeight)
    {
        int yChange = yLeftBottom - parentWidget()->height() + parentWidget()->contentsMargins().bottom();
        int yCoordinate = pos().y() - yChange;
        if (yCoordinate >= 0)
        {
            move(pos().x(), yCoordinate);
        }
    }
}
