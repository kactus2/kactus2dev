//-----------------------------------------------------------------------------
// File: BusInterfaceDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.5.2011
//
// Description:
// Interface mode selection dialog.
//-----------------------------------------------------------------------------

#include "BusInterfaceDialog.h"

#include <QLabel>

#include <library/LibraryManager/libraryinterface.h>

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::BusInterfaceDialog()
//-----------------------------------------------------------------------------
BusInterfaceDialog::BusInterfaceDialog(QWidget* parent) 
    : QDialog(parent),
      modeGroup_(new QGroupBox(tr("Select Interface Mode"), this)),
      btnOK_(new QPushButton(tr("&OK"),this)),
      btnCancel_(new QPushButton(tr("&Cancel"),this))
{
    setWindowTitle(tr("Define Bus Interface"));

    // Create the radio button group.
    for (unsigned int i = 0; i < General::INTERFACE_MODE_COUNT; i++)
    {
        QString modeName = General::INTERFACE_MODE_NAMES[i];
        modeName.replace(0, 1, modeName.at(0).toUpper());

        modeRadioButtons_[i] = new QRadioButton(modeName, modeGroup_);
        modeRadioButtons_[i]->setVisible(false);       
    }

    // Connect the button signals to accept() and reject().
    connect(btnOK_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel_, SIGNAL(clicked()), this, SLOT(reject()));    

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::~BusInterfaceDialog()
//-----------------------------------------------------------------------------
BusInterfaceDialog::~BusInterfaceDialog()
{
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::addMode()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::addMode(General::InterfaceMode mode)
{
    modeRadioButtons_[mode]->setVisible(true);
    modeRadioButtons_[mode]->setChecked(true);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::getSelectedMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusInterfaceDialog::getSelectedMode() const
{
    for (unsigned int i = 0; i < General::INTERFACE_MODE_COUNT; ++i)
    {
        if (modeRadioButtons_[i]->isChecked())
        {
            return static_cast<General::InterfaceMode>(i);
        }
    }

    // Should not be possible to get here.
    Q_ASSERT(false);
    return General::MASTER;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog::setupLayout()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);

    // Create vertical layout for radio buttons.
    QGridLayout* itemLayout = new QGridLayout(modeGroup_);
    for (unsigned int i = 0; i < General::INTERFACE_MODE_COUNT; i++)
    {
        itemLayout->addWidget(modeRadioButtons_[i], i, 0, 1, 1);
    }

    topLayout->addWidget(modeGroup_);
  
    // Create layout for OK and Cancel buttons.
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnOK_, 0, Qt::AlignRight);
    buttonLayout->addWidget(btnCancel_, 0, Qt::AlignRight);

    topLayout->addLayout(buttonLayout);
}
