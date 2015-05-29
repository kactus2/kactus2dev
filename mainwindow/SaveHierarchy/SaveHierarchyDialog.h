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

private:
    // Disable copying.
    SaveHierarchyDialog(SaveHierarchyDialog const& rhs);
    SaveHierarchyDialog& operator=(SaveHierarchyDialog const& rhs);

    //! Sets the dialog layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! View for selecting and editing the documents to save.
    QTreeWidget* documentSelectionView_;

    //! Selector for save location within libraries.
    LibrarySelectorWidget* directoryEditor_;

    //! The building strategy for the document tree.
    HierarchicalSaveBuildStrategy documentSelectionBuilder_;
};

#endif // SAVEHIERARCHYDIALOG_H
