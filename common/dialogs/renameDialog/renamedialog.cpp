/* 
 *
 *  Created on: 9.10.2010
 *      Author: kamppia
 */

#include "renamedialog.h"

#include <QMessageBox>
#include <QWidget>
#include <QString>
#include <QPushButton>

#include <QCoreApplication>

RenameDialog::RenameDialog(QWidget* parent):
QMessageBox(QMessageBox::Warning,
		QCoreApplication::applicationName(),
		tr("text"),
		QMessageBox::NoButton,
		parent,
		Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint ), yesButton_(0),
		cancelButton_(0), renameButton_(0) {

	// add the buttons and connect them to appropriate handlers
	yesButton_ = addButton(tr("Yes"), QMessageBox::YesRole);
	connect(yesButton_, SIGNAL(clicked(bool)),
			this, SLOT(onYesClicked()));

	cancelButton_ = addButton(tr("Cancel"), QMessageBox::RejectRole);
	connect(cancelButton_, SIGNAL(clicked(bool)),
			this, SLOT(onCancelClicked()));

	renameButton_ = addButton(tr("Rename"), QMessageBox::ApplyRole);
	connect(renameButton_, SIGNAL(clicked(bool)),
			this, SLOT(onRenameClicked()));

	return;
}

RenameDialog::~RenameDialog() {
	if (yesButton_) {
		delete yesButton_;
	}
	if (cancelButton_) {
		delete cancelButton_;
	}
	if (renameButton_) {
		delete renameButton_;
	}
	return;
}

// event handler when user clicks "yes"
void RenameDialog::onYesClicked() {
	selected_ = RenameDialog::YES;
}

// event handler when user clicks "cancel"
void RenameDialog::onCancelClicked() {
	selected_ = RenameDialog::CANCEL;
}

// event handler when user clicks "rename"
void RenameDialog::onRenameClicked() {
	selected_ = RenameDialog::RENAME;
}

RenameDialog::ClickedButton RenameDialog::clickedButton() {
	return selected_;
}
