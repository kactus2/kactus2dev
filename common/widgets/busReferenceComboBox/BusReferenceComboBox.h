//-----------------------------------------------------------------------------
// File: BusReferenceComboBox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.02.2022
//
// Description:
// The combo box to select one of the defined bus interfaces.
//-----------------------------------------------------------------------------

#ifndef BUSREFERENCECOMBOBOX_H
#define BUSREFERENCECOMBOBOX_H

#include <QComboBox>

#include <IPXACTmodels/generaldeclarations.h>

class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! The combo box to select one of the defined access types.
//-----------------------------------------------------------------------------
class BusReferenceComboBox: public QComboBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] busInterface    Interface for accessing bus interfaces.
     *      @param [in] parent          Pointer to the owner of the combo box.
	 */
	BusReferenceComboBox(BusInterfaceInterface* busInterface, QWidget *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~BusReferenceComboBox() = default;

    /*!
     *  Setup the combo box.
     *
     *      @param [in] currentValue    Currently selected bus interface.
     *      @param [in] busMode         Filtering bus mode for available bus interfaces.
     */
    void setupSelection(QString const& currentValue = "",
        General::InterfaceMode busMode = General::INTERFACE_MODE_COUNT);

private:
	
    //! No copying.	No assignment.
    BusReferenceComboBox(const BusReferenceComboBox& other);
    BusReferenceComboBox& operator=(const BusReferenceComboBox& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;
};

#endif // BUSREFERENCECOMBOBOX_H
