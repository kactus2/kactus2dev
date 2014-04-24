//-----------------------------------------------------------------------------
// File: FileDependencyInfoWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰, Tommi Korhonen
// Date: 03.03.2013
//
// Description:
// Widget for showing information about one file dependency.
//-----------------------------------------------------------------------------

#include "FileDependencyInfoWidget.h"

#include <models/FileDependency.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::FileDependencyInfoWidget()
//-----------------------------------------------------------------------------
FileDependencyInfoWidget::FileDependencyInfoWidget(QWidget* parent) 
    : QGroupBox(tr("Dependency Information"), parent),
    descEdit_(this),
    directionCheck_(tr("Bidirectional"), this),
    lockedCheck_(tr("Locked"), this),
    directionButton_(tr("Reverse Direction"), this),
    dependency_(0)
{
    directionCheck_.setFixedWidth(150);
    descEdit_.setMaximumHeight(80);
    
    this->setTitle(tr("Dependency Information"));

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel(tr("Description:"), this));
    leftLayout->addWidget(&descEdit_, 1);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(new QLabel("", this));
    rightLayout->addWidget(&directionButton_);
    rightLayout->addWidget(&directionCheck_);
    rightLayout->addWidget(&lockedCheck_);
    rightLayout->addStretch(1);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addLayout(leftLayout, 1);
    layout->addLayout(rightLayout);

    connect(&directionCheck_, SIGNAL(stateChanged(int)), this, SLOT(directionCheckBoxChanged(int)));
    connect(&lockedCheck_, SIGNAL(stateChanged(int)), this, SLOT(lockedCheckChanged(int)));
    connect(&descEdit_, SIGNAL(textChanged()), this, SLOT(descEditTextChanged()));
    connect(&directionButton_, SIGNAL(clicked()), this, SLOT(directionReversed()));

    // Disabling the widgets.
    descEdit_.setEnabled(false);
    directionCheck_.setEnabled(false);
    lockedCheck_.setEnabled(false);
    directionButton_.setEnabled(false);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::~FileDependencyInfoWidget()
//-----------------------------------------------------------------------------
FileDependencyInfoWidget::~FileDependencyInfoWidget()
{

}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::setEditedDependency()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::setEditedDependency(FileDependency* dependency)
{
    dependency_ = dependency;

    disconnect(&directionCheck_, SIGNAL(stateChanged(int)), this, SLOT(directionCheckBoxChanged(int)));
    disconnect(&lockedCheck_, SIGNAL(stateChanged(int)), this, SLOT(lockedCheckChanged(int)));
    disconnect(&descEdit_, SIGNAL(textChanged()), this, SLOT(descEditTextChanged()));
    disconnect(&directionButton_, SIGNAL(clicked()), this, SLOT(directionReversed()));
    
    // Clearing the widgets.
    descEdit_.clear();
    directionCheck_.setChecked(false);
    lockedCheck_.setChecked(false);
    this->setTitle(tr("Dependency Information"));
    // Disabling the widgets.
    descEdit_.setEnabled(false);
    directionCheck_.setEnabled(false);
    lockedCheck_.setEnabled(false);
    directionButton_.setEnabled(false);
    
    if(dependency_ != 0)
    {
        // Enabling the widgets.
        descEdit_.setEnabled(true);
        
        // Enable editing for direction and locking only for manual dependencies.
        if (dependency_->isManual())
        {
            directionCheck_.setEnabled(true);
            lockedCheck_.setEnabled(true);
        }

        // Read locked state.
        lockedCheck_.setChecked(dependency_->isLocked());
        // Set previous text.
        descEdit_.setPlainText(dependency_->getDescription());
        // Set direction checkbox and direction button.
        if (dependency_->isBidirectional())
        {
            directionCheck_.setChecked(true);
            directionButton_.setEnabled(false);
        }
        else if (dependency_->isManual())
        {
            directionCheck_.setChecked(false);
            directionButton_.setEnabled(true);
        }

        if (dependency_->isManual())
        {
            lockEverything(lockedCheck_.isChecked());
        }

        // Set informative file label.
        updateFileLabel();
    }

    connect(&directionCheck_, SIGNAL(stateChanged(int)), this, SLOT(directionCheckBoxChanged(int)));
    connect(&lockedCheck_, SIGNAL(stateChanged(int)), this, SLOT(lockedCheckChanged(int)));
    connect(&descEdit_, SIGNAL(textChanged()), this, SLOT(descEditTextChanged()));
    connect(&directionButton_, SIGNAL(clicked()), this, SLOT(directionReversed()));
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::getEditedDependency()
//-----------------------------------------------------------------------------
FileDependency* FileDependencyInfoWidget::getEditedDependency() const
{
    return dependency_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::directionComboChanged()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::directionCheckBoxChanged(int /*state*/)
{
    if (!dependency_->isLocked())
    {
        dependency_->setBidirectional(directionCheck_.isChecked());
        emit dependencyChanged(dependency_);
        updateFileLabel();

        // If dependency is bidirectional disable "Reverse direction"-button.
        if(dependency_->isBidirectional())
        {
            directionButton_.setEnabled(false);
        }
        // Else enable it.
        else
        {
            directionButton_.setEnabled(true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::lockedCheckChanged()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::lockedCheckChanged(int /*state*/)
{
    dependency_->setLocked(lockedCheck_.isChecked());
    lockEverything(lockedCheck_.isChecked());
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::descEditTextChanged()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::descEditTextChanged()
{
    if (!dependency_->isLocked())
    {
        dependency_->setDescription(descEdit_.toPlainText());
        emit dependencyChanged(dependency_);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::directionReversed()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::directionReversed()
{
    if (!dependency_->isLocked())
    {
        dependency_->reverse();
        emit dependencyChanged(dependency_);
        updateFileLabel();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::updateFileLabel()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::updateFileLabel()
{
    QString fileLabelText = tr("Dependency Information: ");
    QFileInfo fromInfo(dependency_->getFile1());
    QFileInfo toInfo(dependency_->getFile2());
    fileLabelText += fromInfo.fileName();
    if(dependency_->isBidirectional())
    {
        fileLabelText += " <--> ";
    }
    else
    {
        fileLabelText += " ---> ";
    }
    fileLabelText += toInfo.fileName();
    this->setTitle(fileLabelText);
}

void FileDependencyInfoWidget::lockEverything(bool isLocked)
{
    if (isLocked)
    {
        descEdit_.setEnabled(false);
        directionCheck_.setEnabled(false);
        directionButton_.setEnabled(false);
    }
    else
    {
        descEdit_.setEnabled(true);
        directionCheck_.setEnabled(true);
        if (!directionCheck_.isChecked())
        {
            directionButton_.setEnabled(true);
        }
        else
        {
            directionButton_.setEnabled(false);
        }
    }
}