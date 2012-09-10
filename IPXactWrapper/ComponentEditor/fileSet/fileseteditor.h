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
#include "file/fileseditor.h"
#include <models/fileset.h>
#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/listManager/dirlistmanager.h>
#include <IPXactWrapper/ComponentEditor/fileBuilders/filebuilderseditor.h>

#include <QWidget>
#include <QFileInfo>
#include <QSharedPointer>
#include <QString>

class LibraryInterface;

/*! \brief FileSetEditor is a widget to edit the details of a FileSet.
 *
 */
class FileSetEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param handler Pointer to the instance that manages the library.
	 * \param component Pointer to the component model that is being edited.
	 * \param dataPointer Pointer to the FileSet that is being edited
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FileSetEditor(LibraryInterface* handler,
		QSharedPointer<Component> component, 
		QSharedPointer<FileSet> fileSet,
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

	//! \brief Reload the information from the model to the editor.
	virtual void refresh();

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	/*! \brief Handler for changes in the group manager.
	 *
	*/
	void onGroupsChange();

	//! \brief Handler for changes in the dependency manager
	void onDependenciesChange();

private:

	//! No copying
	FileSetEditor(const FileSetEditor& other);

	//! No assignment
	FileSetEditor& operator=(const FileSetEditor& other);

	//! \brief Refers to the location of the base xml-file.
	QFileInfo baseLocation_;

	//! \brief The pointer to the edited item.
	QSharedPointer<FileSet> fileSet_;

	//! \brief Used to edit the name, display name and description.
	NameGroupEditor nameEditor_;

	//! \brief Used to manage groups of FileSet.
	GroupManager groups_;

	//! \brief Used to edit the default file builders of file set.
	FileBuildersEditor fileBuilders_;

	//! \brief The editor to add/remove/edit the files of file set.
	FilesEditor files_;

	//! \brief Used to manage dependencies of the FileSet.
	DirListManager dependencies_;
	
};

#endif // FILESETEDITOR_H
