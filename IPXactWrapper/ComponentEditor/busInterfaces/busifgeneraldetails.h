/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifgeneraldetails.h
 */

#ifndef BUSIFGENERALDETAILS_H
#define BUSIFGENERALDETAILS_H

#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>

class BusInterface;

/*! \brief Editor to edit the bus interface details.
 *
 */
class BusIfGeneralDetails : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param dataPointer Pointer to the bus interface being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfGeneralDetails(void* dataPointer, QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfGeneralDetails();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void restoreChanges();

	/*! \brief Applies the changes made with the editor to the model.
	*
	* After calling this function it is no longer possible to automatically 
	* restore the previous state of the model.
	* 
	* Note: if the editor is not in valid state nothing is changed.
	*/
	virtual void applyChanges();

public slots:

	/*! \brief Enable/disable the bitSteering element.
	 *
	 * \param checked If true then bitSteering checkbox is enabled.
	 *
	*/
	void onBitSteeringState(bool checked);

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private:
	
	//! \brief No copying
	BusIfGeneralDetails(const BusIfGeneralDetails& other);

	//! No assignment
	BusIfGeneralDetails& operator=(const BusIfGeneralDetails& other);

	//! \brief Pointer to the bus interface being edited.
	BusInterface* busif_;

	//! \brief Set the connection required to true or false
	QCheckBox connRequired_;

	//! \brief Sets the value for bits in lau.
	QSpinBox bitsInLau_;

	//! \brief Set the endianness of interface
	QComboBox endianness_;

	//! \brief Set bit steering on/off
	QComboBox bitSteering_;

	//! \brief Enable/disable the bit steering element.
	QCheckBox bitSteeringEnabled_;
};

#endif // BUSIFGENERALDETAILS_H
