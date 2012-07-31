//-----------------------------------------------------------------------------
// File: SwitchHWDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.7.2012
//
// Description:
// Dialog for configuring settings how to switch HW for a system design.
//-----------------------------------------------------------------------------

#ifndef SWITCHHWDIALOG_H
#define SWITCHHWDIALOG_H

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QPushButton>
#include <QLineEdit>
#include <QRadioButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QDialog>
#include <QButtonGroup>

//-----------------------------------------------------------------------------
// Dialog for configuring settings how to switch HW for a system design.
//-----------------------------------------------------------------------------
class SwitchHWDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    SwitchHWDialog(LibraryInterface* lh, QWidget* parent);

    /*!
     *  Destructor.
     */
    ~SwitchHWDialog();

private slots:
    /*!
     *  Called when the user presses OK button.
     */
    virtual void accept();

    /*!
     *  Called when the action selection has changed.
     *
     *      @param [in] button The action button.
     */
    void actionChanged(QAbstractButton* button);

private:
    // Disable copying.
    SwitchHWDialog(SwitchHWDialog const& rhs);
    SwitchHWDialog& operator=(SwitchHWDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Label for information text.
    QLabel infoLabel_;

    //! Label for system view name.
    QLabel viewNameLabel_;

    //! Edit box for system view name.
    QLineEdit viewNameEdit_;

    //! Group box for radio buttons.
    QGroupBox actionGroupBox_;

    //! Radio button group.
    QButtonGroup actionGroup_;

    //! Radio button for move action.
    QRadioButton moveRadioButton_;

    //! Radio button for copy action.
    QRadioButton copyRadioButton_;

    //! VLNV editor for design VLNV (shown in case of copy action).
    VLNVEditor designVlnvEdit_;

    //! The layout for the UI widgets.
    QVBoxLayout layout_;
};

//-----------------------------------------------------------------------------

#endif // SWITCHHWDIALOG_H
