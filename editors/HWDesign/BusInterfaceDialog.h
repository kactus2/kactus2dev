//-----------------------------------------------------------------------------
// File: BusInterfaceDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 30.5.2011
//
// Description:
// Interface mode selection dialog.
//-----------------------------------------------------------------------------

#ifndef SELECTINTERFACEMODEDIALOG_H
#define SELECTINTERFACEMODEDIALOG_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>

//-----------------------------------------------------------------------------
//! BusInterfaceDialog class.
//-----------------------------------------------------------------------------
class BusInterfaceDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent          The parent widget.
     */
    BusInterfaceDialog(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~BusInterfaceDialog();
    
    /*!
     *  Adds a mode to the possible alternatives.
     *
     *      @param [in] mode The interface mode to add.
     */
    void addMode(General::InterfaceMode mode);

    /*!
     *  Returns the selected mode.
     */
    General::InterfaceMode getSelectedMode() const;


private:
    // Disable copying.
    BusInterfaceDialog(BusInterfaceDialog const& rhs);
    BusInterfaceDialog& operator=(BusInterfaceDialog const& rhs);

    /*!
     *  Sets dialog layout for interface selection.
     *
     */
    void setupLayout();


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The group box for the mode selection.
    QGroupBox* modeGroup_;

    //! The OK button.
    QPushButton* btnOK_;

    //! The Cancel button.
    QPushButton* btnCancel_;

    //! The mode radio buttons.
    QRadioButton* modeRadioButtons_[General::MONITOR + 1];

};

//-----------------------------------------------------------------------------

#endif // SELECTINTERFACEMODEDIALOG_H
