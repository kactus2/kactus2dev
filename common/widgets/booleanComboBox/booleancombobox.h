//-----------------------------------------------------------------------------
// File: booleancombobox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 01.04.2011
//
// Description:
// Combo box that has "true", "false" or "unspecified" choices for user to select.
//-----------------------------------------------------------------------------

#ifndef BOOLEANCOMBOBOX_H
#define BOOLEANCOMBOBOX_H

#include <QComboBox>

//-----------------------------------------------------------------------------
//! Combo box that has "true", "false" or "unspecified" choices for user to select.
//-----------------------------------------------------------------------------
class BooleanComboBox : public QComboBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this widget.
	 */
	BooleanComboBox(QWidget *parent);

	//! The destructor.
	virtual ~BooleanComboBox();

	/*!
     *  Get the value currently selected in the combo box.
	 *
	 *      @return Boolean value that matches the currently selected item.
	 */
    QString getCurrentValue() const;

	/*!
     *  Set the current value to be selected.
	 *
	 *      @param [in] value   The value to be selected.
	 */
    void setCurrentValue(QString const& value);

private:

	//! No copying.
	BooleanComboBox(const BooleanComboBox& other);

	//! No assignment.
	BooleanComboBox& operator=(const BooleanComboBox& other);
};

#endif // BOOLEANCOMBOBOX_H
