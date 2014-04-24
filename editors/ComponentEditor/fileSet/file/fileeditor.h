/* 
 *
 *  Created on: 5.2.2011
 *      Author: Antti Kamppi
 * 		filename: fileeditor.h
 */

#ifndef FILEEDITOR_H
#define FILEEDITOR_H

#include "../../itemeditor.h"
#include "filegeneraltab.h"
#include "fileextratab.h"
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/file.h>

#include <QWidget>
#include <QFileInfo>
#include <QTabWidget>
#include <QSharedPointer>

class LibraryInterface;

/*! \brief FileEditor is a widget to edit the details of a File
 * 
 */
class FileEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param handler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param file Pointer to the file that is edited.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FileEditor(LibraryInterface* handler,
		QSharedPointer<Component> component, 
		QSharedPointer<File> file,
		QWidget *parent = 0);

	//! \brief The destructor
	virtual ~FileEditor();

	/*! \brief Check for the validity of the edited item.
	* 
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

private:

	//! No copying
	FileEditor(const FileEditor& other);

	//! No assignment
	FileEditor& operator=(const FileEditor& other);

	//! \brief The tab widget that contains the other widgets.
	QTabWidget tabs_;

	//! \brief Pointer to the widget that is used for general settings of file.
	FileGeneralTab generalTab_;

	//! \brief Pointer to the widget that is used to edit i.e external dependencies.
	FileExtraTab extraTab_;

	//! \brief Pointer to the file that is edited in this editor.
	QSharedPointer<File> file_;
};

#endif // FILEEDITOR_H
