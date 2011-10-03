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
#include <QVBoxLayout>
#include <QHBoxLayout>

FileBuildCommand::FileBuildCommand( QWidget *parent, File* file,
								   const QFileInfo& baseLocation):
QWidget(parent), buildCommand_(NULL), 
command_(this),
commandAttributes_(this, tr("Command attributes")),
flags_(this),
flagAttributes_(this, tr("Flag attributes")),
replaceDefault_(tr("Replace default flags"),this),
replaceAttributes_(this, tr("Replace attributes")),
target_(this, baseLocation),
targetAttributes_(this, tr("Target attributes")),
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
	setupReplaceDefault();
	setupTarget();

	// get the data from the buildCommand to the editor
	restore();
}

FileBuildCommand::~FileBuildCommand() {
}

void FileBuildCommand::setupCommand() {

	// setup the QGroupBox containing the elements
	QGroupBox* commandBox = new QGroupBox(tr("Command"), this);
	QHBoxLayout* commandLayout = new QHBoxLayout();
	QLabel* commandLabel = new QLabel(tr("Command:"), commandBox);
	commandLabel->setToolTip(tr("Command defines a compiler or assembler tool"
		" that processes files of this type"));
	command_.setToolTip(tr("Command defines a compiler or assembler tool"
		" that processes files of this type"));
	commandLayout->addWidget(commandLabel);
	commandLayout->addWidget(&command_);

	QVBoxLayout* boxLayout = new QVBoxLayout(commandBox);
	boxLayout->addLayout(commandLayout);
	boxLayout->addWidget(&commandAttributes_);

	layout_.addWidget(commandBox, 0, 0, 1, 1);

	connect(&command_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&commandAttributes_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

void FileBuildCommand::setupFlags() {

	// setup the QGroupBox containing the elements
	QGroupBox* flagBox = new QGroupBox(tr("Flags"), this);
	QHBoxLayout* flagLayout = new QHBoxLayout();
	QLabel* flagLabel = new QLabel(tr("Flags:"), flagBox);
	flagLabel->setToolTip(tr("Documents any flags to be passed along with the"
		" software tool command"));
	flags_.setToolTip(tr("Documents any flags to be passed along with the"
		" software tool command"));
	flagLayout->addWidget(flagLabel);
	flagLayout->addWidget(&flags_);

	QVBoxLayout* boxLayout = new QVBoxLayout(flagBox);
	boxLayout->addLayout(flagLayout);
	boxLayout->addWidget(&flagAttributes_);

	layout_.addWidget(flagBox, 0, 1, 1, 1);

	connect(&flags_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&flagAttributes_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

void FileBuildCommand::setupReplaceDefault() {

	// setup the QGroupBox containing the elements
	QGroupBox* replaceBox = new QGroupBox(tr("Replace default flags"), this);
	QVBoxLayout* replaceLayout = new QVBoxLayout(replaceBox);
	replaceLayout->addWidget(&replaceDefault_);
	replaceLayout->addWidget(&replaceAttributes_);

	replaceDefault_.setToolTip(tr("When true the flags replace any default "
		"flags from the build script, when false the flags are appended"));

	layout_.addWidget(replaceBox, 1, 0, 1, 1);

	connect(&replaceDefault_, SIGNAL(clicked(bool)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&replaceAttributes_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

void FileBuildCommand::setupTarget() {

	// setup the QGroupBox containing the elements
	QGroupBox* targetBox = new QGroupBox(tr("Build target"), this);
	QHBoxLayout* hLayout = new QHBoxLayout();
	QLabel* targetLabel = new QLabel(tr("Target file:"), this);
	targetLabel->setToolTip(tr("Target defines the path to the file derived "
		"from the source file"));
	target_.setToolTip(tr("Target defines the path to the file derived "
		"from the source file"));
	hLayout->addWidget(targetLabel);
	hLayout->addWidget(&target_);

	QVBoxLayout* boxLayout = new QVBoxLayout(targetBox);
	boxLayout->addLayout(hLayout);
	boxLayout->addWidget(&targetAttributes_);

	layout_.addWidget(targetBox, 1, 1, 1, 1);

	connect(&target_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&targetAttributes_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

void FileBuildCommand::apply() {

	buildCommand_->setCommand(command_.text());
	buildCommand_->setCommandAttributes(commandAttributes_.getAttributes());

	buildCommand_->setFlags(flags_.text());
	buildCommand_->setFlagAttributes(flagAttributes_.getAttributes());
	
	buildCommand_->setReplaceDefaultFlags(replaceDefault_.isChecked());
	buildCommand_->setDefaultFlagAttributes(replaceAttributes_.getAttributes());

	buildCommand_->setTargetName(target_.text());
	buildCommand_->setTargetNameAttributes(targetAttributes_.getAttributes());
}

void FileBuildCommand::restore() {
	command_.setText(buildCommand_->getCommand());
	commandAttributes_.setAttributes(buildCommand_->getCommandAttributes());

	flags_.setText(buildCommand_->getFlags());
	flagAttributes_.setAttributes(buildCommand_->getFlagAttributes());

	replaceDefault_.setChecked(buildCommand_->getReplaceDefaultFlags());
	replaceAttributes_.setAttributes(buildCommand_->getDefaultFlagAttributes());

	target_.setText(buildCommand_->getTargetName());
	targetAttributes_.setAttributes(buildCommand_->getTargetNameAttributes());
}
