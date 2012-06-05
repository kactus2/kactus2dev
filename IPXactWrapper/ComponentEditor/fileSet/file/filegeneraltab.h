/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: filegeneraltab.h
 */

#ifndef FILEGENERALTAB_H
#define FILEGENERALTAB_H

#include <IPXactWrapper/ComponentEditor/fileSet/file/filenameeditor.h>
#include <common/widgets/attributeBox/attributebox.h>
#include <IPXactWrapper/ComponentEditor/fileSet/file/filegeneraleditor.h>
#include <IPXactWrapper/ComponentEditor/fileSet/file/filetypeeditor.h>
#include "filebuildcommand.h"

#include <models/file.h>

#include <QWidget>
#include <QFileInfo>

/*! \brief FileGeneralTab is a widget to edit File's general settings.
 *
 * This widget contains the editing elements in fileEditor's general tab page.
 */
class FileGeneralTab : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param baseLocation Refers to the containing component's xml-file location.
	 * \param file Pointer to the file that is being edited.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FileGeneralTab(const QFileInfo& baseLocation, 
		File* file,
		QWidget *parent);

	//! \brief The destructor
	virtual ~FileGeneralTab();

	/*! \brief Apply the changes from the editor to the model.
	 *
	*/
	void apply();

	/*! \brief Restore the changes from the model to the editor.
	 *
	*/
	void restore();

	/*! \brief Check that the editor is in a valid state.
	 *
	 *
	 * \return True if everything is ok. False if changes can not be applied.
	*/
	bool isValid() const;

signals:

	//! \brief Emitted when contents of one widget change.
	void contentChanged();

	//! \brief Emitted when the name of the file changes.
	void nameChanged(const QString& newName);

private:

	//! \brief No copying
	FileGeneralTab(const FileGeneralTab& other);

	//! No assignment
	FileGeneralTab& operator=(const FileGeneralTab& other);

	//! \brief Pointer to the file-model being edited.
	File* file_;

	//! \brief The widget to edit file's name and name-attributes.
	FileNameEditor nameEditor_;

	//! \brief The widget to edit file's attributes.
	//AttributeBox attributes_;

	//! \brief The editor to edit the general settings of the file
	FileGeneralEditor generalEditor_;

	//! \brief The editor to set the file types of the file.
	FileTypeEditor fileTypeEditor_;

	//! \brief Pointer to the widget that is used to edit the build command.
	FileBuildCommand buildCommand_;
	
};

#endif // FILEGENERALTAB_H
