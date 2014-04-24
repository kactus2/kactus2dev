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
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::BitSelectionDialog()
//-----------------------------------------------------------------------------
BitSelectionDialog::BitSelectionDialog(QString logicalPort, int logicalBeginIndex, QString physicalPort, 
    int portSize, int portMaxSize, QWidget *parent)
    : QDialog(parent),
    logicalNameLabel_(new QLabel(logicalPort, this)),
    physicalNameLabel_(new QLabel(physicalPort, this)),
    higherLogicalBox_(new QSpinBox(this)),
    lowerLogicalBox_(new QSpinBox(this)),
    higherPhysicalBox_(new QSpinBox(this)),
    lowerPhysicalBox_(new QSpinBox(this)),
    buttonBox_(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this)),
    physPortSize_(portSize),
    maxPhysPortSize_(portMaxSize)
{
    setupLayout();

    // Set initial values for physical bits and set ranges.
    higherPhysicalBox_->setRange(0, portSize - 1);
    higherPhysicalBox_->setValue(portMaxSize - 1);
    lowerPhysicalBox_->setRange(0, portSize - 1);
    lowerPhysicalBox_->setValue(0);

    // Set initial values for logical bits and disable editing.
    higherLogicalBox_->setValue(logicalBeginIndex + portMaxSize - 1);
    higherLogicalBox_->setEnabled(false);
    lowerLogicalBox_->setValue(logicalBeginIndex);
    lowerLogicalBox_->setEnabled(false);

    connect(buttonBox_->button(QDialogButtonBox::Ok), SIGNAL(clicked()), 
        this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox_->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), 
        this, SLOT(reject()), Qt::UniqueConnection);
    connect(higherPhysicalBox_, SIGNAL(valueChanged(int)),
        this, SLOT(onHigherBoundChanged()), Qt::UniqueConnection);
    connect(lowerPhysicalBox_, SIGNAL(valueChanged(int)),
        this, SLOT(onLowerBoundChanged()), Qt::UniqueConnection);
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
int BitSelectionDialog::getHigherBound() const
{
    return higherPhysicalBox_->value();
}

//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::getRight()
//-----------------------------------------------------------------------------
int BitSelectionDialog::getLowerBound() const
{
    return lowerPhysicalBox_->value();
}

//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::onLeftChanged()
//-----------------------------------------------------------------------------
void BitSelectionDialog::onHigherBoundChanged()
{
    if (lowerPhysicalBox_->value() > higherPhysicalBox_->value())
    {
        lowerPhysicalBox_->setValue(higherPhysicalBox_->value());
    }

    updateLogicalBits();
}

//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::onRightChanged()
//-----------------------------------------------------------------------------
void BitSelectionDialog::onLowerBoundChanged()
{
    if (lowerPhysicalBox_->value() > higherPhysicalBox_->value())
    {
        higherPhysicalBox_->setValue(lowerPhysicalBox_->value());
    }

    updateLogicalBits();
}

//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::setupLayout()
//-----------------------------------------------------------------------------
void BitSelectionDialog::setupLayout()
{
    const int SPINBOX_MIN_WIDTH = 45;
    higherLogicalBox_->setMinimumWidth(SPINBOX_MIN_WIDTH);
    lowerLogicalBox_->setMinimumWidth(SPINBOX_MIN_WIDTH);
    higherPhysicalBox_->setMinimumWidth(SPINBOX_MIN_WIDTH);
    lowerPhysicalBox_->setMinimumWidth(SPINBOX_MIN_WIDTH);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    
    QGroupBox* group = new QGroupBox(tr("Select bit(s) to connect"), this);

    QGridLayout* selectionLayout = new QGridLayout(group);
    selectionLayout->addWidget(logicalNameLabel_, 0, 1, 1, 1);
    selectionLayout->addWidget(physicalNameLabel_, 0, 2, 1, 1);

    selectionLayout->addWidget(new QLabel(tr("Left (higher) bound")), 1, 0, 1, 1);    
    selectionLayout->addWidget(higherLogicalBox_, 1, 1, 1, 1);
    QHBoxLayout* higherPhysicalLayout = new QHBoxLayout();
    higherPhysicalLayout->addWidget(higherPhysicalBox_);
    higherPhysicalLayout->addStretch();
    selectionLayout->addLayout(higherPhysicalLayout, 1, 2, 1, 1);

    selectionLayout->addWidget(new QLabel(tr("Right (lower) bound")), 2, 0, 1, 1, Qt::AlignTop | Qt::AlignLeft);    
    selectionLayout->addWidget(lowerLogicalBox_, 2, 1, 1, 1);
    QHBoxLayout* lowerPhysicalLayout = new QHBoxLayout();
    lowerPhysicalLayout->addWidget(lowerPhysicalBox_, 0, Qt::AlignTop | Qt::AlignLeft);
    lowerPhysicalLayout->addStretch();
    selectionLayout->addLayout(lowerPhysicalLayout, 2, 2, 1, 1, Qt::AlignTop | Qt::AlignLeft);

    //selectionLayout->setColumnStretch(3, 1);
    //selectionLayout->setRowStretch(2, 1);
    
    topLayout->addWidget(group);
    topLayout->addWidget(buttonBox_);
}

//-----------------------------------------------------------------------------
// Function: BitSelectionDialog::updateLogicalBits()
//-----------------------------------------------------------------------------
void BitSelectionDialog::updateLogicalBits()
{
    int physSize = higherPhysicalBox_->value() - lowerPhysicalBox_->value() + 1;
    int lowerLogical = lowerLogicalBox_->value();

    if (physSize > maxPhysPortSize_)
    {
        buttonBox_->button(QDialogButtonBox::Ok)->setEnabled(false);
        higherLogicalBox_->setValue(lowerLogical + maxPhysPortSize_ - 1);       
    }
    else
    {
        buttonBox_->button(QDialogButtonBox::Ok)->setEnabled(true);
        higherLogicalBox_->setValue(lowerLogical + physSize - 1);
    }
}

