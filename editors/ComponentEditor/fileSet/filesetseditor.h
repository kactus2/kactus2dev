//-----------------------------------------------------------------------------
// File: filesetseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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

class LibraryInterface;
class Component;
class ParameterFinder;
class FileSetInterface;
class FileSet;

//-----------------------------------------------------------------------------
//! The editor to add/remove/edit file sets of a component.
//-----------------------------------------------------------------------------
class FileSetsEditor : public ItemEditor
{
    Q_OBJECT

public:

    /*! The constructor
     *
     *      @param [in] component           The component being edited.
     *      @param [in] libInterface        The library interface.
     *      @param [in] parameterFinder     Finder used to identify parameters.
     *      @param [in] fileSetInterface    Interface for accessing file sets.
     */
    FileSetsEditor(QSharedPointer<Component> component,
        LibraryInterface* libInterface,
        QSharedPointer<ParameterFinder> parameterFinder,
        FileSetInterface* fileSetInterface);
    
    //! The destructor
    ~FileSetsEditor();

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

    //! Sets the widget layout.
    void setupLayout();

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

    //! Interface for accessing file sets.
    FileSetInterface* fileSetInterface_;

    //! The editable file sets.
    QSharedPointer<QList<QSharedPointer<FileSet> > > availableFileSets_;
};

#endif // FILESETSEDITOR_H
