//-----------------------------------------------------------------------------
// File: DeleteWorkspaceDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.1.2012
//
// Description:
// Dialog for deleting workspaces.
//-----------------------------------------------------------------------------

#ifndef DELETEWORKSPACEDIALOG_H
#define DELETEWORKSPACEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>

//-----------------------------------------------------------------------------
//! DeleteWorkspaceDialog class
//-----------------------------------------------------------------------------
class DeleteWorkspaceDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor which initializes the dialog.
     *
     *      @param [in] parent   The parent widget.
     */
    DeleteWorkspaceDialog(QWidget* parent = 0);

    /*!
     *  Adds a workspace name to the dialog.
     *
     *      @param [in] name The name of the existing workspace.
     */
    void addWorkspaceName(QString const& name);

    /*!
     *  Destructor.
     */
    ~DeleteWorkspaceDialog();

    /*!
     *  Returns the name of the workspace to delete.
     */
    QString getName() const;

private:
    // Disable copying.
    DeleteWorkspaceDialog(DeleteWorkspaceDialog const& rhs);
    DeleteWorkspaceDialog& operator=(DeleteWorkspaceDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Combo box for selecting the workspace to delete.
    QComboBox* workspaceCombo_;

    //! The OK button.
    QPushButton* btnOk_;
};

//-----------------------------------------------------------------------------

#endif // DELETEWORKSPACEDIALOG_H
