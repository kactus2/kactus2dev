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
items_(),
exclusiveItems_()
{
    layout_->setContentsMargins(0, 0, 0, 0);

    setAutoFillBackground(true);
}

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::addItem()
//-----------------------------------------------------------------------------
void EnumCollectionEditor::addItem(QString const& text, bool isExclusivePort, bool selected)
{
    QCheckBox* checkBox = createCheckBox(text, selected);

    if (isExclusivePort)
    {
        exclusiveItems_.append(checkBox);
        connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(onExclusiveItemClicked(bool)), Qt::UniqueConnection);
    }
    else
    {
        connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(onItemClicked(bool)), Qt::UniqueConnection);
    }
}

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::clearItems()
//-----------------------------------------------------------------------------
void EnumCollectionEditor::clearItems()
{
    for (auto item : items_)
    {
        layout_->removeWidget(item);
        item->deleteLater();
    }
    items_.clear();

    for (auto item : exclusiveItems_)
    {
        layout_->removeWidget(item);
        item->deleteLater();
    }
    exclusiveItems_.clear();
}

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::createCheckBox()
//-----------------------------------------------------------------------------
QCheckBox* EnumCollectionEditor::createCheckBox(QString const& text, bool isSelected)
{
    QCheckBox* checkBox = new QCheckBox(text, this);
    checkBox->setChecked(isSelected);

    items_.append(checkBox);
    layout_->addWidget(checkBox);

    setMinimumHeight(sizeHint().height());

    return checkBox;
}

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::onExclusiveItemClicked()
//-----------------------------------------------------------------------------
void EnumCollectionEditor::onExclusiveItemClicked(bool newState)
{
    if (newState == true)
    {
        QCheckBox* senderBox;
        QObject* senderObject = sender();
        if (senderObject)
        {
            senderBox = dynamic_cast<QCheckBox*>(senderObject);
        }

        for (auto item : items_)
        {
            if (item != senderBox)
            {
                item->setChecked(false);
            }
        }
    }

    handleCheckAllStateChangeFromItemClick(newState);
}

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::onItemClicked()
//-----------------------------------------------------------------------------
void EnumCollectionEditor::onItemClicked(bool newState)
{
    for (auto item : exclusiveItems_)
    {
        if (item->isChecked())
        {
            item->setChecked(false);
        }
    }

    handleCheckAllStateChangeFromItemClick(newState);
}

//-----------------------------------------------------------------------------
// Function: EnumCollectionEditor::handleCheckAllStateChangeFromItemClick()
//-----------------------------------------------------------------------------
void EnumCollectionEditor::handleCheckAllStateChangeFromItemClick(bool newState)
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

    for (QCheckBox* checkBox : items_)
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
