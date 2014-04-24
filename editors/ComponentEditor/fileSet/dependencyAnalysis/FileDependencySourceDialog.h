//-----------------------------------------------------------------------------
// File: FileDependencySourceDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Tommi Korhonen, Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// Dialog for choosing the file dependency source directories.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYSOURCEDIALOG_H
#define FILEDEPENDENCYSOURCEDIALOG_H

#include "FileDependencySourceEditor.h"

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
//! Dialog for choosing the file dependency source directories.
//-----------------------------------------------------------------------------
class FileDependencySourceDialog : public QDialog
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
    FileDependencySourceDialog(QString const& basePath, QStringList const& sourceDirs, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~FileDependencySourceDialog();

    /*!
     *  Retrieves the new list of source directories.
     *
     *      @return The list of source directories.
     *
     *      @remarks Valid only when the user presses OK.
     */
    QStringList getSourceDirectories() const;

private:
    // Disable copying.
    FileDependencySourceDialog(FileDependencySourceDialog const& rhs);
    FileDependencySourceDialog& operator=(FileDependencySourceDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The main group box.
    //QGroupBox mainGroupBox_;

    FileDependencySourceEditor editor_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYSOURCEDIALOG_H
