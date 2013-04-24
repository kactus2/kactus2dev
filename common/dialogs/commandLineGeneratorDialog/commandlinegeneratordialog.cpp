/* 
 *	Created on:	19.4.2013
 *	Author:		Antti Kamppi
 *	File name:	commandlinegeneratordialog.cpp
 *	Project:		Kactus 2
*/

#include "commandlinegeneratordialog.h"

#include <QString>
#include <QByteArray>

CommandLineGeneratorDialog::CommandLineGeneratorDialog(QWidget *parent):
QDialog(parent),
output_(NULL),
outputBox_(NULL),
process_(NULL),
buttonLayout_(NULL),
statusLayout_(NULL),
statusLabel_(NULL),
runButton_(NULL),
cancelButton_(NULL),
okButton_(NULL) {

	QLabel* statusName = new QLabel(tr("Status: "), this);
	statusLabel_ = new QLabel(tr("Waiting"), this);
	statusLayout_ = new QHBoxLayout();
	statusLayout_->addWidget(statusName);
	statusLayout_->addWidget(statusLabel_);
	statusLayout_->addStretch();

	outputBox_ = new QGroupBox(tr("Output"), this);
	QHBoxLayout* boxLayout = new QHBoxLayout(outputBox_);
	output_ = new CommandLineConsole(outputBox_);
	boxLayout->addWidget(output_);

	// set up the buttons
	runButton_ = new QPushButton(tr("Run"), this);
	okButton_ = new QPushButton(tr("Ok"), this);
	cancelButton_ = new QPushButton(tr("Cancel"), this);
	buttonLayout_ = new QHBoxLayout();
	buttonLayout_->addStretch();
	buttonLayout_->addWidget(runButton_, 0, Qt::AlignRight);
	buttonLayout_->addWidget(okButton_, 0, Qt::AlignRight);
	buttonLayout_->addWidget(cancelButton_, 0, Qt::AlignRight);
	connect(runButton_, SIGNAL(clicked(bool)),
		this, SLOT(onRunClicked()), Qt::UniqueConnection);
	connect(okButton_, SIGNAL(clicked(bool)),
		this, SLOT(accept()), Qt::UniqueConnection);
	connect(cancelButton_, SIGNAL(clicked(bool)),
		this, SLOT(reject()), Qt::UniqueConnection);
}

CommandLineGeneratorDialog::~CommandLineGeneratorDialog() {
}

void CommandLineGeneratorDialog::connectProcessToOutput() {

	// the process is set up in sub-class but it has to be done before calling this function.
	Q_ASSERT(process_);

	connect(process_, SIGNAL(readyReadStandardOutput()),
		this, SLOT(onStandardOutputRead()), Qt::UniqueConnection);
	connect(process_, SIGNAL(readyReadStandardError()),
		this, SLOT(onStandardErrorRead()), Qt::UniqueConnection);
	connect(process_, SIGNAL(started()),
		this, SLOT(onProcessStarted()), Qt::UniqueConnection);
	connect(process_, SIGNAL(finished(int, QProcess::ExitStatus)),
		this, SLOT(onProcessFinished(int, QProcess::ExitStatus)), Qt::UniqueConnection);
}

void CommandLineGeneratorDialog::onStandardOutputRead() {
	QString output(process_->readAllStandardOutput());
	output_->printStandard(output);
}

void CommandLineGeneratorDialog::onStandardErrorRead() {
	QString output(process_->readAllStandardError());
	output_->printError(output);
}

void CommandLineGeneratorDialog::onProcessStarted() {
	statusLabel_->setText(tr("Running..."));
}

void CommandLineGeneratorDialog::onProcessFinished( int exitCode, QProcess::ExitStatus exitStatus ) {
	QString text;

	switch (exitStatus) {
	case QProcess::NormalExit: {
		text.append(tr("Finished normally "));
		break;
										}
	default: {
		text.append(tr("Crashed "));
		break;
				}
	}

	text.append(tr("with exit code %1.").arg(exitCode));
	statusLabel_->setText(text);
}
