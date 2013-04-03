/* 
 *	Created on:	3.4.2013
 *	Author:		Antti Kamppi
 *	File name:	filetypeselector.cpp
 *	Project:		Kactus 2
*/

#include "filetypeselector.h"

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

	// add the file type choices to the combo box
	QStringList comboItems;
	comboItems.append("asmSource");
	comboItems.append("cSource");
	comboItems.append("cppSource");
	comboItems.append("eSource");
	comboItems.append("OVASource");
	comboItems.append("perlSource");
	comboItems.append("pslSource");
	comboItems.append("SVASource");
	comboItems.append("tclSource");
	comboItems.append("veraSource");
	comboItems.append("systemCSource");
	comboItems.append("systemCSource-2.0");
	comboItems.append("systemCSource-2.0.1");
	comboItems.append("systemCSource-2.1");
	comboItems.append("systemCSource-2.2");
	comboItems.append("systemVerilogSource");
	comboItems.append("systemVerilogSource-3.0");
	comboItems.append("systemVerilogSource-3.1");
	comboItems.append("systemVerilogSource-3.1a");
	comboItems.append("verilogSource");
	comboItems.append("verilogSource-95");
	comboItems.append("verilogSource-2001");
	comboItems.append("vhdlSource");
	comboItems.append("vhdlSource-87");
	comboItems.append("vhdlSource-93");

	comboItems.append("swObject");
	comboItems.append("swObjectLibrary");

	comboItems.append("vhdlBinaryLibrary");
	comboItems.append("verilogBinaryLibrary");

	comboItems.append("executableHdl");
	comboItems.append("unelaboratedHdl");

	comboItems.append("SDC");

	comboItems.append("unknown");

	addItems(comboItems);

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
