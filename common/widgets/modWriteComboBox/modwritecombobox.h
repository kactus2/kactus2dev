/* 
 *  	Created on: 27.8.2012
 *      Author: Antti Kamppi
 * 		filename: modwritecombobox.h
 *		Project: Kactus 2
 */

#ifndef MODWRITECOMBOBOX_H
#define MODWRITECOMBOBOX_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QComboBox>

/*! \brief The combo box to select one of the defined modified write values.
 *
 */
class ModWriteComboBox : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the combo box.
	 *
	*/
	ModWriteComboBox(QWidget *parent);

	//! \brief The destructor
	virtual ~ModWriteComboBox();

	/*! \brief Get the currently selected modified write value.
	 *
	 * \return The currently selected value.
	*/
	General::ModifiedWrite getCurrentValue() const;

	/*! \brief Set the currently selected value.
	 *
	 * \param modWriteValue The value to select in the combo box.
	 *
	*/
	void setCurrentValue(General::ModifiedWrite modWriteValue);

private:
	
	//! \brief No copying
	ModWriteComboBox(const ModWriteComboBox& other);

	//! \brief No assignment
	ModWriteComboBox& operator=(const ModWriteComboBox& other);
};

#endif // MODWRITECOMBOBOX_H
