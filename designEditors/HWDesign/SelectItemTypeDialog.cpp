//-----------------------------------------------------------------------------
// File: SelectItemTypeDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.5.2011
//
// Description:
// Item type selection dialog.
//-----------------------------------------------------------------------------

#include "SelectItemTypeDialog.h"

#include <QGridLayout>

namespace
{
    QString const ITEM_NAMES[CIT_COUNT] =
    {
        "Interface",
        "Component",
        "Channel",
        "Bridge"
    };
}

//-----------------------------------------------------------------------------
// Function: SelectItemTypeDialog()
//-----------------------------------------------------------------------------
SelectItemTypeDialog::SelectItemTypeDialog(QWidget* parent,
                                           unsigned int allowedItems) : QDialog(parent),
                                                                        allowedItems_(allowedItems),
                                                                        layout_(0), allowedItemsGroup_(0)
{
    Q_ASSERT(allowedItems != CIT_NONE);

    setWindowTitle(tr("Add Item"));

    layout_ = new QGridLayout(this);

    // Create the radio button group and a layout for it.
    allowedItemsGroup_ = new QGroupBox(tr("Item Type"), this);
    QGridLayout* itemLayout = new QGridLayout(allowedItemsGroup_);
    
    // Create the item radio buttons.
    if (allowedItems & CIT_INTERFACE)
    {
        itemRadioButtons_[0] = new QRadioButton(ITEM_NAMES[CIT_COMPONENT], allowedItemsGroup_);
        itemRadioButtons_[0]->setChecked(true);
        itemLayout->addWidget(itemRadioButtons_[0], 0, 0, 1, 1);
    }

    QString text = "";

    if (allowedItems & CIT_COMPONENT)
    {
        text += ITEM_NAMES[CIT_COMPONENT];
    }
    
    if (allowedItems & CIT_CHANNEL)
    {
        text += ITEM_NAMES[CIT_CHANNEL];
    }

    if (allowedItems & CIT_BRIDGE)
    {
        text += ITEM_NAMES[CIT_BRIDGE];
    }

    text = text.left(text.length() - 1);

    if (!text.isEmpty())
    {
        itemRadioButtons_[1] = new QRadioButton(text, allowedItemsGroup_);
        itemLayout->addWidget(itemRadioButtons_[1], 1, 0, 1, 1);
    }

    layout_->addWidget(allowedItemsGroup_, 0, 0, 1, 2);

    // Create the OK and Cancel buttons.
    QPushButton* btnOK = new QPushButton(this);
    btnOK->setText(tr("&Add"));
    layout_->addWidget(btnOK, 1, 0, 1, 1);

    QPushButton* btnCancel = new QPushButton(this);
    btnCancel->setText(tr("&Cancel"));
    layout_->addWidget(btnCancel, 1, 1, 1, 1);

    // Connect the button signals to accept() and reject().
    QObject::connect(btnOK, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

//-----------------------------------------------------------------------------
// Function: ~SelectItemTypeDialog()
//-----------------------------------------------------------------------------
SelectItemTypeDialog::~SelectItemTypeDialog()
{

}

//-----------------------------------------------------------------------------
// Function: getSelectedItemType()
//-----------------------------------------------------------------------------
ColumnItemType SelectItemTypeDialog::getSelectedItemType() const
{
    for (unsigned int i = 0; i < CIT_COUNT; ++i)
    {
        if (itemRadioButtons_[i] != 0 && itemRadioButtons_[i]->isChecked())
        {
            return static_cast<ColumnItemType>(1 << i);
        }
    }

    // We should not get here at all.
    Q_ASSERT(false);
    return CIT_NONE;
}
