/* 
 *  	Created on: 23.8.2012
 *      Author: Antti Kamppi
 * 		filename: usagecombobox.h
 *		Project: Kactus 2
 */

#ifndef USAGECOMBOBOX_H
#define USAGECOMBOBOX_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QComboBox>

class UsageComboBox : public QComboBox {
	Q_OBJECT

public:

	UsageComboBox(QWidget *parent);
	
	//! \brief The destructor
	virtual ~UsageComboBox();

	/*! \brief Get the currently selected value on the combo box.
	 *
	 * \return The selected usage value.
	*/
	General::Usage getCurrentValue() const;

	/*! \brief Set the given usage value as the selected value.
	 *
	 * \param usage The value to select.
	 *
	*/
	void setCurrentValue(General::Usage usage);

private:
	
	//! \brief No copying
	UsageComboBox(const UsageComboBox& other);

	//! \brief No assignment
	UsageComboBox& operator=(const UsageComboBox& other);
};

#endif // USAGECOMBOBOX_H
