/* 
 *	Created on: 18.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	filesavedialog.cpp
 * 	Project:	Kactus 2
*/

#include "filesavedialog.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>

FileSaveDialog::FileSaveDialog(QWidget *parent):
QDialog(parent),
view_(new QTableView(this)) {

	QLabel* infoText = new QLabel(tr("Select names for the output files"), this);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
		Qt::Horizontal, this);

	connect(buttons, SIGNAL(accepted()), 
		this, SLOT(accept()), Qt::UniqueConnection);
	connect(buttons, SIGNAL(rejected()), 
		this, SLOT(reject()), Qt::UniqueConnection);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(infoText);
	layout->addWidget(view_);
	layout->addWidget(buttons);
}

FileSaveDialog::~FileSaveDialog() {
}
