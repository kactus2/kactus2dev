/* 
 *	Created on: 18.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	filesavedialog.h
 * 	Project:	Kactus 2
*/
#ifndef FILESAVEDIALOG_H
#define FILESAVEDIALOG_H

#include <common/Global.h>

#include <QDialog>
#include <QTableView>
#include <QAbstractTableModel>

/*! \brief The dialog can be used for user to select multiple files to be created.
 * 
 * The dialog needs a model which is used to display the files to be created. 
 * User can change the file names and paths.
 */
class KACTUS2_API FileSaveDialog : public QDialog {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * Method: 		FileSaveDialog
	 * Full name:	FileSaveDialog::FileSaveDialog
	 * Access:		public 
	 *
	 * \param parent Pointer to the owner of the dialog.
	 *
	*/
	FileSaveDialog(QWidget *parent);

	//! \brief The destructor
	virtual ~FileSaveDialog();

	/*! \brief Set the model for the table view.
	 *
	 * Method: 		setModel
	 * Full name:	FileSaveDialog::setModel
	 * Access:		public 
	 *
	 * \param model Pointer to the model class, which contains the data to modify.
	 *
	*/
	void setModel(QAbstractTableModel* model);

private:
	
	//! \brief No copying
	FileSaveDialog(const FileSaveDialog& other);

	//! \brief No assignment
	FileSaveDialog& operator=(const FileSaveDialog& other);

	//! \brief The view displaying the files to be created.
	QTableView* view_;
};

#endif // FILESAVEDIALOG_H
