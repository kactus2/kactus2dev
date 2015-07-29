//-----------------------------------------------------------------------------
// File: filesetseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.5.2012
//
// Description:
// The editor to add/remove/edit file sets of a component.
//-----------------------------------------------------------------------------

#ifndef FILESETSEDITOR_H
#define FILESETSEDITOR_H

#include "filesetsmodel.h"
#include "dependencyAnalysis/FileDependencyEditor.h"

#include <editors/ComponentEditor/itemeditor.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <QSortFilterProxyModel>
#include <QSplitter>

class PluginManager;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! The editor to add/remove/edit file sets of a component.
//-----------------------------------------------------------------------------
class FileSetsEditor : public ItemEditor
{
	Q_OBJECT

public:

	//! The default width of the name column.
	static const int NAME_COLUMN_WIDTH = 200;

	//! The default width of the description column.
	static const int DESC_COLUMN_WIDTH = 300;

	/*! The constructor
	 *
	 *      @param [in] component           The component being edited.
     *      @param [in] libraryInterface    The library interface.
     *      @param [in] pluginMgr           The plugin manager.    
	 */
	FileSetsEditor(QSharedPointer<Component> component, LibraryInterface* libInterface, PluginManager& pluginMgr);
	
	//! The destructor
	~FileSetsEditor();

	/*! Check for the validity of the editor
	* 
	*       @return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	//! Reload the information from the model to the editor.
	virtual void refresh();

signals:
    //! Emitted when a file has been added to any file set.
    void fileAdded(File* file);

    //! Emitted when the files have been updated.
    void filesUpdated();

    //! Emitted when the file dependencies have changed.
    void dependenciesChanged();

public slots:
    /*!
     *  Refreshes the dependency model.
     */
    void refreshDependencyModel();

protected:
	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying
	FileSetsEditor(const FileSetsEditor& other);
	FileSetsEditor& operator=(const FileSetsEditor& other);

    //! Splitter for the fileset table and dependency graph.
    QSplitter splitter_;
    
	//! The view to display the file sets.
	EditableTableView view_;

	//! The model that manages the file set objects for the view.
	FileSetsModel model_;

	//! The proxy to do the sorting.
	QSortFilterProxyModel proxy_;

    //! The file dependency editor.
    FileDependencyEditor dependencyEditor_;

    //! If true, the widget has not yet been shown at all.
    bool firstShow_;
};

#endif // FILESETSEDITOR_H
