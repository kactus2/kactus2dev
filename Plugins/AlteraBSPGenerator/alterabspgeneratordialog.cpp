/* 
 *	Created on:	19.4.2013
 *	Author:		Antti Kamppi
 *	File name:	alterabspgeneratordialog.cpp
 *	Project:		Kactus 2
*/

#include "alterabspgeneratordialog.h"

#include <QVBoxLayout>

AlteraBSPGeneratorDialog::AlteraBSPGeneratorDialog(QSharedPointer<Component> component, QWidget *parent):
CommandLineGeneratorDialog(parent),
component_(component) {

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(output_);
	topLayout->addLayout(buttonLayout_);
}

AlteraBSPGeneratorDialog::~AlteraBSPGeneratorDialog() {
}
