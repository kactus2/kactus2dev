/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: filetypeeditor.h
 */

#ifndef FILETYPEEDITOR_H
#define FILETYPEEDITOR_H

#include <common/widgets/listManager/listmanager.h>
#include <IPXACTmodels/file.h>

#include <QSharedPointer>

/*! \brief FileTypeEditor is used to specify a file type for a file.
 * 
 * Widget allows user to select a predefined file type or create own file type.
 */
class FileTypeEditor : public ListManager {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget
	 * \param file Pointer to the file being edited
	 *
	*/
	FileTypeEditor(QWidget *parent,
		QSharedPointer<File> file);

	//! \brief The destructor
	virtual ~FileTypeEditor();

	/*! \brief Initialize the file type editor.
	 *
	 * This function must be called after creating the file type editor and before
	 * using it.
	 * 
	 * \param items QStringList that contains the items to add to the widget.
	 *
	*/
	virtual void initialize(const QStringList& items = QStringList());

	/*! \brief Apply the changes from the editor to the model.
	 *
	*/
	void apply();

	/*! \brief Restore the changes from the model to the editor.
	 *
	*/
	void restore();

	/*! \brief Check if the editor is in a valid state or not
	 *
	 *
	 * \return True if the editor is in valid state
	*/
	bool isValid() const;

private:

	//! \brief No copying
	FileTypeEditor(const FileTypeEditor& other);

	//! No assignment
	FileTypeEditor& operator=(const FileTypeEditor& other);

	//! \brief Pointer to the file-model being edited.
	QSharedPointer<File> file_;
	
};

#endif // FILETYPEEDITOR_H
