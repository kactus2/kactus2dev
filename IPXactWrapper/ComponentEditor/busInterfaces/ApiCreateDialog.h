//-----------------------------------------------------------------------------
// File: ApiCreateDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.8.2011
//
// Description:
// API creation dialog.
//-----------------------------------------------------------------------------

#ifndef APICREATEDIALOG_H
#define APICREATEDIALOG_H

#include <QDialog>
#include <QLineEdit>

class VLNV;

//-----------------------------------------------------------------------------
//! ApiCreateDialog class.
//-----------------------------------------------------------------------------
class ApiCreateDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] vlnv    The VLNV of the API definition to be created.
     *      @param [in] parent  The parent widget.
     */
    ApiCreateDialog(VLNV const& vlnv, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~ApiCreateDialog();

    /*!
     *  Returns the selected directory.
     */
    QString getDirectory() const;

public slots:
    /*!
     *  Asks the user to select a directory.
     */
    void selectDirectory();

private:
    // Disable copying.
    ApiCreateDialog(ApiCreateDialog const& rhs);
    ApiCreateDialog& operator=(ApiCreateDialog const& rhs);

    /*!
     *  Creates a directory path based on the given VLNV.
     */
    QString createPath(VLNV const& vlnv);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Directory line edit.
    QLineEdit* directoryEdit_;
};

//-----------------------------------------------------------------------------

#endif // APICREATEDIALOG_H
