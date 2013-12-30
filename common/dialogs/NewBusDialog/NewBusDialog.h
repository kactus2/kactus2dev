//-----------------------------------------------------------------------------
// File: NewBusDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Dialog for new bus VLNV selection and bus abstraction generation settings.
//-----------------------------------------------------------------------------

#ifndef NewBusDialog_H
#define NewBusDialog_H

#include <QDialog>
#include <QRadioButton>

#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <IPXactWrapper/BusEditor/buseditor.h>
#include <models/businterface.h>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Dialog for selecting new bus VLNV and bus signal name source selection.
//-----------------------------------------------------------------------------
class NewBusDialog : public NewObjectDialog
{
public:

    enum Signal_selection
    {
        USE_NAME = 0,
        USE_DESCRIPTION
    };

    /*!
    *  Constructor.
    *
    *      @param [in] lh     The library interface.
    *      @param [in] parent The parent wizard.
    */
    NewBusDialog(LibraryInterface* lh, QWidget* parent);

    /*!
    *  Destructor.
    */
    ~NewBusDialog();
  
    /*!
     *  Gets the selected source for logical signal name genertaion.
     *
     *      @return The selected source that is either name or description.
     */
    Signal_selection getSignalSelection() const;


private:
    // Disable copying.
    NewBusDialog(NewBusDialog const& rhs);
    NewBusDialog& operator=(NewBusDialog const& rhs);

    //! Sets the dialog layout.
    void setupLayout();
   
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Button for name selection.
    QRadioButton nameSelection_;

    //! Button for description selection.
    QRadioButton descriptionSelection_;

};

#endif // NewBusDialog_H
