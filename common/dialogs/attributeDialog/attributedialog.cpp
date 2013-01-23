/* 
 *
 *  Created on: 15.1.2011
 *      Author: Antti Kamppi
 */

#include "attributedialog.h"

#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>

AttributeDialog::Attribute::Attribute(const QString name,
		const QString value): name_(name), value_(value) {
}

AttributeDialog::AttributeDialog(QWidget* parent): QDialog(parent),
nameEdit_(0), valueEdit_(0) {

	// layout for the dialog
	QGridLayout* layout = new QGridLayout(this);

	QLabel* nameLabel = new QLabel(tr("Name:"), this);
	layout->addWidget(nameLabel, 0, 0, 1, 1);

	nameEdit_ = new QLineEdit(this);
	nameEdit_->setToolTip(tr("Set the name for the attribute"));
	layout->addWidget(nameEdit_, 0, 1, 1, 1);

	QLabel* valueLabel = new QLabel(tr("Value:"), this);
	layout->addWidget(valueLabel, 1, 0, 1, 1);

	valueEdit_ = new QLineEdit(this);
	valueEdit_->setToolTip(tr("Set the value for the attribute"));
	layout->addWidget(valueEdit_, 1, 1, 1, 1);

	QDialogButtonBox* okCancelButtons = new QDialogButtonBox(
			QDialogButtonBox::Ok | QDialogButtonBox::Cancel ,Qt::Horizontal,
			this);
	connect(okCancelButtons, SIGNAL(accepted()),
			this, SLOT(accept()), Qt::UniqueConnection);
	connect(okCancelButtons, SIGNAL(rejected()),
			this, SLOT(reject()), Qt::UniqueConnection);
	layout->addWidget(okCancelButtons, 2, 0, 1, 2);
}

AttributeDialog::~AttributeDialog() {
}

AttributeDialog::Attribute* AttributeDialog::execDialog() {

	// execute at least once
	do {
		// if user clicked "cancel"
		if (!QDialog::exec()) {
			return 0;
		}

		// the dialog is executed again if the given info is not valid
	} while (nameEdit_->text().isEmpty() || valueEdit_->text().isEmpty());

	return new AttributeDialog::Attribute(nameEdit_->text(),
			valueEdit_->text());
}
