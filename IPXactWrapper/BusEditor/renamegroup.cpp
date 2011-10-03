/* 
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: renamegroup.cpp
 */

#include "renamegroup.h"

#include <QVBoxLayout>

RenameGroup::RenameGroup(QWidget *parent): 
QGroupBox(tr("Renaming"),parent),
renameButton_(tr("Rename signal group"), this),
addKeepButton_(tr("Add new keep column"), this),
addChangeButton_(tr("Add new change column"), this) {


	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&renameButton_);
	layout->addWidget(&addKeepButton_);
	layout->addWidget(&addChangeButton_);
	layout->addStretch();

	connect(&renameButton_, SIGNAL(clicked(bool)),
		this, SIGNAL(renameSignalGroup()), Qt::UniqueConnection);
	connect(&addKeepButton_, SIGNAL(clicked(bool)),
		this, SIGNAL(addNewKeepColumn()), Qt::UniqueConnection);
	connect(&addChangeButton_, SIGNAL(clicked(bool)),
		this, SIGNAL(addNewChangeColumn()), Qt::UniqueConnection);
}

RenameGroup::~RenameGroup() {
}
