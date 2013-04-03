/* 
 *	Created on:	3.4.2013
 *	Author:		Antti Kamppi
 *	File name:	filetypeselector.h
 *	Project:		Kactus 2
*/

#ifndef FILETYPESELECTOR_H
#define FILETYPESELECTOR_H

#include <QComboBox>

/*! \brief Combo box to select a file type.
 *
 */
class FileTypeSelector : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		FileTypeSelector
	 * Full name:	FileTypeSelector::FileTypeSelector
	 * Access:		private 
	 *
	 * \param parent Pointer to the owner of the combo box.
	 *
	*/
	FileTypeSelector(QWidget *parent);
	
	//! \brief The destructor.
	virtual ~FileTypeSelector();

	public slots:

	//! \brief Refresh the items to be selected in the combo box.
	void refresh();

	/*! \brief Set the specified file type as selected.
	 *
	 * \param fileType The name of the file type to select.
	 *
	*/
	void selectFileType(const QString& fileType);

signals:

	//! \brief Emitted when file type was selected by user.
	void fileTypeSelected(const QString& fileType);

private slots:

	//! \brief Handler for changes in combo box.
	void onFileChange(const QString& fileType);

private:
	
	//! \brief No copying
	FileTypeSelector(const FileTypeSelector& other);

	//! \brief No assignment
	FileTypeSelector& operator=(const FileTypeSelector& other);

};

#endif // FILETYPESELECTOR_H
