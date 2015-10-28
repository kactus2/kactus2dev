//-----------------------------------------------------------------------------
// File: bspbuildeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 03.04.2013
//
// Description:
// The editor to set the BSP build options for an SW view of a HW component.
//-----------------------------------------------------------------------------

#include "bspbuildeditor.h"

#include "cpuselector.h"

#include <common/widgets/fileTypeSelector/filetypeselector.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/kactusExtensions/BSPBuildCommand.h>

#include <QFormLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QSizePolicy>

//-----------------------------------------------------------------------------
// Function: BSPBuildEditor::BSPBuildEditor()
//-----------------------------------------------------------------------------
BSPBuildEditor::BSPBuildEditor(QSharedPointer<BSPBuildCommand> BSPCommand, QSharedPointer<Component> component, 
    QWidget* parent):
QGroupBox(tr("BSP build options"), parent),
    bspCommand_(BSPCommand),
    fileTypeSelector_(new FileTypeSelector(this)),
    commandEditor_(new QLineEdit(this)),
    argumentsEditor_(new QLineEdit(this)),
    cpuSelector_(new CpuSelector(component, this))
{
	fileTypeSelector_->setMinimumWidth(BSPBuildEditor::LINE_WIDTH);
	connect(fileTypeSelector_, SIGNAL(fileTypeSelected(QString const&)),
		this, SLOT(onFileTypeChange(QString const&)), Qt::UniqueConnection);

	commandEditor_->setPlaceholderText(tr("Command to build the BSP"));
	commandEditor_->setMinimumWidth(BSPBuildEditor::LINE_WIDTH);
	connect(commandEditor_, SIGNAL(textEdited(QString const&)),
		this, SLOT(onCommandChange(QString const&)), Qt::UniqueConnection);

	argumentsEditor_->setPlaceholderText(tr("Arguments for the build command"));
	argumentsEditor_->setMinimumWidth(BSPBuildEditor::LINE_WIDTH);
	connect(argumentsEditor_, SIGNAL(textEdited(QString const&)),
		this, SLOT(onArgumentChange(QString const&)), Qt::UniqueConnection);

	cpuSelector_->setMinimumWidth(BSPBuildEditor::LINE_WIDTH);
	connect(cpuSelector_, SIGNAL(cpuSelected(QString const&)),
		this, SLOT(onCpuChange(QString const&)), Qt::UniqueConnection);

	QFormLayout* layout = new QFormLayout();
	layout->addRow(tr("File type"), fileTypeSelector_);
    layout->addRow(tr("Command"), commandEditor_);
	layout->addRow(tr("Arguments"), argumentsEditor_);
	layout->addRow(tr("CPU name"), cpuSelector_);

	QHBoxLayout* spacerLayout = new QHBoxLayout(this);
	spacerLayout->addLayout(layout);
	spacerLayout->addStretch();

	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

//-----------------------------------------------------------------------------
// Function: BSPBuildEditor::~BSPBuildEditor()
//-----------------------------------------------------------------------------
BSPBuildEditor::~BSPBuildEditor()
{
}

//-----------------------------------------------------------------------------
// Function: BSPBuildEditor::isValid()
//-----------------------------------------------------------------------------
bool BSPBuildEditor::isValid() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: BSPBuildEditor::refresh()
//-----------------------------------------------------------------------------
void BSPBuildEditor::refresh()
{
	fileTypeSelector_->refresh();
	fileTypeSelector_->selectFileType(bspCommand_->getFileType());

	commandEditor_->setText(bspCommand_->getCommand());

	argumentsEditor_->setText(bspCommand_->getArguments());

	cpuSelector_->refresh();
	cpuSelector_->selectCpu(bspCommand_->getCPUName());
}

//-----------------------------------------------------------------------------
// Function: BSPBuildEditor::onFileTypeChange()
//-----------------------------------------------------------------------------
void BSPBuildEditor::onFileTypeChange(QString const& fileType)
{
	bspCommand_->setFileType(fileType);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BSPBuildEditor::onCommandChange()
//-----------------------------------------------------------------------------
void BSPBuildEditor::onCommandChange(QString const& newCom)
{
	bspCommand_->setCommand(newCom);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BSPBuildEditor::onArgumentChange()
//-----------------------------------------------------------------------------
void BSPBuildEditor::onArgumentChange(QString const& newArg)
{
	bspCommand_->setArguments(newArg);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BSPBuildEditor::onCpuChange()
//-----------------------------------------------------------------------------
void BSPBuildEditor::onCpuChange(QString const& cpuName)
{
	bspCommand_->setCPUName(cpuName);
	emit contentChanged();
}
