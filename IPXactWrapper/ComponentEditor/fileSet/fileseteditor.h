/* 
 *
 *  Created on: 3.2.2011
 *      Author: Antti Kamppi
 * 		filename: fileseteditor.h
 */

#ifndef FILESETEDITOR_H
#define FILESETEDITOR_H

#include "../itemeditor.h"
#include "groupmanager.h"

#include <models/fileset.h>
#include <common/widgets/nameGroupBox/namegroupbox.h>
#include <common/widgets/listManager/dirlistmanager.h>

#include <QWidget>
#include <QFileInfo>
#include <QSharedPointer>
#include <QString>

/*! \brief FileSetEditor is a widget to edit the details of a FileSet.
 *
 */
class FileSetEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param baseLocation Refers to the location in file system that is used
	 * as base to create relative file paths.
	 * \param component Pointer to the component model that is being edited.
	 * \param dataPointer Pointer to the FileSet that is being edited
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FileSetEditor(const QFileInfo& baseLocation, 
		QSharedPointer<Component> component, 
		void* dataPointer,
		QWidget *parent);

	/*! \brief The constructor
	 *
	 * \param baseLocation The file path used as base for the relative paths.
	 * \param component Pointer to the component model that is being edited.
	 * \param dataPointer Pointer to the FileSet that is being edited
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FileSetEditor(const QString& baseLocation, 
		QSharedPointer<Component> component, 
		FileSet* fileSet,
		QWidget *parent);

	//! \brief The destructor.
	virtual ~FileSetEditor();

	/*! \brief Check for the validity of the edited item.
	*
	* Base class implementation always returns true.
	* 
	* \return True is item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Remove the FileSet from the component
	 *
	 * This function should be used when user wants to remove a file set from 
	 * the component.
	 *
	*/
	virtual void removeModel();

	/*! \brief Make the changes from the widgets editors to the IPXact model.
	 *
	 * The base class implementation does nothing.
	*/
	virtual void makeChanges();

private:

	//! No copying
	FileSetEditor(const FileSetEditor& other);

	//! No assignment
	FileSetEditor& operator=(const FileSetEditor& other);

	//! \brief Initialize the editor
	void initialize();

	//! \brief Refers to the location of the base xml-file.
	QFileInfo baseLocation_;

	//! \brief The pointer to the edited item.
	FileSet* fileSet_;

	//! \brief Used to manage the name, displayName and description of FileSet.
	NameGroupBox nameBox_;

	//! \brief Used to manage groups of FileSet.
	GroupManager groups_;

	//! \brief Used to manage dependencies of the FileSet.
	DirListManager dependencies_;
	
};

#endif // FILESETEDITOR_H
