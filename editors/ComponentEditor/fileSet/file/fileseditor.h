//-----------------------------------------------------------------------------
// File: fileseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 1.6.2012
//
// Description:
// The editor to add/remove/edit files of a file set.
//-----------------------------------------------------------------------------

#ifndef FILESEDITOR_H
#define FILESEDITOR_H

#include "filesmodel.h"
#include "filesview.h"

#include <QGroupBox>
#include <QSharedPointer>

class LibraryInterface;
class Component;
class FileSet;
class File;
class FileInterface;

//-----------------------------------------------------------------------------
//! The editor to add/remove/edit files of a file set.
//-----------------------------------------------------------------------------
class FilesEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] availableFiles  List of editable files.
     *      @param [ni] fileInterface   Interface for accessing files.
	 *      @param [in] component       Pointer to the component being edited.
	 *      @param [in] handler         Pointer to the instance that manages the library.
	 *      @param [in] parent          Pointer to the owner of this editor.
	 *      @param [in] title           The title for the group box.
	 */
    FilesEditor(QSharedPointer<QList<QSharedPointer<File> > > availableFiles,
        FileInterface* fileInterface,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QWidget *parent,
        const QString& title = tr("Files"));

	/*!
     *  The destructor.
     */
	~FilesEditor();

public slots:

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

signals:

	//! Emitted when the files change.
	void contentChanged();

	//! Emitted when a file is added to the list.
	void fileAdded(int index);

	//! Emitted when a file is removed from the list.
	void fileRemoved(int index);

	//! Emitted when a file is moved from position to another
	void fileMoved(int source, int target);

    /*!
     *  Inform of a name change in a file.
     *
     *      @param [in] oldName     Old name of the file.
     *      @param [in] newName     The new name of the file.
     */
    void fileRenamed(std::string const& oldName, std::string const& newName);

private slots:

	//! Handler for addFilesButton clicked() signal.
	void onAddFiles();

private:
	
	//! No copying. No Assignment.
	FilesEditor(const FilesEditor& other);
	FilesEditor& operator=(const FilesEditor& other);

	//! Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! The view to display the file information.
	FilesView view_;

	//! The model to edit the file summary.
	FilesModel model_;

    //! Interface for accessing files.
    FileInterface* fileInterface_;

    //! The editable files.
    QSharedPointer<QList<QSharedPointer<File> > > availableFiles_;
};

#endif // FILESEDITOR_H
