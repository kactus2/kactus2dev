//-----------------------------------------------------------------------------
// File: ImportEndpointsDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.1.2012
//
// Description:
// Dialog for selecting from which component to import endpoints.
//-----------------------------------------------------------------------------

#ifndef IMPORTENDPOINTSDIALOG_H
#define IMPORTENDPOINTSDIALOG_H

#include <QDialog>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>

#include <common/widgets/vlnvEditor/vlnveditor.h>

//-----------------------------------------------------------------------------
//! ImportEndpointsDialog class.
//-----------------------------------------------------------------------------
class ImportEndpointsDialog : public QDialog
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! ImportOption enumeration.
    //-----------------------------------------------------------------------------
    enum ImportOption
    {
        IMPORT_SYNCHRONIZE = 0,     //!< All endpoints are synchronized.
        IMPORT_KEEP_EXISTING        //!< Existing endpoints that are not part of the source component will be kept.
    };

    /*!
     *  Constructor.
     *
     *      @param [in] lh         The library interface.
     *      @param [in] parentWnd  The parent window.
     *      @param [in] parent     The parent widget. Can be null.
     */
    ImportEndpointsDialog(LibraryInterface* lh, QWidget* parentWnd, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~ImportEndpointsDialog();

    /*!
     *  Returns the source VLNV.
     */
    VLNV getSourceVLNV() const;

    /*!
     *  Returns the selected import option.
     */
    ImportOption getImportOption() const;

public slots:
    /*!
     *  Called when the source VLNV has been edited.
     */
    void onVlnvEdited();

private:
    // Disable copying.
    ImportEndpointsDialog(ImportEndpointsDialog const& rhs);
    ImportEndpointsDialog& operator=(ImportEndpointsDialog const& rhs);

    enum
    {
        IMPORT_OPTION_COUNT = 2
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! VLNV editor for selecting the source component.
    VLNVEditor sourceVLNV_;

    //! Import options group box.
    QGroupBox importOptionsGroup_;

    //! Import options radio buttons.
    QRadioButton importOptions_[IMPORT_OPTION_COUNT];

    //! OK button.
    QPushButton okButton_;

    //! Cancel button.
    QPushButton cancelButton_;
};

//-----------------------------------------------------------------------------

#endif // IMPORTENDPOINTSDIALOG_H
