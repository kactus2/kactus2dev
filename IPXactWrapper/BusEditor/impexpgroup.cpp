/*
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: impexpgroup.cpp
 */

#include "impexpgroup.h"

#include <QHBoxLayout>

ImpExpGroup::ImpExpGroup(QWidget *parent): 
QGroupBox(tr("Import/export"), parent),
importButton_(tr("Import csv"), this),
exportButton_(tr("Export csv"), this) {

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&importButton_);
	layout->addWidget(&exportButton_);

	connect(&importButton_, SIGNAL(clicked(bool)),
		this, SIGNAL(importCSV()), Qt::UniqueConnection);
	connect(&exportButton_, SIGNAL(clicked(bool)),
		this, SIGNAL(exportCSV()), Qt::UniqueConnection);
}

ImpExpGroup::~ImpExpGroup() {
}
