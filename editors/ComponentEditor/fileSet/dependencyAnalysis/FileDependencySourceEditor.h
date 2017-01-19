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
#include <QStringListModel>
#include <QFileDialog>

#include <common/widgets/listManager/dirlistmanager.h>

//-----------------------------------------------------------------------------
//! Editor for changing source directories.
//-----------------------------------------------------------------------------
class FileDependencySourceEditor : public DirListManager
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
    	
    /*!
     *  Initializes the editor.
	 * 
	 *      @param [in] items   QStringList that contains the items to add to the widget.
     *
     *      @remark FileDependencySourceEditor calls initialize() in constructor.
	 */
    virtual void initialize(QStringList const& items = QStringList());

private slots:

    /*!
     *  Adds a new source to the list.
     */
    void addSource();

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
     *      @return The absolute paths for the directories.
     */
    QStringList getAbsoluteDirectories();

    //! Path to start search for relative paths.
    QString basePath_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYSOURCEEDITOR_H
