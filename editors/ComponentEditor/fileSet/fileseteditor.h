//-----------------------------------------------------------------------------
// File: fileseteditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 03.02.2011
//
// Description:
// FileSetEditor is an editor used to edit the details of a FileSet.
//-----------------------------------------------------------------------------

#ifndef FILESETEDITOR_H
#define FILESETEDITOR_H

#include "groupmanager.h"

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/fileSet/file/fileseditor.h>
#include <editors/ComponentEditor/fileBuilders/filebuilderseditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/listManager/dirlistmanager.h>

#include <QWidget>
#include <QFileInfo>
#include <QSharedPointer>
#include <QString>

class LibraryInterface;
class FileSet;
class ExpressionParser;
class ExpressionFormatter;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! FileSetEditor is a widget to edit the details of a FileSet.
//-----------------------------------------------------------------------------
class FileSetEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] handler                 Pointer to the instance that manages the library.
	 *      @param [in] component               Pointer to the component model that is being edited.
     *      @param [in] parameterFinder         Finder used to identify parameters.
     *      @param [in] expressionParser        Parser used to calculate expressions.
     *      @param [in] expressionFormatter     Formatter used to format expressions.
	 *      @param [in] dataPointer             Pointer to the FileSet that is being edited
	 *      @param [in] parent                  Pointer to the owner of this widget.
	 */
    FileSetEditor(LibraryInterface* handler, QSharedPointer<Component> component, QSharedPointer<FileSet> fileSet,
        QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget *parent);

	//! The destructor.
	virtual ~FileSetEditor();

	//! Reload the information from the model to the editor.
	virtual void refresh();

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private slots:

	/*!
     *  Handler for changes in the group manager.
	 */
	void onGroupsChange();

	//! Handler for changes in the dependency manager.
	void onDependenciesChange();

private:

	//! No copying. No assignment.
	FileSetEditor(const FileSetEditor& other);
	FileSetEditor& operator=(const FileSetEditor& other);

    /*!
     *  Setup the layout for file set editor.
     */
    void setupLayout();

    /*!
     *  Connect the signals.
     */
    void connectSignals();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Refers to the location of the base xml-file.
	QFileInfo baseLocation_;

	//! The pointer to the edited item.
	QSharedPointer<FileSet> fileSet_;

	//! Used to edit the name, display name and description.
	NameGroupEditor nameEditor_;

	//! Used to manage groups of FileSet.
	GroupManager groupsEditor_;

	//! Used to edit the default file builders of file set.
	FileBuildersEditor fileBuilderEditor_;

	//! The editor to add/remove/edit the files of file set.
	FilesEditor files_;

	//! Used to manage dependencies of the FileSet.
	DirListManager dependencies_;
};

#endif // FILESETEDITOR_H
