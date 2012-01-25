//-----------------------------------------------------------------------------
// File: NewWorkspaceDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.1.2012
//
// Description:
// Dialog for creating new workspaces.
//-----------------------------------------------------------------------------

#ifndef NEWWORKSPACEDIALOG_H
#define NEWWORKSPACEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

//-----------------------------------------------------------------------------
//! NewWorkspaceDialog class
//-----------------------------------------------------------------------------
class NewWorkspaceDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor which initializes the dialog.
     *
     *      @param [in] parent   The parent widget.
     */
    NewWorkspaceDialog(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~NewWorkspaceDialog();

    /*!
     *  Returns the contents of the name field.
     */
    QString getName() const;

public slots:
    void accept();

    //! Called when the contents of the name field have changed.
    void onContentChanged();

private:
    // Disable copying.
    NewWorkspaceDialog(NewWorkspaceDialog const& rhs);
    NewWorkspaceDialog& operator=(NewWorkspaceDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Line edit for the name of the new workspace.
    QLineEdit* nameEdit_;

    //! The OK button.
    QPushButton* btnOk_;
};

//-----------------------------------------------------------------------------

#endif // NEWWORKSPACEDIALOG_H
