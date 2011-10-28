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
#include <QMessageBox>
#include <QCoreApplication>

namespace
{
    QString const MODE_NAMES[] =
    {
        "Master",
        "Slave",
        "Mirrored Master",
        "Mirrored Slave",
        "System",
        "Mirrored System",
        "Monitor"
    }; 
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceDialog()
//-----------------------------------------------------------------------------
BusInterfaceDialog::BusInterfaceDialog(bool enableNameEdit,
                                       QWidget* parent) : QDialog(parent), nameEdit_(0),
                                                          modeGroup_(0), btnOK_(0)
{
    setWindowTitle(tr("Define Bus Interface"));

    layout_ = new QGridLayout(this);

    // Check if name editing is requested.
    if (enableNameEdit)
    {
        // Create the name edit and its label.
        QLabel* nameLabel = new QLabel(tr("Name:"), this);
        nameEdit_ = new QLineEdit(this);

        layout_->addWidget(nameLabel, 0, 0, 1, 2);
        layout_->addWidget(nameEdit_, 1, 0, 1, 2);
    }

    // Create the radio button group and a layout for it.
    modeGroup_ = new QGroupBox(tr("Interface Mode"), this);
    QGridLayout* itemLayout = new QGridLayout(modeGroup_);

    for (unsigned int i = 0; i < General::MONITOR + 1; ++i)
    {
        modeRadioButtons_[i] = new QRadioButton(MODE_NAMES[i], modeGroup_);
        modeRadioButtons_[i]->setVisible(false);
        itemLayout->addWidget(modeRadioButtons_[i], i, 0, 1, 1);
    }

    layout_->addWidget(modeGroup_, 2, 0, 1, 2);

    // Create the OK and Cancel buttons.
    btnOK_ = new QPushButton(this);
    btnOK_->setText(tr("&OK"));

    if (enableNameEdit)
    {
        btnOK_->setEnabled(false);
    }

    layout_->addWidget(btnOK_, 3, 0, 1, 1);

    QPushButton* btnCancel = new QPushButton(this);
    btnCancel->setText(tr("&Cancel"));
    layout_->addWidget(btnCancel, 3, 1, 1, 1);

    // Connect the button signals to accept() and reject().
    connect(btnOK_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

    if (nameEdit_ != 0)
    {
        connect(nameEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(onNameChanged()));
    }
}

//-----------------------------------------------------------------------------
// Function: ~BusInterfaceDialog()
//-----------------------------------------------------------------------------
BusInterfaceDialog::~BusInterfaceDialog()
{
}

//-----------------------------------------------------------------------------
// Function: addMode()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::addMode(General::InterfaceMode mode)
{
    modeRadioButtons_[mode]->setVisible(true);
    modeRadioButtons_[mode]->setChecked(true);
}

//-----------------------------------------------------------------------------
// Function: getSelectedMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusInterfaceDialog::getSelectedMode() const
{
    for (unsigned int i = 0; i < General::MONITOR + 1; ++i)
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
// Function: setName()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::setName(QString const& name)
{
    Q_ASSERT(nameEdit_ != 0);
    nameEdit_->setText(name);
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString BusInterfaceDialog::getName() const
{
    Q_ASSERT(nameEdit_ != 0);
    return nameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: onNameChanged()
//-----------------------------------------------------------------------------
void BusInterfaceDialog::onNameChanged()
{
    btnOK_->setEnabled(!nameEdit_->text().isEmpty());
}
