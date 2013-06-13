/* 
 *  	Created on: 25.5.2012
 *      Author: Antti Kamppi
 * 		filename: filesetseditor.h
 *		Project: Kactus 2
 */

#ifndef FILESETSEDITOR_H
#define FILESETSEDITOR_H

#include "filesetsmodel.h"
#include "dependencyAnalysis/FileDependencyEditor.h"

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <QSortFilterProxyModel>
#include <QSplitter>

class PluginManager;
class LibraryInterface;

/*! \brief The editor to add/remove/edit file sets of a component.
 *
 */
class FileSetsEditor : public ItemEditor {
	Q_OBJECT

public:

	//! \brief The default width of the name column.
	static const int NAME_COLUMN_WIDTH = 200;

	//! \brief The default width of the description column.
	static const int DESC_COLUMN_WIDTH = 300;

	/*! \brief The constructor
	 *
	 * \param component        Pointer to the component being edited.
     * \param libraryInterface The library interface.
     * \param pluginMgr        The plugin manager.    
	 */
	FileSetsEditor(QSharedPointer<Component> component,
                   LibraryInterface* libInterface,
                   PluginManager& pluginMgr);
	
	//! \brief The destructor
	~FileSetsEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Make the changes from the editor to the IPXact model.
	 *
	*/
	virtual void makeChanges();

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

signals:
    //! Emitted when a file has been added to any file set.
    void fileAdded(File* file);

    //! Emitted when the files have been updated.
    void filesUpdated();

    void dependenciesChanged();

public slots:
    /*!
     *  Refreshes the dependency model.
     */
    void refreshDependencyModel();

private slots:
    void updateFileSetView();

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! \brief No copying
	FileSetsEditor(const FileSetsEditor& other);

	//! \brief No assignment
	FileSetsEditor& operator=(const FileSetsEditor& other);

    //! Splitter for the fileset table and dependency graph.
    QSplitter splitter_;
    
	//! \brief The view to display the file sets.
	EditableTableView view_;

	//! \brief The model that manages the file set objects for the view.
	FileSetsModel model_;

	//! \brief The proxy to do the sorting
	QSortFilterProxyModel proxy_;

    //! The file dependency editor.
    FileDependencyEditor dependencyEditor_;

    //! If true, the widget has not yet been shown at all.
    bool firstShow_;
};

#endif // FILESETSEDITOR_H
