//-----------------------------------------------------------------------------
// File: FileDependencySourceEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Editor for changing source directories.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYSOURCEEDITOR_H
#define FILEDEPENDENCYSOURCEEDITOR_H

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
//! Editor for changing source directories.
//-----------------------------------------------------------------------------
class FileDependencySourceEditor : public QGroupBox
{
    Q_OBJECT
public:
    /*!
     *  Constructor.
     *
     *      @param [in] xmlPath     The directory path where the parent component is located.
     *                              Source directories are saved relative to this path.
     *      @param [in] sourceDirs  The current list of source directories.
     *      @param [in] parent      The parent widget.
     */
    FileDependencySourceEditor(QString const& basePath, QStringList const& sourceDirs, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~FileDependencySourceEditor();

    /*!
     *  Retrieves the new list of source directories.
     *
     *      @return The list of source directories.
     */
    QStringList getSourceDirectories() const;

private slots:
    /*!
     *  Adds a new source to the list. The source directory is prompted with a folder selection dialog.
     */
    void addSource();

    /*!
     *  Removes the currently selected source from the list.
     */
    void removeSource();

private:
    // Disable copying.
    FileDependencySourceEditor(FileDependencySourceEditor const& rhs);
    FileDependencySourceEditor& operator=(FileDependencySourceEditor const& rhs);

    // Help functions.
    bool checkIfSelectedDirectoryHasBeenPreviouslyAdded(QString const& newDirectory);
    
    void removeUnnecessaryDirectories(QString const& newDirectory);

    /*!
     *  Returns the currently selected directories in absolute paths.
     *
     *      @param [out] dirs The absolute paths for the directories.
     */
    void getAbsoluteDirectories(QStringList& dirs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    QPushButton* buttonAdd_;
    QPushButton* buttonRemove_;
    QVBoxLayout* verizontalMainLayout_;
    QHBoxLayout* horizontalGroupBoxLayout_;
    QListView* directoryListView_;
    QStringListModel* directoryListModel_;
    QString basePath_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYSOURCEEDITOR_H
