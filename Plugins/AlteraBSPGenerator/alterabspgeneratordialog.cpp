/* 
 *	Created on:	19.4.2013
 *	Author:		Antti Kamppi
 *	File name:	alterabspgeneratordialog.cpp
 *	Project:		Kactus 2
*/

#include "alterabspgeneratordialog.h"
#include <library/LibraryManager/libraryinterface.h>
#include <IPXACTmodels/bspbuildcommand.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/component.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QGuiApplication>
#include <QCursor>

#include <QDebug>

const QString AlteraBSPGeneratorDialog::VARIABLE_NAMES[AlteraBSPGeneratorDialog::VARIABLE_COUNT] = {
	"NIOS_COMMAND_SHELL_WIN",
	"NIOS_COMMAND_SHELL_LINUX"
};

AlteraBSPGeneratorDialog::AlteraBSPGeneratorDialog(LibraryInterface* handler, 
	QSharedPointer<Component> component, 
	QWidget *parent):
CommandLineGeneratorDialog(parent),
	handler_(handler),
	component_(component),
viewSelector_(NULL),
dirButton_(NULL),
commandLabel_(NULL), 
outPutDirLabel_(NULL),
sourceLabel_(NULL),
argLabel_(NULL),
targetDir_(),
currentView_(),
generatedPaths_() {

	Q_ASSERT(handler_);
	Q_ASSERT(component_);

	setMinimumWidth(600);
	setMinimumHeight(400);

	viewSelector_ = new ViewSelector(ViewSelector::SW_VIEWS, component, this, false);
	viewSelector_->refresh();
	connect(viewSelector_, SIGNAL(viewSelected(const QString&)),
		this, SLOT(onViewChange(const QString&)), Qt::UniqueConnection);
	dirButton_ = new QPushButton(tr("Select..."), this);
	connect(dirButton_, SIGNAL(clicked(bool)),
		this, SLOT(onSelectOutput()), Qt::UniqueConnection);
	QFormLayout* settingsLayout = new QFormLayout();
	settingsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	settingsLayout->addRow(tr("1. Select SW view"), viewSelector_);
	settingsLayout->addRow(tr("2. Select output BSP directory"), dirButton_);

	QLabel* commandName = new QLabel(tr("Command:"), this);
	commandLabel_ = new QLabel(this);
	outPutDirLabel_ = new QLabel(this);
	sourceLabel_ = new QLabel(this);
	argLabel_ = new QLabel(this);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(settingsLayout);
	topLayout->addWidget(commandName, 0, Qt::AlignLeft);
	topLayout->addWidget(commandLabel_, 0, Qt::AlignLeft);
	topLayout->addWidget(outPutDirLabel_, 0, Qt::AlignLeft);
	topLayout->addWidget(sourceLabel_, 0, Qt::AlignLeft);
	topLayout->addWidget(argLabel_, 0, Qt::AlignLeft);
	topLayout->addWidget(outputBox_, 1);
	topLayout->addLayout(statusLayout_);
	topLayout->addLayout(buttonLayout_);

	process_ = new QProcess(this);
	QStringList environment = QProcess::systemEnvironment();
	process_->setEnvironment(environment);
	connectProcessToOutput();

	// by default the target directory is on the root of the component
	targetDir_ = handler_->getDirectoryPath(*component_->getVlnv());
	targetDir_.append("/BSP");

	onViewChange(viewSelector_->currentText());
}

AlteraBSPGeneratorDialog::~AlteraBSPGeneratorDialog() {
}

void AlteraBSPGeneratorDialog::onViewChange( const QString& viewName ) {
	currentView_ = component_->findSWView(viewName);
	Q_ASSERT(currentView_);

	updateCommand();
}

void AlteraBSPGeneratorDialog::onSelectOutput() {
	QString newTarget = QFileDialog::getExistingDirectory(this, tr("Select directory to generate BSP to."),
		targetDir_);

	// if user selected a directory
	if (!newTarget.isEmpty()) {
		targetDir_ = newTarget;
		updateCommand();
	}
}

void AlteraBSPGeneratorDialog::updateCommand() {
	QString command("[COMMAND MISSING]");
	QString sourceFile("[SOURCE_FILE MISSING]");
	QString args("[ARGUMENTS MISSING]");
	QString cpuName("[CPU_NAME MISSING]");

	Q_ASSERT(currentView_);
	QSharedPointer<BSPBuildCommand> com = currentView_->getBSPBuildCommand();

	if (com) {

		// only the defined elements are set

		if (!com->getCommand().isEmpty()) {
			command = com->getCommand();
		}

		// search the for the source file
		QString sourceFileType = com->getFileType();
		QStringList sourceFiles = component_->findFilesByFileType(sourceFileType);
		
		// if theres exactly one file that matches
		if (sourceFiles.size() == 1) {
			QString xmlPath = handler_->getPath(*component_->getVlnv());
			sourceFile = General::getAbsolutePath(xmlPath, sourceFiles.first());
		}
		// if there are several source files.
		else if (sourceFiles.size() > 1) {
			sourceFile = tr("[SEVERAL SOURCE FILES FOUND]");
		}

		// arguments may not be necessary
		args = com->getArguments();
		
		if (!com->getCPUName().isEmpty()) {
			cpuName = com->getCPUName();
		}
	}

// 	QString wholeCommand = QString("%1 %2 %3 %4 --cpu-name %5").arg(command).arg(targetDir_).arg(
// 		sourceFile).arg(args).arg(cpuName);

	commandLabel_->setText(command);
	outPutDirLabel_->setText(targetDir_);
	sourceLabel_->setText(sourceFile);
	argLabel_->setText(QString("%1 --cpu-name %2").arg(args).arg(cpuName));
}

