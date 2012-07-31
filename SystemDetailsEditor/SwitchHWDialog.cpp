//-----------------------------------------------------------------------------
// File: SwitchHWDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.7.2012
//
// Description:
// Dialog for configuring settings how to switch HW for a system design.
//-----------------------------------------------------------------------------

#include "SwitchHWDialog.h"

#include <QDialogButtonBox>

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::SwitchHWDialog()
//-----------------------------------------------------------------------------
SwitchHWDialog::SwitchHWDialog(LibraryInterface* lh, QWidget* parent)
    : QDialog(parent),
      infoLabel_(tr("System design needs to be either moved or copied under the selected HW. Choose how to proceed."), this),
      viewNameLabel_(tr("System view name:"), this),
      viewNameEdit_(this),
      actionGroupBox_(tr("Action"), this),
      actionGroup_(this),
      moveRadioButton_(tr("Move existing design\nRemoves the system view completely from the previously mapped HW and moves it to the new one."), this),
      copyRadioButton_(tr("Copy as a new design\nCreates an identical copy of the system design with a new VLNV."), this),
      designVlnvEdit_(VLNV::DESIGN, lh, this, this),
      layout_(this)
{
    setWindowTitle(tr("Switch HW"));

    // Set widget settings.
    designVlnvEdit_.setTitle(tr("Design VLNV"));
    designVlnvEdit_.setVisible(false);
    moveRadioButton_.setChecked(true);

    actionGroup_.addButton(&moveRadioButton_);
    actionGroup_.addButton(&copyRadioButton_);

    // Create layouts.
    QVBoxLayout* groupLayout = new QVBoxLayout(&actionGroupBox_);
    groupLayout->addWidget(&moveRadioButton_);
    groupLayout->addWidget(&copyRadioButton_);

    layout_.addWidget(&infoLabel_);
    layout_.addSpacing(12);
    layout_.addWidget(&viewNameLabel_);
    layout_.addWidget(&viewNameEdit_);
    layout_.addWidget(&actionGroupBox_);
    layout_.addWidget(&designVlnvEdit_);
    layout_.addStretch(1);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this);
    layout_.addWidget(buttonBox);

    // Setup connections.
    connect(&actionGroup_, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(actionChanged(QAbstractButton*)), Qt::UniqueConnection);
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()), Qt::UniqueConnection);

    setFixedHeight(sizeHint().height());
}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::~SwitchHWDialog()
//-----------------------------------------------------------------------------
SwitchHWDialog::~SwitchHWDialog()
{

}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::accept()
//-----------------------------------------------------------------------------
void SwitchHWDialog::accept()
{
    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: SwitchHWDialog::actionChanged()
//-----------------------------------------------------------------------------
void SwitchHWDialog::actionChanged(QAbstractButton* button)
{
    designVlnvEdit_.setVisible(button == &copyRadioButton_);
    layout_.activate();
    setFixedHeight(sizeHint().height());
}
