//-----------------------------------------------------------------------------
// File: usagecombobox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 23.08.2012
//
// Description:
// The combo box to select one of the defined usage types.
//-----------------------------------------------------------------------------

#ifndef USAGECOMBOBOX_H
#define USAGECOMBOBOX_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QComboBox>

//-----------------------------------------------------------------------------
//! The combo box to select one of the defined usage types.
//-----------------------------------------------------------------------------
class UsageComboBox : public QComboBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      param [in] parent   Pointer to the owner of the combo box.
	 */
	UsageComboBox(QWidget *parent);
	
	//! The destructor.
	virtual ~UsageComboBox();

	/*!
     *  Get the currently selected value on the combo box.
	 *
	 *      @return The selected usage value.
	 */
	General::Usage getCurrentValue() const;

	/*!
     *  Set the given usage value as the selected value.
	 *
	 *      @param [in] usage   The value to select.
	 */
	void setCurrentValue(General::Usage usage);

private:
	
	//! No copying.
	UsageComboBox(const UsageComboBox& other);

	//! No assignment.
	UsageComboBox& operator=(const UsageComboBox& other);
};

#endif // USAGECOMBOBOX_H
