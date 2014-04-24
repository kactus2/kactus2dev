/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifgeneraldetails.h
 */

#ifndef BUSIFGENERALDETAILS_H
#define BUSIFGENERALDETAILS_H

#include <IPXACTmodels/businterface.h>

#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QSharedPointer>

class BusInterface;

/*! \brief Editor to edit the bus interface details.
 *
 */
class BusIfGeneralDetails : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param busif Pointer to the bus interface being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfGeneralDetails(QSharedPointer<BusInterface> busif,
		QWidget* parent);
	
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
	virtual void refresh();

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private slots:

	//! \brief Handler for changes in least addressable unit (bits in lau)
	void onAddressableUnitChanged(int newValue);

	//! \brief Handler for changes in the endianness.
	void onEndiannessChanged();

	//! \brief Handler for changes in bit steering.
	void onBitSteeringChanged();

	//! \brief Handler for changes in connection required check box
	void onConnectionRequiredChanged();

private:
	
	//! \brief No copying
	BusIfGeneralDetails(const BusIfGeneralDetails& other);

	//! No assignment
	BusIfGeneralDetails& operator=(const BusIfGeneralDetails& other);

	//! \brief Pointer to the bus interface being edited.
	QSharedPointer<BusInterface> busif_;

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
