/* 
 *	Created on:	3.4.2013
 *	Author:		Antti Kamppi
 *	File name:	filetypeselector.cpp
 *	Project:		Kactus 2
*/

#include "filetypeselector.h"

#include <QSettings>

FileTypeSelector::FileTypeSelector(QWidget *parent):
QComboBox(parent) {

	setEditable(true);

	connect(this, SIGNAL(currentTextChanged(const QString&)),
		this, SLOT(onFileChange(const QString&)), Qt::UniqueConnection);
}

FileTypeSelector::~FileTypeSelector() {
}

void FileTypeSelector::refresh() {
	disconnect(this, SIGNAL(currentTextChanged(const QString&)),
		this, SLOT(onFileChange(const QString&)));

	// remove the previous items
	clear();

	// read the possible file types from the settings
	QSettings settings;

	settings.beginGroup("FileTypes");
	QStringList typeNames = settings.childKeys();
	settings.endGroup();

	// sort the names aphabetically
	typeNames.sort(Qt::CaseInsensitive);

	// add the specified types to the combo box list
	addItems(typeNames);

	connect(this, SIGNAL(currentTextChanged(const QString&)),
		this, SLOT(onFileChange(const QString&)), Qt::UniqueConnection);
}

void FileTypeSelector::selectFileType( const QString& fileType ) {
	disconnect(this, SIGNAL(currentTextChanged(const QString&)),
		this, SLOT(onFileChange(const QString&)));

	int index = findText(fileType);

	// if the item is not found
	if (index < 0) {
		addItem(fileType);
		index = count() - 1;
	}

	setCurrentIndex(index);

	connect(this, SIGNAL(currentTextChanged(const QString&)),
		this, SLOT(onFileChange(const QString&)), Qt::UniqueConnection);
}

void FileTypeSelector::onFileChange( const QString& fileType ) {
	emit fileTypeSelected(fileType);
}
