//-----------------------------------------------------------------------------
// File: BusInterfaceDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.5.2011
//
// Description:
// Interface mode selection dialog.
//-----------------------------------------------------------------------------

#ifndef SELECTINTERFACEMODEDIALOG_H
#define SELECTINTERFACEMODEDIALOG_H

#include <models/generaldeclarations.h>

#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QLineEdit>
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
     *      @param [in] enableNameEdit  Enables the name editing in the dialog.
     *      @param [in] parent          The parent widget.
     */
    BusInterfaceDialog(bool enableNameEdit, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~BusInterfaceDialog();
    
    /*!
     *  Sets the interface name in the dialog.
     *
     *      @param [in] name The name to set.
     */
    void setName(QString const& name);

    /*!
     *  Adds a mode to the possible alternatives.
     *
     *      @param [in] mode The interface mode to add.
     */
    void addMode(General::InterfaceMode mode);

    /*!
     *  Returns the interface name.
     */
    QString getName() const;

    /*!
     *  Returns the selected mode.
     */
    General::InterfaceMode getSelectedMode() const;

public slots:
    //! Called when the value of the name field has been changed.
    void onNameChanged();

private:
    // Disable copying.
    BusInterfaceDialog(BusInterfaceDialog const& rhs);
    BusInterfaceDialog& operator=(BusInterfaceDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The layout for the UI widgets.
    QGridLayout* layout_;

    //! The line edit for the name.
    QLineEdit* nameEdit_;

    //! The group box for the mode selection.
    QGroupBox* modeGroup_;

    //! The OK button.
    QPushButton* btnOK_;

    //! The mode radio buttons.
    QRadioButton* modeRadioButtons_[General::MONITOR + 1];
};

//-----------------------------------------------------------------------------

#endif // SELECTINTERFACEMODEDIALOG_H
