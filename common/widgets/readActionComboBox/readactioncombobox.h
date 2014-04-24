/* 
 *  	Created on: 27.8.2012
 *      Author: Antti Kamppi
 * 		filename: readactioncombobox.h
 *		Project: Kactus 2
 */

#ifndef READACTIONCOMBOBOX_H
#define READACTIONCOMBOBOX_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QComboBox>

/*! \brief The combo box to select one of the defined read actions.
 *
 */
class ReadActionComboBox : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of the combo box.
	 *
	*/
	ReadActionComboBox(QWidget *parent);
	
	//! \brief The destructor
	virtual ~ReadActionComboBox();

	/*! \brief Get the currently selected read action.
	 *
	 * \return The currently selected read action.
	*/
	General::ReadAction getCurrentValue() const;

	/*! \brief Set the selected read action.
	 *
	 * \param readAction The read action to select.
	 *
	*/
	void setCurrentValue(General::ReadAction readAction);

private:

	//! \brief No copying
	ReadActionComboBox(const ReadActionComboBox& other);

	//! \brief No assignment
	ReadActionComboBox& operator=(const ReadActionComboBox& other);
};

#endif // READACTIONCOMBOBOX_H
