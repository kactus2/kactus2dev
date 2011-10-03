/* 
 *
 *  Created on: 1.4.2011
 *      Author: Antti Kamppi
 * 		filename: booleancombobox.h
 */

#ifndef BOOLEANCOMBOBOX_H
#define BOOLEANCOMBOBOX_H

#include <QComboBox>

/*! \brief Combo box that has "true" or "false" choices for user to select.
 *
 */
class BooleanComboBox : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	BooleanComboBox(QWidget *parent);

	//! \brief The destructor
	virtual ~BooleanComboBox();

	/*! \brief Get the value currently selected in the combo box
	 *
	 * \return Boolean value that matches the currently selected item.
	*/
	bool getCurrentValue() const;

	/*! \brief Set the current value to be selected.
	 *
	 * \param value The value to be selected.
	 *
	*/
	void setCurrentValue(bool value);

private:

	//! \brief No copying
	BooleanComboBox(const BooleanComboBox& other);

	//! No assignment
	BooleanComboBox& operator=(const BooleanComboBox& other);
};

#endif // BOOLEANCOMBOBOX_H
