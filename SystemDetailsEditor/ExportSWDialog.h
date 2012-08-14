//-----------------------------------------------------------------------------
// File: ExportSWDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 14.8.2012
//
// Description:
// Dialog for exporting SW from system design.
//-----------------------------------------------------------------------------

#ifndef EXPORTSWDIALOG_H
#define EXPORTSWDIALOG_H

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/LibraryPathSelector/librarypathselector.h>

#include <QPushButton>
#include <QLineEdit>
#include <QRadioButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QDialog>
#include <QButtonGroup>
#include <QDialogButtonBox>

class Component;
class LineEditEx;

//-----------------------------------------------------------------------------
// Dialog for configuring settings how to switch HW for a system design.
//-----------------------------------------------------------------------------
class ExportSWDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] lh         The library interface.
     *      @param [in] parent     The parent widget.
     */
    ExportSWDialog(LibraryInterface* lh, QWidget* parent);

    /*!
     *  Destructor.
     */
    ~ExportSWDialog();

    /*!
     *  Returns the base VLNV for sysdesign and sysdesigncfg.
     */
    VLNV getVLNV() const;

    /*!
     *  Returns the selected path for new IP-XACT objects.
     */
    QString getPath() const;

private slots:
    /*!
     *  Called when the user presses OK button.
     */
    virtual void accept();

    /*!
     *  Updates the directory based on the current VLNV.
     */
    void updateDirectory();

    /*!
     *  Validates the contents of the dialog and enables/disables the OK button based on the validation result.
     */
    void validate();

private:
    // Disable copying.
    ExportSWDialog(ExportSWDialog const& rhs);
    ExportSWDialog& operator=(ExportSWDialog const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* lh_;

    //! The new HW component.
    QSharedPointer<Component> component_;

    //! Label for information text.
    QLabel* infoLabel_;

    //! VLNV editor for system component VLNV.
    VLNVEditor* vlnvEdit_;

    //! Label for the directory selector.
    QLabel* directoryLabel_;

    //! The editor to select the directory to save to. 
    LibraryPathSelector* directoryEdit_;

    //! Button box for OK and Cancel buttons.
    QDialogButtonBox* buttonBox_;

    //! The layout for the UI widgets.
    QVBoxLayout* layout_;
};

//-----------------------------------------------------------------------------

#endif // EXPORTSWDIALOG_H
