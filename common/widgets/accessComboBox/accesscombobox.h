//-----------------------------------------------------------------------------
// File: accesscombobox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 23.08.2012
//
// Description:
// The combo box to select one of the defined access types.
//-----------------------------------------------------------------------------

#ifndef ACCESSCOMBOBOX_H
#define ACCESSCOMBOBOX_H

#include <IPXACTmodels/common/AccessTypes.h>

#include <QComboBox>

//-----------------------------------------------------------------------------
//! The combo box to select one of the defined access types.
//-----------------------------------------------------------------------------
class AccessComboBox : public QComboBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      param [in] parent   Pointer to the owner of the combo box.
	 */
	AccessComboBox(QWidget *parent);
	
	//! The destructor.
	virtual ~AccessComboBox();

	/*!
     *  Get the currently selected access value.
	 *
	 *      @return The selected value.
	 */
	AccessTypes::Access getCurrentValue() const;

	/*!
     *  Set the value to be currently selected.
	 *
	 *      @param [in] access  The value to select.
	 */
	void setCurrentValue(AccessTypes::Access access);

private:
	
	//! No copying.
	AccessComboBox(const AccessComboBox& other);

	//! No assignment.
	AccessComboBox& operator=(const AccessComboBox& other);
};

#endif // ACCESSCOMBOBOX_H
