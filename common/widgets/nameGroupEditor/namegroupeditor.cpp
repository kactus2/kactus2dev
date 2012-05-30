/* 
 *  	Created on: 28.5.2012
 *      Author: Antti Kamppi
 * 		filename: namegroupeditor.cpp
 *		Project: Kactus 2
 */

#include "namegroupeditor.h"
#include <common/validators/nameValidator/namevalidator.h>

#include <QLabel>
#include <QGridLayout>

NameGroupEditor::NameGroupEditor( General::NameGroup& nameGroup,
								 QWidget *parent,
								 const QString& title /*= QString("Names")*/ ):
QGroupBox(title, parent),
nameGroup_(nameGroup),
nameEdit_(this),
displayNameEdit_(this),
descriptionEdit_(this) {

	// the layout manager for this widget
	QGridLayout* layout = new QGridLayout(this);

	QLabel* nameLabel = new QLabel(tr("Name:"), this);
	layout->addWidget(nameLabel, 0, 0, 1, 1);

	nameEdit_.setToolTip(tr("Set the name for the element"));
	nameEdit_.setProperty("mandatoryField", true);

	NameValidator* validator = new NameValidator(&nameEdit_);
	nameEdit_.setValidator(validator);
	layout->addWidget(&nameEdit_, 0, 1, 1, 1);

	QLabel* displayLabel = new QLabel(tr("Display Name:"), this);
	layout->addWidget(displayLabel, 1, 0, 1, 1);

	displayNameEdit_.setToolTip(tr("Set the display name for the element"));
	layout->addWidget(&displayNameEdit_, 1, 1, 1, 1);

	QLabel* descriptionLabel = new QLabel(tr("Description:"), this);
	layout->addWidget(descriptionLabel, 2, 0, 1, 1);

	descriptionEdit_.setToolTip(tr("Set the description for the element"));
	layout->addWidget(&descriptionEdit_, 3, 0, 1, 2);

	refresh();

	connect(&nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onNameChanged(const QString&)), Qt::UniqueConnection);
	connect(&displayNameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onDisplayNameChanged(const QString&)), Qt::UniqueConnection);
	connect(&descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
}

NameGroupEditor::~NameGroupEditor() {
}

QString NameGroupEditor::getName() const {
	return nameGroup_.name_;
}

QString NameGroupEditor::getDisplayName() const {
	return nameGroup_.displayName_;
}

QString NameGroupEditor::getDescription() const {
	return nameGroup_.description_;
}

void NameGroupEditor::onNameChanged( const QString& newName ) {
	nameGroup_.name_ = newName;
	emit contentChanged();
}

void NameGroupEditor::onDisplayNameChanged( const QString& newName ) {
	nameGroup_.displayName_ = newName;
	emit contentChanged();
}

void NameGroupEditor::onDescriptionChanged() {
	nameGroup_.description_ = descriptionEdit_.toPlainText();
	emit contentChanged();
}

bool NameGroupEditor::isValid() const {
	return !nameGroup_.name_.isEmpty();
}

void NameGroupEditor::refresh() {
	nameEdit_.setText(nameGroup_.name_);
	displayNameEdit_.setText(nameGroup_.displayName_);
	descriptionEdit_.setPlainText(nameGroup_.description_);
}
