/* 
 *  	Created on: 29.5.2012
 *      Author: Antti Kamppi
 * 		filename: selectdirectorydialog.h
 *		Project: Kactus 2
 */

#ifndef SELECTDIRECTORYDIALOG_H
#define SELECTDIRECTORYDIALOG_H

#include <QFileDialog>

/*! \brief Dialog for user to select a directory in the file system.
 *
 */
class SelectDirectoryDialog : public QFileDialog {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the dialog.
	 *
	*/
	SelectDirectoryDialog(QWidget *parent);
	
	//! \brief The destructor
	~SelectDirectoryDialog();

private:

	//! \brief No copying
	SelectDirectoryDialog(const SelectDirectoryDialog& other);

	//! \brief No assignment
	SelectDirectoryDialog& operator=(const SelectDirectoryDialog& other);
};

#endif // SELECTDIRECTORYDIALOG_H
