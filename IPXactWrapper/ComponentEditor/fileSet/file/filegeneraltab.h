/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: filegeneraltab.h
 */

#ifndef FILEGENERALTAB_H
#define FILEGENERALTAB_H

#include "filenameeditor.h"
#include <common/widgets/attributeBox/attributebox.h>
#include "filegeneraleditor.h"
#include "filetypeeditor.h"
#include "filebuildcommand.h"

#include <models/component.h>
#include <models/file.h>

#include <QWidget>
#include <QFileInfo>
#include <QSharedPointer>

class LibraryInterface;

/*! \brief FileGeneralTab is a widget to edit File's general settings.
 *
 * This widget contains the editing elements in fileEditor's general tab page.
 */
class FileGeneralTab : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param handler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param file Pointer to the file that is being edited.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FileGeneralTab(LibraryInterface* handler,
		QSharedPointer<Component> component,
		QSharedPointer<File> file,
		QWidget *parent);

	//! \brief The destructor
	virtual ~FileGeneralTab();

	/*! \brief Restore the changes from the model to the editor.
	 *
	*/
	void refresh();

	/*! \brief Check that the editor is in a valid state.
	 *
	 *
	 * \return True if everything is ok. False if changes can not be applied.
	*/
	bool isValid() const;

signals:

	//! \brief Emitted when contents of one widget change.
	void contentChanged();

private slots:

	//! \brief Handler for file type editor's changes.
	void onFileTypesChanged();

private:

	//! \brief No copying
	FileGeneralTab(const FileGeneralTab& other);

	//! No assignment
	FileGeneralTab& operator=(const FileGeneralTab& other);

	//! \brief Pointer to the file-model being edited.
	QSharedPointer<File> file_;

	//! \brief The widget to edit file's name and name-attributes.
	FileNameEditor nameEditor_;

	//! \brief The editor to edit the general settings of the file
	FileGeneralEditor generalEditor_;

	//! \brief The editor to set the file types of the file.
	FileTypeEditor fileTypeEditor_;

	//! \brief Pointer to the widget that is used to edit the build command.
	FileBuildCommand buildCommand_;
	
};

#endif // FILEGENERALTAB_H
