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
runButton_(NULL),
cancelButton_(NULL),
okButton_(NULL) {

	outputBox_ = new QGroupBox(tr("Output"), this);
	QHBoxLayout* boxLayout = new QHBoxLayout(outputBox_);
	output_ = new MessageConsole(outputBox_);
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
		this, SLOT(onStandardOutputRead), Qt::UniqueConnection);
	connect(process_, SIGNAL(readyReadStandardError()),
		this, SLOT(onStandardErrorRead()), Qt::UniqueConnection);
}

void CommandLineGeneratorDialog::onStandardOutputRead() {
	QString output(process_->readAllStandardOutput());
	output_->onNoticeMessage(output);
}

void CommandLineGeneratorDialog::onStandardErrorRead() {
	QString output(process_->readAllStandardError());
	output_->onErrorMessage(output);
}
