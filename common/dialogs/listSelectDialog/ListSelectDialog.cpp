//-----------------------------------------------------------------------------
// File: ListSelectDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.12.2011
//
// Description:
// Dialog for making a selection from a list of strings.
//-----------------------------------------------------------------------------

#include "ListSelectDialog.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ListSelectDialog()
//-----------------------------------------------------------------------------
ListSelectDialog::ListSelectDialog(QWidget* parent) : QDialog(parent), 
                                                      descLabel_(tr("Select item:"), this),
                                                      itemList_(this)
{
    // Create the layout for the dialog.
    QPushButton* btnOK = new QPushButton(tr("OK") , this);
    QPushButton* btnCancel = new QPushButton(tr("Cancel"), this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnOK);
    buttonLayout->addWidget(btnCancel);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(&descLabel_);
    mainLayout->addWidget(&itemList_, 1);
    mainLayout->addLayout(buttonLayout);

    connect(btnOK, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(&itemList_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));
}

//-----------------------------------------------------------------------------
// Function: ~ListSelectDialog()
//-----------------------------------------------------------------------------
ListSelectDialog::~ListSelectDialog()
{
}

//-----------------------------------------------------------------------------
// Function: setDescription()
//-----------------------------------------------------------------------------
void ListSelectDialog::setDescription(QString const& desc)
{
    descLabel_.setText(desc);
}

//-----------------------------------------------------------------------------
// Function: addItem()
//-----------------------------------------------------------------------------
void ListSelectDialog::addItem(QListWidgetItem* item)
{
    itemList_.addItem(item);
}

//-----------------------------------------------------------------------------
// Function: getSelectedItem()
//-----------------------------------------------------------------------------
QListWidgetItem* ListSelectDialog::getSelectedItem() const
{
    return itemList_.currentItem();
}
