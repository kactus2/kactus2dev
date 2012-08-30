/* 
 *  	Created on: 30.8.2012
 *      Author: Antti Kamppi
 * 		filename: writevalueconstrainteditor.cpp
 *		Project: Kactus 2
 */

#include "writevalueconstrainteditor.h"

#include <QSizePolicy>
#include <QVBoxLayout>
#include <QFormLayout>

#include <QDebug>

WriteValueConstraintEditor::WriteValueConstraintEditor(QSharedPointer<WriteValueConstraint> writeConstraint, 
													   const QString& title,
													   QWidget *parent ):
QGroupBox(title, parent),
writeConstraint_(writeConstraint),
noConstrButton_(new QRadioButton(tr("No constraints"), this)),
writeAsReadButton_(new QRadioButton(tr("Write as read"), this)),
useEnumButton_(new QRadioButton(tr("Use enumerated values"), this)),
minMaxButton_(new QRadioButton(tr("Set minimum and maximum limits"), this)),
minBox_(new QSpinBox(this)),
maxBox_(new QSpinBox(this)) {

	minBox_->setDisabled(true);
	minBox_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	minBox_->setRange(0, 9999);
	maxBox_->setDisabled(true);
	maxBox_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	maxBox_->setRange(0, 9999);

	QFormLayout* minMaxLayout = new QFormLayout();
	minMaxLayout->addRow(tr("\tMinimum"), minBox_);
	minMaxLayout->addRow(tr("\tMaximum"), maxBox_);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(noConstrButton_);
	layout->addWidget(writeAsReadButton_);
	layout->addWidget(useEnumButton_);
	layout->addWidget(minMaxButton_);
	layout->addLayout(minMaxLayout);
	layout->addStretch();

	switch (writeConstraint_->getType()) {
		case WriteValueConstraint::WRITE_AS_READ: {
			writeAsReadButton_->setChecked(true);
			break;
												  }
		case WriteValueConstraint::USE_ENUM: {
			useEnumButton_->setChecked(true);
			break;
											 }
		case WriteValueConstraint::MIN_MAX: {
			minMaxButton_->setChecked(true);
			minBox_->setEnabled(true);
			minBox_->setValue(writeConstraint_->getMinimum());
			maxBox_->setEnabled(true);
			maxBox_->setValue(writeConstraint_->getMaximum());
			break;
											}
		default: {
			noConstrButton_->setChecked(true);
			break;
				 }
	}

	connect(noConstrButton_, SIGNAL(clicked(bool)),
		this, SLOT(onNoConstr(bool)), Qt::UniqueConnection);
	connect(writeAsReadButton_, SIGNAL(clicked(bool)),
		this, SLOT(onWriteAsRead(bool)), Qt::UniqueConnection);
	connect(useEnumButton_, SIGNAL(clicked(bool)),
		this, SLOT(onUseEnum(bool)), Qt::UniqueConnection);
	connect(minMaxButton_, SIGNAL(clicked(bool)),
		this, SLOT(onMinMax(bool)), Qt::UniqueConnection);

	connect(minBox_, SIGNAL(valueChanged(int)),
		this, SLOT(onMinChanged(int)), Qt::UniqueConnection);
	connect(maxBox_, SIGNAL(valueChanged(int)),
		this, SLOT(onMaxChanged(int)), Qt::UniqueConnection);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
}

WriteValueConstraintEditor::~WriteValueConstraintEditor() {
}

void WriteValueConstraintEditor::onNoConstr( bool checked ) {
	if (checked) {
		writeConstraint_->setType(WriteValueConstraint::TYPE_COUNT);
		minBox_->setDisabled(true);
		minBox_->clear();
		maxBox_->setDisabled(true);
		maxBox_->clear();
		emit contentChanged();
	}
}

void WriteValueConstraintEditor::onWriteAsRead( bool checked ) {
	if (checked) {
		writeConstraint_->setType(WriteValueConstraint::WRITE_AS_READ);
		minBox_->setDisabled(true);
		minBox_->clear();
		maxBox_->setDisabled(true);
		maxBox_->clear();
		emit contentChanged();
	}
}

void WriteValueConstraintEditor::onUseEnum( bool checked ) {
	if (checked) {
		writeConstraint_->setType(WriteValueConstraint::USE_ENUM);
		minBox_->setDisabled(true);
		minBox_->clear();
		maxBox_->setDisabled(true);
		maxBox_->clear();
		emit contentChanged();
	}
}

void WriteValueConstraintEditor::onMinMax( bool checked ) {
	if (checked) {
		writeConstraint_->setType(WriteValueConstraint::MIN_MAX);
		minBox_->setEnabled(true);
		minBox_->setValue(writeConstraint_->getMinimum());
		maxBox_->setEnabled(true);
		maxBox_->setValue(writeConstraint_->getMaximum());
		emit contentChanged();
	}
}

void WriteValueConstraintEditor::onMinChanged( int newValue ) {
	qDebug() << "onMinChanged";
	writeConstraint_->setMinimum(newValue);
	emit contentChanged();
}

void WriteValueConstraintEditor::onMaxChanged( int newValue ) {
	qDebug() << "onMaxChanged";
	writeConstraint_->setMaximum(newValue);
	emit contentChanged();
}
