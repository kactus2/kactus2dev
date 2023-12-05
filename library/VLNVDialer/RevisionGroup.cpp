//-----------------------------------------------------------------------------
// File: RevisionGroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.12.2023
//
// Description:
// Contains the check boxes to set the revision options for VLNVDialer.
//-----------------------------------------------------------------------------

#include "RevisionGroup.h"

#include <KactusAPI/include/utils.h>

#include <common/widgets/tagEditor/FlowLayout.h>

//-----------------------------------------------------------------------------
// Function: RevisionGroup::RevisionGroup()
//-----------------------------------------------------------------------------
RevisionGroup::RevisionGroup(QWidget *parent):
FilterGroup(tr("IP-XACT version"), parent)
{
    setupButton(&std14Button_, tr("Valid"));
    setupButton(&std22Button_, tr("Errors"));

    std14Button_.setChecked(true);
    std22Button_.setChecked(true);

    connect(&std14Button_, SIGNAL(clicked(bool)), this, SLOT(onStd14Changed(bool)), Qt::UniqueConnection);
    connect(&std22Button_, SIGNAL(clicked(bool)), this, SLOT(onStd22Changed(bool)), Qt::UniqueConnection);

    auto layout = new FlowLayout(this, 2, 1, 1);
    layout->addWidget(&std14Button_);
    layout->addWidget(&std22Button_);
}

//-----------------------------------------------------------------------------
// Function: RevisionGroup::setRevision()
//-----------------------------------------------------------------------------
void RevisionGroup::setRevision(Utils::RevisionOptions options)
{    
    std14Button_.setChecked(options.std14_);
    std22Button_.setChecked(options.std22_);

    options_ = options;
    emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: RevisionGroup::getRevision()
//-----------------------------------------------------------------------------
Utils::RevisionOptions RevisionGroup::getRevision() const
{
    Utils::RevisionOptions options;
    options.std14_ = std14Button_.isChecked();
    options.std22_ = std22Button_.isChecked();

    return options;
}

//-----------------------------------------------------------------------------
// Function: RevisionGroup::selectAll()
//-----------------------------------------------------------------------------
void RevisionGroup::selectAll(bool select)
{
    Utils::RevisionOptions options;
    options.std14_ = select;
    options.std22_ = select;

    setRevision(options);
}

//-----------------------------------------------------------------------------
// Function: RevisionGroup::onStd14Changed()
//-----------------------------------------------------------------------------
void RevisionGroup::onStd14Changed(bool checked)
{
	options_.std14_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: RevisionGroup::onStd22Changed()
//-----------------------------------------------------------------------------
void RevisionGroup::onStd22Changed(bool checked)
{
	options_.std22_ = checked;
	emit optionsChanged(options_);
}
