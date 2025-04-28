//-----------------------------------------------------------------------------
// File: FileDependencyInfoWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta, Tommi Korhonen
// Date: 03.03.2013
//
// Description:
// Widget for showing information about one file dependency.
//-----------------------------------------------------------------------------

#include "FileDependencyInfoWidget.h"

#include <IPXACTmodels/kactusExtensions/FileDependency.h>

#include <QGridLayout>
#include <QLabel>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::FileDependencyInfoWidget()
//-----------------------------------------------------------------------------
FileDependencyInfoWidget::FileDependencyInfoWidget(QWidget* parent):
QGroupBox(tr("Dependency information"), parent),
    descEdit_(this),
    bidirectionalCheck_(tr("Bidirectional"), this),
    lockedCheck_(tr("Locked"), this),
    directionButton_(QIcon(":icons/common/graphics/imported.png"), tr("Reverse direction"), this),
    dependency_(0)
{
    setFlat(true);
    setupLayout();

    connect(&bidirectionalCheck_, SIGNAL(stateChanged(int)), this, SLOT(directionChanged(int)));
    connect(&lockedCheck_, SIGNAL(stateChanged(int)), this, SLOT(lockedCheckChanged(int)));
    connect(&descEdit_, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
    connect(&directionButton_, SIGNAL(clicked()), this, SLOT(directionReversed()));

    // Disabling the widgets.
    descEdit_.setEnabled(false);
    bidirectionalCheck_.setEnabled(false);
    lockedCheck_.setEnabled(false);
    directionButton_.setEnabled(false);
    directionButton_.setFlat(true);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::setEditedDependency()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::setEditedDependency(FileDependency* dependency)
{
    dependency_ = dependency;

    disconnect(&bidirectionalCheck_, SIGNAL(stateChanged(int)), this, SLOT(directionChanged(int)));
    disconnect(&lockedCheck_, SIGNAL(stateChanged(int)), this, SLOT(lockedCheckChanged(int)));
    disconnect(&descEdit_, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
    disconnect(&directionButton_, SIGNAL(clicked()), this, SLOT(directionReversed()));

    setTitle(tr("Dependency information"));

    descEdit_.setEnabled(dependency_ != 0);
    descEdit_.clear();
    if (dependency_ != 0)
    {
        descEdit_.setPlainText(dependency_->getDescription());
    }

    bidirectionalCheck_.setEnabled(dependency_ != 0 && dependency_->isManual());
    bidirectionalCheck_.setChecked(dependency_ != 0 && dependency_->isBidirectional());

    lockedCheck_.setEnabled(dependency_ != 0 && dependency_->isManual() && !dependency_->isBidirectional());
    lockedCheck_.setChecked(dependency_ != 0 && dependency_->isLocked());

    directionButton_.setEnabled(dependency_ != 0 && dependency_->isManual());
    
    if (dependency_ != 0)
    {
        if (dependency_->isManual())
        {
            lockEverything(lockedCheck_.isChecked());
        }

        updateFileLabel();
    }

    connect(&bidirectionalCheck_, SIGNAL(stateChanged(int)), this, SLOT(directionChanged(int)));
    connect(&lockedCheck_, SIGNAL(stateChanged(int)), this, SLOT(lockedCheckChanged(int)));
    connect(&descEdit_, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
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
// Function: FileDependencyInfoWidget::directionChanged()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::directionChanged(int /*state*/)
{
    if (!dependency_->isLocked())
    {
        dependency_->setBidirectional(bidirectionalCheck_.isChecked());
        emit dependencyChanged(dependency_);
        updateFileLabel();

        directionButton_.setEnabled(!dependency_->isBidirectional());
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::lockedCheckChanged()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::lockedCheckChanged(int /*state*/)
{
    dependency_->setLocked(lockedCheck_.isChecked());
    lockEverything(lockedCheck_.isChecked());
    emit dependencyChanged(dependency_);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::descriptionChanged()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::descriptionChanged()
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
    QString fileLabelText = tr("Dependency information: ");

    QFileInfo fromInfo(dependency_->getFile1());
    fileLabelText += fromInfo.fileName();

    if (dependency_->isBidirectional())
    {
        fileLabelText.append(QLatin1String(" <--> "));
    }
    else
    {
        fileLabelText.append(QLatin1String(" ---> "));
    }

    QFileInfo toInfo(dependency_->getFile2());
    fileLabelText.append(toInfo.fileName());
    
    setTitle(fileLabelText);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::lockEverything()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::lockEverything(bool isLocked)
{
    descEdit_.setEnabled(!isLocked);
    bidirectionalCheck_.setEnabled(!isLocked);
    directionButton_.setEnabled(!isLocked && !bidirectionalCheck_.isChecked());
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::setupLayout()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::setupLayout()
{
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(&directionButton_);
    buttonLayout->addWidget(&bidirectionalCheck_);
    buttonLayout->addWidget(&lockedCheck_);
    buttonLayout->addStretch();

    auto topLayout = new QGridLayout(this);
    topLayout->addLayout(buttonLayout, 0, 0, 1, 1);
    topLayout->addWidget(new QLabel(tr("Description:"), this), 1, 0, 1, 1);
    topLayout->addWidget(&descEdit_, 2, 0, 1, 1);
}
