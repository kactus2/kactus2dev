/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: filebuildcommand.cpp
 */

#include "filebuildcommand.h"

#include <models/file.h>
#include <models/buildcommand.h>

#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>

FileBuildCommand::FileBuildCommand( QWidget *parent, 
								   File* file,
								   const QFileInfo& baseLocation):
QWidget(parent), buildCommand_(NULL), 
command_(this),
flags_(this),
replaceDefault_(tr("Replace default flags"),this),
target_(this, baseLocation),
layout_(this) {

	Q_ASSERT_X(file, "FileBuildCommand constructor",
		"Null File-pointer given to the constructor");
	
	buildCommand_ = file->getBuildcommand();

	// if buildCommand was not defined
	if (!buildCommand_) {

		// create an empty build command and add it to the file
		buildCommand_ = new BuildCommand();
		file->setBuildcommand(buildCommand_);
	}

	setLayout(&layout_);

	// setup the GUI items to the buildCommand editor
	setupCommand();
	setupFlags();
	setupTarget();

	// get the data from the buildCommand to the editor
	restore();
}

FileBuildCommand::~FileBuildCommand() {
}

void FileBuildCommand::setupCommand() {

	// setup the QGroupBox containing the elements
	QLabel* commandLabel = new QLabel(tr("Command:"), this);
	commandLabel->setToolTip(tr("Command defines a compiler or assembler tool"
		" that processes files of this type"));
	command_.setToolTip(tr("Command defines a compiler or assembler tool"
		" that processes files of this type"));

	layout_.addWidget(commandLabel, 0, 0, 1, 1);
	layout_.addWidget(&command_, 0, 1, 1, 2);

	connect(&command_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

void FileBuildCommand::setupFlags() {

	// setup the QGroupBox containing the elements
	QLabel* flagLabel = new QLabel(tr("Flags:"), this);
	flagLabel->setToolTip(tr("Documents any flags to be passed along with the"
		" software tool command"));
	flags_.setToolTip(tr("Documents any flags to be passed along with the"
		" software tool command"));

	layout_.addWidget(flagLabel, 1, 0, 1, 1);
	layout_.addWidget(&flags_, 1, 1, 1, 1);
	layout_.addWidget(&replaceDefault_, 1, 2, 1, 1);

	replaceDefault_.setToolTip(tr("When true the flags replace any default "
		"flags from the build script, when false the flags are appended"));

	connect(&replaceDefault_, SIGNAL(clicked(bool)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(&flags_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

void FileBuildCommand::setupTarget() {

	// setup the QGroupBox containing the elements
	QLabel* targetLabel = new QLabel(tr("Target file:"), this);
	targetLabel->setToolTip(tr("Target defines the path to the file derived "
		"from the source file"));
	target_.setToolTip(tr("Target defines the path to the file derived "
		"from the source file"));

	layout_.addWidget(targetLabel, 2, 0, 1, 1);
	layout_.addWidget(&target_, 2, 1, 1, 2);

	connect(&target_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

void FileBuildCommand::apply() {

	buildCommand_->setCommand(command_.text());

	buildCommand_->setFlags(flags_.text());
	
	buildCommand_->setReplaceDefaultFlags(replaceDefault_.isChecked());

	buildCommand_->setTargetName(target_.text());
}

void FileBuildCommand::restore() {
	command_.setText(buildCommand_->getCommand());

	flags_.setText(buildCommand_->getFlags());

	replaceDefault_.setChecked(buildCommand_->getReplaceDefaultFlags());

	target_.setText(buildCommand_->getTargetName());
}
