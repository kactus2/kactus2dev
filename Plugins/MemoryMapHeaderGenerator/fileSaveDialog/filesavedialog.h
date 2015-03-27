/* 
 *	Created on: 18.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	filesavedialog.h
 * 	Project:	Kactus 2
*/
#ifndef FILESAVEDIALOG_H
#define FILESAVEDIALOG_H

#include "filesavedialogview.h"

#include <QDialog>
#include <QAbstractTableModel>
#include <QSize>
#include <QStyledItemDelegate>

/*! \brief The dialog can be used for user to select multiple files to be created.
 * 
 * The dialog needs a model which is used to display the files to be created. 
 * User can change the file names and paths.
 */
class FileSaveDialog : public QDialog
{
	Q_OBJECT

public:

	//! \brief Contains the height and width of the dialog.
	enum Sizes {
		WIDTH = 600,
		HEIGHT = 300
	};

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

	/*! \brief Get the size hint for the dialog.
	 *
	 * Method: 		sizeHint
	 * Full name:	FileSaveDialog::sizeHint
	 * Access:		virtual public 
	 *
	 * 
	 * \return QSize contains the preferred size of the dialog.
	*/
	virtual QSize sizeHint() const;

	/*! \brief Get the minimum size hint for the dialog.
	 *
	 * Method: 		minimumSizeHint
	 * Full name:	FileSaveDialog::minimumSizeHint
	 * Access:		virtual public 
	 *
	 *
	 * \return QSize contains the minimum size of the dialog.
	*/
	virtual QSize minimumSizeHint() const;

	//! \brief Set the delegate class for the view.
	void setItemDelegate(QStyledItemDelegate* delegate);

private:
	
	//! \brief No copying
	FileSaveDialog(const FileSaveDialog& other);

	//! \brief No assignment
	FileSaveDialog& operator=(const FileSaveDialog& other);

	//! \brief The view displaying the files to be created.
	FileSaveDialogView* view_;
};

#endif // FILESAVEDIALOG_H
