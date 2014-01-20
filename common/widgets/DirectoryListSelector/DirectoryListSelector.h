//-----------------------------------------------------------------------------
// File: DirectoryListSelector.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 2.1.2014
//
// Description:
// Editor for selecting multiple directories.
//-----------------------------------------------------------------------------

#ifndef DIRECTORYLISTSELECTOR_H
#define DIRECTORYLISTSELECTOR_H

#include <QDialog>
#include <QStringList>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListView>
#include <QStringListModel>
#include <QFileDialog>

//-----------------------------------------------------------------------------
//! Editor for selecting multiple directories.
//-----------------------------------------------------------------------------
class DirectoryListSelector : public QWidget
{
    Q_OBJECT
public:
    /*!
     *  Constructor.
     *
     *      @param [in] basePath    The path to base directory.
     *      @param [in] initialDirs The current list of directories.
     *      @param [in] parent      The parent widget.
     */
    DirectoryListSelector(QString const& basePath = QString(), 
        QStringList const& initialDirs = QStringList(), 
        QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~DirectoryListSelector();

    /*!
     *  Sets a directory non-removable.
     *
     *      @param [in] directory   The directory to set.
     */
    void setPersistentDirectory(QString const& directory);

    /*!
     *  Sets a directory removable.
     *
     *      @param [in] directory   The directory to set.
     */
    void removePersistentDirectory(QString const& directory);

    /*!
     *  Gets the list of non-removable directories.
     *     
     *      @return List of non-removable directories.
     */
    QStringList getPersistentDirectories() const;

    /*!
     *  Retrieves the list of directories.
     *
     *      @return The list of directories.
     */
    QStringList getDirectories() const;

signals:
    
    //! Emitted when directories have been changed.
    void contentChanged();

private slots:

    /*!
     *  Adds a new directory to the list. The source directory is prompted with a folder selection dialog.
     *
     *      @remark Directories under the base directory are denoted with relative path. All other directories
     *              use absolute paths.
     */
    void addDirectory();

    /*!
     *  Removes the currently selected directory from the list.
     */
    void removeDirectory();

    
    /*!
     *  Handler for directory selection changes.
     */
    void onSelectionChanged();

private:
    // Disable copying.
    DirectoryListSelector(DirectoryListSelector const& rhs);
    DirectoryListSelector& operator=(DirectoryListSelector const& rhs);

    //! Sets the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Button for adding new directories.
    QPushButton* buttonAdd_;

    //! Button for removing directories.
    QPushButton* buttonRemove_;

    //! View for directories.
    QListView* directoryListView_;

    //! Model for directories.
    QStringListModel* directoryListModel_;

    //! The base path for directories.
    QString basePath_;

    //! List of persistent, non-removable directories.
    QStringList persistentDirectories_;
};

//-----------------------------------------------------------------------------

#endif // DIRECTORYLISTSELECTOR_H