void AlteraBSPGeneratorDialog::onRunClicked() {
	// clear previous prints
	output_->clear();

	QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	// make sure the output path exists
	QDir targetDirPath(targetDir_);
	QString dirName = targetDirPath.dirName();
	targetDirPath.cdUp();
	targetDirPath.mkpath(dirName);

	// on windows the command prompt is run and commands written to it
#ifdef Q_OS_WIN32

	runWindowsCommands();

	// others run the shell script
#else
	runOtherCommands();
#endif

	// add the target dir to the list to remember where the generator is run.
	Q_ASSERT(currentView_);

	generatedPaths_.append(AlteraBSPGeneratorDialog::GenerationOptions(targetDir_ + "/", currentView_->getName()));

	QGuiApplication::restoreOverrideCursor();
}

void AlteraBSPGeneratorDialog::runWindowsCommands() {
	process_->start("cmd");

	// if process can not be started successfully
	if (!process_->waitForStarted()) {
		output_->printError(tr("Process could not be started successfully."));
		statusLabel_->setText(tr("Could not start."));
		process_->close();
		return;
	}

	// inform user that batch file is run
	process_->write("echo Running the batch file to start the Nios II command shell.\n");

	// find the saved path
	QSettings settings;
	QString batchPath = settings.value("K2Variables/" +
		AlteraBSPGeneratorDialog::VARIABLE_NAMES[AlteraBSPGeneratorDialog::WIN_PATH] +
		"/value").toString();

	// run the batch file
	QString batchCom = "\"" + batchPath + "\"\n";
	process_->write(batchCom.toLatin1());

	runCygwinCommands();
}

void AlteraBSPGeneratorDialog::runOtherCommands() {
	process_->start("/bin/bash");

	// if process can not be started successfully
	if (!process_->waitForStarted()) {
		output_->printError(tr("Process could not be started successfully."));
		statusLabel_->setText(tr("Could not start."));
		process_->close();
		return;
	}

	// inform user that batch file is run
	process_->write("echo Running the shell script to set the environment variables for Nios II tools.\n");

	// find the saved path
	QSettings settings;
	QString batchPath = settings.value("K2Variables/" +
		AlteraBSPGeneratorDialog::VARIABLE_NAMES[AlteraBSPGeneratorDialog::LINUX_PATH] +
		"/value").toString();

	// run the batch file
	QString batchCom = "\"" + batchPath + "\"\n";
	process_->write(batchCom.toLatin1());

	runCygwinCommands();
}

const QList<AlteraBSPGeneratorDialog::GenerationOptions>& AlteraBSPGeneratorDialog::getCreatedDirs() const {
	return generatedPaths_;
}

void AlteraBSPGeneratorDialog::runCygwinCommands() {
	// convert the output dir path
	QString outputComPath = QString("export kactus2Target=\"$(cygpath -u '%1')\"\n").arg(outPutDirLabel_->text());
	process_->write(outputComPath.toLatin1());

	QString sourceFilePath = QString("export kactus2Source=\"$(cygpath -u '%1')\"\n").arg(sourceLabel_->text());
	process_->write(sourceFilePath.toLatin1());

	// print the saved paths
	process_->write("echo The converted output directory path:\n");
	process_->write("echo $kactus2Target\n");
	process_->write("echo \n");
	process_->write("echo The converted source file path:\n");
	process_->write("echo $kactus2Source\n");

	process_->write("echo \n");
	process_->write("echo Path variables set, running the generation command...\n");
	process_->write("echo \n");

	QString bspCommand = QString("%1 $kactus2Target $kactus2Source %3\n").arg(
		commandLabel_->text()).arg(argLabel_->text());

	process_->write(bspCommand.toLatin1());

	process_->write("exit\n");

	process_->write("exit\n");
	process_->waitForBytesWritten();
	if (!process_->waitForFinished()) {
		output_->printError(tr("Process could not be finished successfully."));
		statusLabel_->setText(tr("Could not finish."));
		process_->close();
		return;
	}

	process_->close();
}

AlteraBSPGeneratorDialog::GenerationOptions::GenerationOptions( const QString& path, const QString& view ):
dirPath_(path), 
swViewName_(view) {
}
