//-----------------------------------------------------------------------------
// File: ObjectExportDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.07.2017
//
// Description:
// Dialog for selecting the exported items.
//-----------------------------------------------------------------------------

#ifndef OBJECTEXPORTDIALOG_H
#define OBJECTEXPORTDIALOG_H

#include "ObjectSelectionDialog.h"

#include <QPushButton>
#include <QLineEdit>

//-----------------------------------------------------------------------------
//! Dialog for selecting the exported items.
//-----------------------------------------------------------------------------
class ObjectExportDialog : public ObjectSelectionDialog
{
	Q_OBJECT

public:

    /*!
	 *  The Constructor.
	 *
	 *    @param [in] parent          The parent widget.
	 *    @param [in] f               Widget flags for the dialog.
	 */
	ObjectExportDialog(QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

    /*!
	 *  The destructor.
	 */
    virtual ~ObjectExportDialog();

    /*!
     *  Get the path to the destination directory.
     *
     *    @return The path to the destination directory.
     */
    QString getTargetDirectory() const;

private slots:

    /*!
     *  Open up a dialog for selecting the destination path for the export.
     */
    void onBrowseTarget();

    /*!
     *  Handles the changes in the changing of the destination path.
     *
     *    @param [in] newDestination  The new destination path.
     */
    void onDestinationFolderChanged(const QString& newDestination);

protected:

    /*!
     *  Connect the signals for folder browsing.
     */
    virtual void connectSignals();

    /*!
     *  Setup the dialog layout.
     */
    virtual void setupLayout();

private:

	//! No copying. No assignment.
    ObjectExportDialog(const ObjectExportDialog& other);
    ObjectExportDialog& operator=(const ObjectExportDialog& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Editor for the destination folder.
    QLineEdit folderPathEditor_;

    //! Button for browsing the destination folder.
    QPushButton folderBrowseButton_;

    //! The current path.
    QString currentPath_;
};

#endif // OBJECTEXPORTDIALOG_H
