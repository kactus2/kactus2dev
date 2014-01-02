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
//! Editor for selecting directories.
//-----------------------------------------------------------------------------
class DirectoryListSelector : public QGroupBox
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
};

//-----------------------------------------------------------------------------

#endif // DIRECTORYLISTSELECTOR_H
