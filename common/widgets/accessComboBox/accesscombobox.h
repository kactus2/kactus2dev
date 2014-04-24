/* 
 *  	Created on: 23.8.2012
 *      Author: Antti Kamppi
 * 		filename: accesscombobox.h
 *		Project: Kactus 2
 */

#ifndef ACCESSCOMBOBOX_H
#define ACCESSCOMBOBOX_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QComboBox>

/*! \brief The combo box to select one of the defined access types.
 *
 */
class AccessComboBox : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the combo box.
	 *
	*/
	AccessComboBox(QWidget *parent);
	
	//! \brief The destructor
	virtual ~AccessComboBox();

	/*! \brief Get the currently selected access value.
	 *
	 * \return The selected value.
	*/
	General::Access getCurrentValue() const;

	/*! \brief Set the value to be currently selected.
	 *
	 * \param access The value to select.
	 *
	*/
	void setCurrentValue(General::Access access);

private:
	
	//! \brief No copying
	AccessComboBox(const AccessComboBox& other);

	//! \brief No assignment
	AccessComboBox& operator=(const AccessComboBox& other);
};

#endif // ACCESSCOMBOBOX_H
