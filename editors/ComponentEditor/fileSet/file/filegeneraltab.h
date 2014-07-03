//-----------------------------------------------------------------------------
// File: filegeneraltab.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.2.2011
//
// Description:
// FileGeneralTab is a widget to edit File's general settings.
//-----------------------------------------------------------------------------

#ifndef FILEGENERALTAB_H
#define FILEGENERALTAB_H

#include "filenameeditor.h"
#include "filegeneraleditor.h"
#include "filetypeeditor.h"
#include "filebuildcommand.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/file.h>

#include <QWidget>
#include <QSharedPointer>

class LibraryInterface;

//-----------------------------------------------------------------------------
// Class FileGeneralTab.
//-----------------------------------------------------------------------------
class FileGeneralTab : public QWidget
{
	Q_OBJECT

public:

	/*! 
     *  The constructor
	 *
	 *      @param [in] handler     Pointer to the instance that manages the library.
	 *      @param [in] component   Pointer to the component being edited.
	 *      @param [in] file        Pointer to the file that is being edited.
	 *      @param [in] parent      Pointer to the owner of this widget.
	 */
	FileGeneralTab(LibraryInterface* handler, QSharedPointer<Component> component,
		QSharedPointer<File> file, QWidget *parent);

	//! The destructor
	virtual ~FileGeneralTab();

	//! Restore the changes from the model to the editor.
	void refresh();

	/*!
	 *  Check that the editor is in a valid state.
	 *
	 *      @return  True if everything is ok. False if changes can not be applied.
	 */
	bool isValid() const;

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

signals:

	//! Emitted when contents of one widget change.
	void contentChanged();

	//! Emitted when a help page should be changed in the context help window.
	void helpUrlRequested(QString const& url);

    //! Emitted when the file should be opened in default editor.
    void editFile();

    //! Emitted when the file should be run.
    void runFile();

private slots:

	//! Handler for file type editor's changes.
	void onFileTypesChanged();

private:

	//! No copying
	FileGeneralTab(const FileGeneralTab& other);

	//! No assignment
	FileGeneralTab& operator=(const FileGeneralTab& other);

    //! Sets the widget layout.
    void setupLayout();

	//! Pointer to the file-model being edited.
	QSharedPointer<File> file_;

	//! The widget to edit file's name and name-attributes.
	FileNameEditor nameEditor_;

    //! Button for opening the file in default editor.
    QPushButton* editButton_;

    //! Button for running the file.
    QPushButton* runButton_;

	//! The editor to edit the general settings of the file
	FileGeneralEditor generalEditor_;

	//! The editor to set the file types of the file.
	FileTypeEditor fileTypeEditor_;

	//! Pointer to the widget that is used to edit the build command.
	FileBuildCommand buildCommand_;
	
};

#endif // FILEGENERALTAB_H
