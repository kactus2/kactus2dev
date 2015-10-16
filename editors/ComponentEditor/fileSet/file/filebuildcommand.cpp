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
// Function: filebuildcommand::FileBuildCommand()
//-----------------------------------------------------------------------------
FileBuildCommand::FileBuildCommand( QWidget *parent, LibraryInterface* handler,
    QSharedPointer<Component> component, QSharedPointer<File> file):
QGroupBox(tr("Build command"), parent),
buildCommand_(file->getBuildCommand()),
command_(this),
flags_(this),
replaceDefault_(this),
target_(this, handler, component),
layout_(this)
{
	Q_ASSERT_X(file, "FileBuildCommand constructor", "Null File-pointer given to the constructor");
	Q_ASSERT(buildCommand_);
	
	setLayout(&layout_);

	// setup the GUI items to the buildCommand editor
    setupTarget();
	setupCommand();
	setupFlags();
    setupReplaceDefaultFlags();

    setContentsMargins(0, 0, 0, 0);

	// get the data from the buildCommand to the editor
	refresh();
}

//-----------------------------------------------------------------------------
// Function: filebuildcommand::~FileBuildCommand()
//-----------------------------------------------------------------------------
FileBuildCommand::~FileBuildCommand()
{

}

//-----------------------------------------------------------------------------
// Function: filebuildcommand::setupTarget()
//-----------------------------------------------------------------------------
void FileBuildCommand::setupTarget()
{
    QLabel* targetLabel = new QLabel(tr("Target file:"), this);
    targetLabel->setToolTip(tr("Target defines the path to the file derived from the source file"));
    target_.setToolTip(tr("Target defines the path to the file derived from the source file"));

    layout_.addWidget(targetLabel, 0, 0, 1, 1);
    layout_.addWidget(&target_, 0, 1, 1, 2);

    connect(&target_, SIGNAL(contentChanged()), this, SLOT(onTargetChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: filebuildcommand::setupCommand()
//-----------------------------------------------------------------------------
void FileBuildCommand::setupCommand()
{
	QLabel* commandLabel = new QLabel(tr("Command:"), this);
	commandLabel->setToolTip(tr("Command defines a compiler or assembler tool that processes files of this type"));
	command_.setToolTip(tr("Command defines a compiler or assembler tool that processes files of this type"));

    layout_.addWidget(commandLabel, 1, 0, 1, 1);
    layout_.addWidget(&command_, 1, 1, 1, 1);

	connect(&command_, SIGNAL(textEdited(const QString&)), this, SLOT(onCommandChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: filebuildcommand::setupFlags()
//-----------------------------------------------------------------------------
void FileBuildCommand::setupFlags()
{
	QLabel* flagLabel = new QLabel(tr("Flags:"), this);
	flagLabel->setToolTip(tr("Documents any flags to be passed along with the software tool command"));
	flags_.setToolTip(tr("Documents any flags to be passed along with the software tool command"));

    layout_.addWidget(flagLabel, 2, 0, 1, 1);
    layout_.addWidget(&flags_, 2, 1, 1, 2);

	connect(&flags_, SIGNAL(textEdited(const QString&)), this, SLOT(onFlagsChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: filebuildcommand::setupReplaceDefaultFlags()
//-----------------------------------------------------------------------------
void FileBuildCommand::setupReplaceDefaultFlags()
{
    QLabel* replaceLabel = new QLabel(tr("Replace default flags, f(x):"), this);
    replaceLabel->setToolTip(tr("Expression that must evaluate to true or false. When true the flags replace "
        "any default flags from the build script, when false the flags are appended"));
    flags_.setToolTip(tr("Expression that must evaluate to true or false. When true the flags replace any "
        "default flags from the build script, when false the flags are appended"));

    layout_.addWidget(replaceLabel, 3, 0, 1, 1);
    layout_.addWidget(&replaceDefault_, 3, 1, 1, 2);

    connect(&replaceDefault_, SIGNAL(textEdited(const QString&)),
        this, SLOT(onReplaceDefaultChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: filebuildcommand::refresh()
//-----------------------------------------------------------------------------
void FileBuildCommand::refresh()
{
	command_.setText(buildCommand_->getCommand());
	flags_.setText(buildCommand_->getFlags());
    replaceDefault_.setText(buildCommand_->getReplaceDefaultFlags());
    target_.setText(buildCommand_->getTargetName());
}

//-----------------------------------------------------------------------------
// Function: filebuildcommand::onCommandChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onCommandChanged()
{
	buildCommand_->setCommand(command_.text());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filebuildcommand::onFlagsChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onFlagsChanged()
{
	buildCommand_->setFlags(flags_.text());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filebuildcommand::onTargetChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onTargetChanged()
{
	buildCommand_->setTargetName(target_.text());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filebuildcommand::onReplaceDefaultChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onReplaceDefaultChanged()
{
    buildCommand_->setReplaceDefaultFlags(replaceDefault_.text());
    emit contentChanged();
}
