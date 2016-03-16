//-----------------------------------------------------------------------------
// File: filebuildcommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.2.2011
//
// Description:
// FileBuildCommand is a group box for editing buildCommand of a file.
//-----------------------------------------------------------------------------

#include "filebuildcommand.h"

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/BuildCommand.h>

#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::FileBuildCommand()
//-----------------------------------------------------------------------------
FileBuildCommand::FileBuildCommand(QWidget *parent, LibraryInterface* handler,
    QSharedPointer<Component> component, QSharedPointer<File> file):
QGroupBox(tr("Build command"), parent),
    file_(file),
    buildCommand_(),
    commandEditor_(this),
    flagsEditor_(this),
    replaceDefaultEditor_(this),
    targetEditor_(this, handler, component)
{
    Q_ASSERT_X(file, "FileBuildCommand constructor", "Null File-pointer given to the constructor");

    setupLayout();

	refresh();

    connect(&targetEditor_, SIGNAL(contentChanged()), this, SLOT(onTargetChanged()), Qt::UniqueConnection);
    connect(&commandEditor_, SIGNAL(textEdited(QString const& )), 
        this, SLOT(onCommandChanged()), Qt::UniqueConnection);
    connect(&flagsEditor_, SIGNAL(textEdited(QString const& )), this, SLOT(onFlagsChanged()), Qt::UniqueConnection);
    connect(&replaceDefaultEditor_, SIGNAL(textEdited(QString const& )),
        this, SLOT(onReplaceDefaultChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::~FileBuildCommand()
//-----------------------------------------------------------------------------
FileBuildCommand::~FileBuildCommand()
{

}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::refresh()
//-----------------------------------------------------------------------------
void FileBuildCommand::refresh()
{   
    if (file_->getBuildCommand())
    {
        buildCommand_ = file_->getBuildCommand();
    }
    else
    {
        buildCommand_ = QSharedPointer<BuildCommand>(new BuildCommand());    
    }

    commandEditor_.setText(buildCommand_->getCommand());
    flagsEditor_.setText(buildCommand_->getFlags());
    replaceDefaultEditor_.setText(buildCommand_->getReplaceDefaultFlags());
    targetEditor_.setText(buildCommand_->getTargetName());
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onCommandChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onCommandChanged()
{
	buildCommand_->setCommand(commandEditor_.text());
    updateFileBuildCommand();
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onFlagsChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onFlagsChanged()
{
	buildCommand_->setFlags(flagsEditor_.text());
    updateFileBuildCommand();
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onTargetChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onTargetChanged()
{
	buildCommand_->setTargetName(targetEditor_.text());
    updateFileBuildCommand();
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onReplaceDefaultChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onReplaceDefaultChanged()
{
    buildCommand_->setReplaceDefaultFlags(replaceDefaultEditor_.text());
    updateFileBuildCommand();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::updateFileBuildCommand()
//-----------------------------------------------------------------------------
void FileBuildCommand::updateFileBuildCommand()
{
    bool emptyBuildCommand = commandEditor_.text().isEmpty() && flagsEditor_.text().isEmpty() &&
        targetEditor_.text().isEmpty() && replaceDefaultEditor_.text().isEmpty();

    if (!file_->getBuildCommand() && !emptyBuildCommand)
    {
        file_->setBuildcommand(buildCommand_);
    }
    else if (emptyBuildCommand)
    {
        file_->setBuildcommand(QSharedPointer<BuildCommand>(0));
    }
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::setupLayout()
//-----------------------------------------------------------------------------
void FileBuildCommand::setupLayout()
{
    QGridLayout* topLayout = new QGridLayout(this);

    QString targetToolTip(tr("Target defines the path to the file derived from the source file."));
    QLabel* targetLabel = new QLabel(tr("Target file:"), this);
    targetLabel->setToolTip(targetToolTip);
    targetEditor_.setToolTip(targetToolTip);

    topLayout->addWidget(targetLabel, 0, 0, 1, 1);
    topLayout->addWidget(&targetEditor_, 0, 1, 1, 2);

    QString commandToolTip(tr("Command defines a compiler or assembler tool that processes files of this type."));
    QLabel* commandLabel = new QLabel(tr("Command:"), this);
    commandLabel->setToolTip(commandToolTip);
    commandEditor_.setToolTip(commandToolTip);

    topLayout->addWidget(commandLabel, 1, 0, 1, 1);
    topLayout->addWidget(&commandEditor_, 1, 1, 1, 2);

    QString flagToolTip(tr("Documents any flags to be passed along with the software tool command."));
    QLabel* flagLabel = new QLabel(tr("Flags:"), this);
    flagLabel->setToolTip(flagToolTip);
    flagsEditor_.setToolTip(flagToolTip);

    topLayout->addWidget(flagLabel, 2, 0, 1, 1);
    topLayout->addWidget(&flagsEditor_, 2, 1, 1, 2);

    QString replaceFlagsToolTip(tr("Expression that must evaluate to true or false. When true, the flags replace "
        "any default flags from the build script. When false, the given flags are appended."));
    QLabel* replaceLabel = new QLabel(tr("Replace default flags, f(x):"), this);
    replaceLabel->setToolTip(replaceFlagsToolTip);
    flagsEditor_.setToolTip(replaceFlagsToolTip);

    topLayout->addWidget(replaceLabel, 3, 0, 1, 1);
    topLayout->addWidget(&replaceDefaultEditor_, 3, 1, 1, 2);

    setContentsMargins(0, 0, 0, 0);
}
