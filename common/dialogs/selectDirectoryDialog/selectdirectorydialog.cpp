/* 
 *  	Created on: 29.5.2012
 *      Author: Antti Kamppi
 * 		filename: selectdirectorydialog.cpp
 *		Project: Kactus 2
 */

#include "selectdirectorydialog.h"

SelectDirectoryDialog::SelectDirectoryDialog(QWidget *parent):
QFileDialog(parent, tr("Select a directory")) {

	setFileMode(QFileDialog::Directory);
	setOption(QFileDialog::ShowDirsOnly);
}

SelectDirectoryDialog::~SelectDirectoryDialog() {
}
