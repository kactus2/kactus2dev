//-----------------------------------------------------------------------------
// File: BitSelectionDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 3.10.2013
//
// Description:
// Dialog for selecting bit indexes of physical port.
//-----------------------------------------------------------------------------
#include "BitSelectionDialog.h"

#include <QWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>


//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::BitSelectionDialog()
//-----------------------------------------------------------------------------
BitSelectionDialog::BitSelectionDialog(int portSize, int portMaxSize, QWidget *parent)
    : QDialog(parent),
    leftBox_(new QSpinBox(this)),
    rightBox_(new QSpinBox(this)),
    buttonBox_(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this)),
    portSize_(portSize),
    maxSize_(portMaxSize)
{
    setupLayout();

    leftBox_->setRange(0, portSize - 1);
    leftBox_->setValue(portMaxSize-1);
    rightBox_->setRange(0, portSize - 1);
    rightBox_->setValue(0);

    connect(buttonBox_->button(QDialogButtonBox::Ok), SIGNAL(clicked()), 
        this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox_->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), 
        this, SLOT(reject()), Qt::UniqueConnection);
    connect(leftBox_, SIGNAL(valueChanged(int)),
        this, SLOT(onLeftChanged()), Qt::UniqueConnection);
    connect(rightBox_, SIGNAL(valueChanged(int)),
        this, SLOT(onRightChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::BitSelectionDialog()
//-----------------------------------------------------------------------------
BitSelectionDialog::~BitSelectionDialog()
{
}

//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::getLeft()
//-----------------------------------------------------------------------------
int BitSelectionDialog::getLeft() const
{
    return leftBox_->value();
}

//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::getRight()
//-----------------------------------------------------------------------------
int BitSelectionDialog::getRight() const
{
    return rightBox_->value();
}

//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::onLeftChanged()
//-----------------------------------------------------------------------------
void BitSelectionDialog::onLeftChanged()
{
    if (rightBox_->value() > leftBox_->value())
    {
        rightBox_->setValue(leftBox_->value());
    }

    if (leftBox_->value() - rightBox_->value() + 1 > maxSize_)
    {
        buttonBox_->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else
    {
        buttonBox_->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::onRightChanged()
//-----------------------------------------------------------------------------
void BitSelectionDialog::onRightChanged()
{
    if (rightBox_->value() > leftBox_->value())
    {
        leftBox_->setValue(rightBox_->value());
    }

    if (leftBox_->value() - rightBox_->value() + 1 > maxSize_)
    {
        buttonBox_->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else
    {
        buttonBox_->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::setupLayout()
//-----------------------------------------------------------------------------
void BitSelectionDialog::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);

    QString labelText = "Physical port has size of " + QString::number(portSize_) + " bits, but only " + 
        QString::number(maxSize_) + " bit(s) can be connected to logical signal.\n" +
        "Select which physical bit(s) to connect:";
    QLabel* descLabel = new QLabel(labelText, this);
    descLabel->setWordWrap(true);
    topLayout->addWidget(descLabel);

    QHBoxLayout* indexLayout = new QHBoxLayout();
    QLabel* downLabel = new QLabel(tr("downto"), this);
    indexLayout->addWidget(leftBox_, 0 ,Qt::AlignLeft);
    indexLayout->addWidget(downLabel, 0 ,Qt::AlignLeft);
    indexLayout->addWidget(rightBox_, 0 ,Qt::AlignLeft);
    indexLayout->addStretch();
    topLayout->addLayout(indexLayout);
    
    topLayout->addWidget(buttonBox_);
}





