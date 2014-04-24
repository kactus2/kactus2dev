/* 
 *	Created on:	3.4.2013
 *	Author:		Antti Kamppi
 *	File name:	bspbuildeditor.cpp
 *	Project:		Kactus 2
*/

#include "bspbuildeditor.h"
#include <common/widgets/fileTypeSelector/filetypeselector.h>
#include <common/widgets/cpuSelector/cpuselector.h>

#include <QGridLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QSizePolicy>

BSPBuildEditor::BSPBuildEditor(QSharedPointer<BSPBuildCommand> BSPCommand,
	QSharedPointer<Component> component, 
	QWidget *parent):
QGroupBox(tr("BSP build options"), parent),
bspCommand_(BSPCommand),
fileType_(NULL),
command_(NULL),
arguments_(NULL),
cpuName_(NULL) {

	QLabel* fileTypeLabel = new QLabel(tr("File type"), this);

	fileType_ = new FileTypeSelector(this);
	fileType_->setMinimumWidth(BSPBuildEditor::LINE_WIDTH);
	connect(fileType_, SIGNAL(fileTypeSelected(const QString&)),
		this, SLOT(onFileTypeChange(const QString&)), Qt::UniqueConnection);

	QLabel* comLabel = new QLabel(tr("Command"), this);

	command_ = new QLineEdit(this);
	command_->setPlaceholderText(tr("Command to build the BSP"));
	command_->setMinimumWidth(BSPBuildEditor::LINE_WIDTH);
	connect(command_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onCommandChange(const QString&)), Qt::UniqueConnection);

	QLabel* argLabel = new QLabel(tr("Arguments"), this);

	arguments_ = new QLineEdit(this);
	arguments_->setPlaceholderText(tr("Arguments for the build command"));
	arguments_->setMinimumWidth(BSPBuildEditor::LINE_WIDTH);
	connect(arguments_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onArgumentChange(const QString&)), Qt::UniqueConnection);

	QLabel* cpuLabel = new QLabel(tr("CPU name"), this);

	cpuName_ = new CpuSelector(component, this); 
	cpuName_->setMinimumWidth(BSPBuildEditor::LINE_WIDTH);
	connect(cpuName_, SIGNAL(cpuSelected(const QString&)),
		this, SLOT(onCpuChange(const QString&)), Qt::UniqueConnection);

	QGridLayout* layout = new QGridLayout();
	layout->addWidget(fileTypeLabel, 0, 0, Qt::AlignLeft);
	layout->addWidget(fileType_, 0, 1, Qt::AlignLeft);
	layout->addWidget(comLabel, 1, 0, Qt::AlignLeft);
	layout->addWidget(command_, 1, 1, Qt::AlignLeft);
	layout->addWidget(argLabel, 2, 0, Qt::AlignLeft);
	layout->addWidget(arguments_, 2, 1, Qt::AlignLeft);
	layout->addWidget(cpuLabel, 3, 0, Qt::AlignLeft);
	layout->addWidget(cpuName_, 3, 1, Qt::AlignLeft);

	QHBoxLayout* spacerLayout = new QHBoxLayout(this);
	spacerLayout->addLayout(layout);
	spacerLayout->addStretch();

	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

BSPBuildEditor::~BSPBuildEditor() {
}

bool BSPBuildEditor::isValid() const {
	return true;
}

void BSPBuildEditor::refresh() {
	fileType_->refresh();
	fileType_->selectFileType(bspCommand_->getFileType());
	command_->setText(bspCommand_->getCommand());
	arguments_->setText(bspCommand_->getArguments());
	cpuName_->refresh();
	cpuName_->selectCpu(bspCommand_->getCPUName());
}

void BSPBuildEditor::onFileTypeChange( const QString& fileType ) {
	bspCommand_->setFileType(fileType);
	emit contentChanged();
}

void BSPBuildEditor::onCommandChange( const QString& newCom ) {
	bspCommand_->setCommand(newCom);
	emit contentChanged();
}

void BSPBuildEditor::onArgumentChange( const QString& newArg ) {
	bspCommand_->setArguments(newArg);
	emit contentChanged();
}

void BSPBuildEditor::onCpuChange( const QString& cpuName ) {
	bspCommand_->setCPUName(cpuName);
	emit contentChanged();
}
