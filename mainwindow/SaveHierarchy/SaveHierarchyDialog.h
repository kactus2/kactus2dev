//-----------------------------------------------------------------------------
// File: SaveHierarchyDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.05.2015
//
// Description:
// Dialog for hierarchical save setup.
//-----------------------------------------------------------------------------

#ifndef SAVEHIERARCHYDIALOG_H
#define SAVEHIERARCHYDIALOG_H

#include <QDialog>

#include <QObject>
#include <QProgressBar>
#include <QTreeWidget>

#include "HierarchicalSaveBuildStrategy.h"

//-----------------------------------------------------------------------------
//! Dialog for hierarchical save setup.
//-----------------------------------------------------------------------------
class SaveHierarchyDialog : public QDialog
{
    Q_OBJECT
public:
    //! The constructor.
    SaveHierarchyDialog(QObject* rootObject, LibraryInterface* library, QWidget *parent);

    //! The destructor.
    ~SaveHierarchyDialog();

public slots:

    //! Called when the save button is clicked.
    virtual void accept();

private slots:

    //! Called when saving to current directories is selected.
    void onSetSaveModeToCurrent(bool enabled);

    //! Called when saving to new root directory is selected.
    void onSetSaveModeToCommonRoot(bool enabled);

    //! Called when saving to a single directory is selected.
    void onSetSaveModeToSingleDirectory(bool enabled);

    //! Called when an item has been saved.
    void onItemSaved();

private:
    // Disable copying.
    SaveHierarchyDialog(SaveHierarchyDialog const& rhs);
    SaveHierarchyDialog& operator=(SaveHierarchyDialog const& rhs);

    
    /*!
     *  Finds the number of checked items under a given tree item.
     *
     *      @param [in] item   The item to start the count from.
     *
     *      @return The number of checked items under the item including the item itself.
     */
    int findNumberOfCheckedItems(QTreeWidgetItem* item);

    //! Sets the dialog layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! View for selecting and editing the documents to save.
    QTreeWidget* documentSelectionView_;

    //! Progress bar for showing save progression.
    QProgressBar* saveProgressBar_;

    //! Selector for save location within libraries.
    LibrarySelectorWidget* directoryEditor_;

    //! The building strategy for the document tree.
    HierarchicalSaveBuildStrategy documentSelectionBuilder_;
};

#endif // SAVEHIERARCHYDIALOG_H
