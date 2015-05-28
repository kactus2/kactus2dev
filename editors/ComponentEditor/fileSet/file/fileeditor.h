/* 
 *
 *  Created on: 5.2.2011
 *      Author: Antti Kamppi
 * 		filename: fileeditor.h
 */

#ifndef FILEEDITOR_H
#define FILEEDITOR_H

#include "../../itemeditor.h"

#include "filenameeditor.h"
#include "filegeneraleditor.h"
#include "filetypeeditor.h"
#include "filebuildcommand.h"

#include <common/widgets/listManager/dirlistmanager.h>
#include <common/widgets/listManager/listmanager.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/file.h>

#include <QWidget>
#include <QFileInfo>
#include <QTabWidget>
#include <QSharedPointer>

class LibraryInterface;

//-----------------------------------------------------------------------------
// Editor for the details of a file.
//-----------------------------------------------------------------------------
class FileEditor : public ItemEditor
{
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

signals:

    //! Emitted when the file should be opened in default editor.
    void editFile();

    //! Emitted when the file should be run.
    void runFile();

protected:

    /*!
     *  Shows the editor and emits a request for the help file.
     *
     *      @param [in] event   The show event.
     */
    virtual void showEvent(QShowEvent* event);

private slots:

    /*!
     *  Changes the file types in the current file.
     */
    void onFileTypesChanged();

    /*!
     *  Changes the dependencies of the current file.
     */
    void onDependenciesChanged();

    /*!
     *  Changes the exported names of the current file.
     */
    void onExportedNamesChanged();

    /*!
     *  Changes the image types of the current file.
     */
    void onImageTypesChanged();

private:

	//! No copying
	FileEditor(const FileEditor& other);

	//! No assignment
	FileEditor& operator=(const FileEditor& other);

    /*!
     *  Setups the layout for the editor.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the file that is edited in this editor.
	QSharedPointer<File> file_;

    //! The editor for the file name and description.
    FileNameEditor nameEditor_;

    //! The button for editing the current file.
    QPushButton* editButton_;

    //! The button for running the current file.
    QPushButton* runButton_;

    //! Editor for the files general settings.
    FileGeneralEditor generalEditor_;

    //! Editor for the file types of a file.
    FileTypeEditor fileTypeEditor_;

    //! Editor for the build commands of a file.
    FileBuildCommand buildCommand_;

    //! Editor for the dependencies of a file.
    DirListManager dependenciesEditor_;

    //! Editor for the exported names of a file.
    ListManager exportedNamesEditor_;

    //! Editor for the image types of a file.
    ListManager imageTypesEditor_;
};

#endif // FILEEDITOR_H
