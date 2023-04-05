﻿//-----------------------------------------------------------------------------
// File: RenodeFileSelectionGroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.04.2023
//
// Description:
// Group for selecting files to be written in Renode generation.
//-----------------------------------------------------------------------------

#include "RenodeFileSelectionGroup.h"

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: RenodeFileSelectionGroup::RenodeFileSelectionGroup()
//-----------------------------------------------------------------------------
RenodeFileSelectionGroup::RenodeFileSelectionGroup(QWidget *parent):
QGroupBox(parent),
selectAll_(new QCheckBox("All")),
cpuCheck_(new QCheckBox("CPU")),
memoryCheck_(new QCheckBox("Memory")),
peripheralsCheck_(new QCheckBox("Peripherals")),
boxStorage_()
{
    setTitle("Select files to write");

    boxStorage_.append(cpuCheck_);
    boxStorage_.append(memoryCheck_);
    boxStorage_.append(peripheralsCheck_);

    selectAll_->setTristate(true);
    selectAll_->setChecked(true);

    connect(selectAll_, SIGNAL(clicked(bool)), this, SLOT(onCheckAllStateChange(bool)), Qt::UniqueConnection);

    QFrame* line = new QFrame();
    line->setLineWidth(1);
    line->setMidLineWidth(1);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* checkBoxLayout(new QVBoxLayout(this));
    checkBoxLayout->addWidget(selectAll_);
    checkBoxLayout->addWidget(line);

    for (auto box : boxStorage_)
    {
        connect(box, SIGNAL(clicked(bool)), this, SLOT(onItemClicked(bool)), Qt::UniqueConnection);

        box->setChecked(true);

        checkBoxLayout->addWidget(box);
    }
}

//-----------------------------------------------------------------------------
// Function: RenodeFileSelectionGroup::onCheckAllStateChange()
//-----------------------------------------------------------------------------
void RenodeFileSelectionGroup::onCheckAllStateChange(bool newState)
{
    if (newState == true)
    {
        selectAll_->setChecked(true);
        selectAll_->repaint();
    }

    cpuCheck_->setChecked(newState);
    memoryCheck_->setChecked(newState);
    peripheralsCheck_->setChecked(newState);
}

//-----------------------------------------------------------------------------
// Function: RenodeFileSelectionGroup::onItemClicked()
//-----------------------------------------------------------------------------
void RenodeFileSelectionGroup::onItemClicked(bool newState)
{
    Qt::CheckState checkAllState = Qt::Checked;
    if (newState == false)
    {
        checkAllState = Qt::Unchecked;
    }

    for (auto box : boxStorage_)
    {
        if (box->isChecked() != newState)
        {
            checkAllState = Qt::PartiallyChecked;
            break;
        }
    }

    selectAll_->setCheckState(checkAllState);
    selectAll_->repaint();
}

//-----------------------------------------------------------------------------
// Function: RenodeFileSelectionGroup::writeCpu()
//-----------------------------------------------------------------------------
bool RenodeFileSelectionGroup::writeCpu()
{
    return cpuCheck_->isChecked();
}

//-----------------------------------------------------------------------------
// Function: RenodeFileSelectionGroup::writeMemory()
//-----------------------------------------------------------------------------
bool RenodeFileSelectionGroup::writeMemory()
{
    return memoryCheck_->isChecked();
}

//-----------------------------------------------------------------------------
// Function: RenodeFileSelectionGroup::writePeripherals()
//-----------------------------------------------------------------------------
bool RenodeFileSelectionGroup::writePeripherals()
{
    return peripheralsCheck_->isChecked();
}
