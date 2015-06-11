/* 
 *
 *  Created on: 16.1.2011
 *      Author: Antti Kamppi
 */

#include "namegroupbox.h"

#include <IPXACTmodels/validators/namevalidator.h>

#include <QSizePolicy>
#include <QLabel>
#include <QGridLayout>
#include <QValidator>
#include <QRegExpValidator>
#include <QRegExp>

NameGroupBox::NameGroupBox(QWidget* parent,
						   const QString& title): 
QGroupBox(title, parent), 
nameEdit_(0),
displayNameEdit_(0), 
descriptionEdit_(0) {

	// set the size policy for the widget
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

	// set the maximum height for the widget
    setMaximumHeight(130);

	// the layout manager for this widget
	QGridLayout* layout = new QGridLayout(this);

	QLabel* nameLabel = new QLabel(tr("Name:"), this);
	layout->addWidget(nameLabel, 0, 0, 1, 1);

	nameEdit_ = new QLineEdit(this);
	nameEdit_->setToolTip(tr("Set the name for the element"));
	nameEdit_->setProperty("mandatoryField", true);

	NameValidator* validator = new NameValidator(nameEdit_);
	nameEdit_->setValidator(validator);
	layout->addWidget(nameEdit_, 0, 1, 1, 1);

	QLabel* displayLabel = new QLabel(tr("Display Name:"), this);
	layout->addWidget(displayLabel, 1, 0, 1, 1);

	displayNameEdit_ = new QLineEdit(this);
	displayNameEdit_->setToolTip(tr("Set the display name for the element"));
	layout->addWidget(displayNameEdit_, 1, 1, 1, 1);

	QLabel* descriptionLabel = new QLabel(tr("Description:"), this);
    layout->addWidget(descriptionLabel, 2, 0, 1, 1, Qt::AlignTop);

	descriptionEdit_ = new QPlainTextEdit(this);
	descriptionEdit_->setToolTip(tr("Set the description for the element"));
    layout->addWidget(descriptionEdit_, 2, 1, 1, 1);

	// connect the signals that inform of changes made to items
	connect(nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(displayNameEdit_, SIGNAL(textEdited(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(displayNameEdit_, SIGNAL(textEdited(const QString&)),
		this, SIGNAL(displayNameChanged(const QString&)), Qt::UniqueConnection);
	connect(descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
}

NameGroupBox::~NameGroupBox() {
}

QString NameGroupBox::getName() const {
	return nameEdit_->text();
}

QString NameGroupBox::getDisplayName() const {
	return displayNameEdit_->text();
}

QString NameGroupBox::getDescription() const {
	return descriptionEdit_->toPlainText();
}

bool NameGroupBox::isValid() const {
	return !nameEdit_->text().isEmpty();
}

void NameGroupBox::setName( const QString& name ) {
	nameEdit_->setText(name);
}

void NameGroupBox::setDisplayName( const QString& displayName ) {
	displayNameEdit_->setText(displayName);
}

void NameGroupBox::setDescription( const QString& description ) {
	// disconnect the signals 
	disconnect(descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()));
	descriptionEdit_->setPlainText(description);
	connect(descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
}

void NameGroupBox::onDescriptionChanged() {
	emit contentChanged();
	emit descriptionChanged(descriptionEdit_->toPlainText());
}
