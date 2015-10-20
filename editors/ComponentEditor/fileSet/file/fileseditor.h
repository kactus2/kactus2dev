//-----------------------------------------------------------------------------
// File: fileseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
#include <QPushButton>

class LibraryInterface;
class Component;
class FileSet;
class File;

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
	 *      @param [in] component   Pointer to the component being edited.
	 *      @param [in] fileSet     Pointer to the file set being edited.
	 *      @param [in] handler     Pointer to the instance that manages the library.
	 *      @param [in] parent      Pointer to the owner of this editor.
	 *      @param [in] title       The title for the group box.
	 */
	FilesEditor(QSharedPointer<Component> component, QSharedPointer<FileSet> fileSet, LibraryInterface* handler,
        QWidget *parent, const QString& title = tr("Files"));

	//! The destructor.
	~FilesEditor();

	/*!
     *  Check for the validity of the editor.
	 * 
	 *      @return True if the editor is in valid state.
	 */
	virtual bool isValid() const;

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

	//! The button to add new files to file set.
	QPushButton addFilesButton_;
};

#endif // FILESEDITOR_H
