//-----------------------------------------------------------------------------
// File: EnumCollectionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
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
QWidget(parent),
layout_(new QVBoxLayout(this)),
items_()
{
    layout_->setContentsMargins(0, 0, 0, 0);

    setAutoFillBackground(true);
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

    connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(onItemClicked(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::onItemClicked()
//-----------------------------------------------------------------------------
void EnumCollectionEditor::onItemClicked(bool newState)
{
    Qt::CheckState checkAllState = Qt::Checked;
    if (newState == false)
    {
        checkAllState = Qt::Unchecked;
    }

    for (auto item : items_)
    {
        if (item->isChecked() != newState)
        {
            checkAllState = Qt::PartiallyChecked;
            break;
        }
    }

    emit itemStateChanged(checkAllState);
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

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::changeStatesOfAllItems()
//-----------------------------------------------------------------------------
void EnumCollectionEditor::changeStatesOfAllItems(bool newState)
{
    for (auto item : items_)
    {
        item->setChecked(newState);
    }
}
