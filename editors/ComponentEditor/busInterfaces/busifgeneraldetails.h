/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifgeneraldetails.h
 */

#ifndef BUSIFGENERALDETAILS_H
#define BUSIFGENERALDETAILS_H

#include <common/widgets/interfaceModeSelector/interfacemodeselector.h>

#include <IPXACTmodels/businterface.h>

#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QSharedPointer>

class BusInterface;

/*! Editor to edit the bus interface details.
 *
 */
class BusIfGeneralDetails : public QGroupBox {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] busif   Pointer to the bus interface being edited.
	 *      @param [in] parent  Pointer to the owner of this editor.
	 *
	*/
	BusIfGeneralDetails(QSharedPointer<BusInterface> busif, QWidget* parent);
	
	//! The destructor
	virtual ~BusIfGeneralDetails();

	/*! Check for the validity of the edited item.
	*
	*       @return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh();

signals:

	//! Emitted when contents of the model change
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

    //! Emitted when a new interface mode has been selected.
    void modeSelected(General::InterfaceMode);

private slots:

	//! Handler for changes in least addressable unit (bits in lau)
	void onAddressableUnitChanged(int newValue);

	//! Handler for changes in the endianness.
	void onEndiannessChanged();

	//! Handler for changes in bit steering.
	void onBitSteeringChanged();

	//! Handler for changes in connection required check box
	void onConnectionRequiredChanged();

private:
	
	//! No copying
	BusIfGeneralDetails(const BusIfGeneralDetails& other);
    void setupLayout();

	//! No assignment
	BusIfGeneralDetails& operator=(const BusIfGeneralDetails& other);

	//! Pointer to the bus interface being edited.
	QSharedPointer<BusInterface> busif_;

    //! The selector to select the interface mode.
    InterfaceModeSelector modeSelector_;

	//! Set the connection required to true or false
	QCheckBox connRequired_;

	//! Sets the value for bits in lau.
	QSpinBox bitsInLau_;

	//! Set the endianness of interface
	QComboBox endianness_;

	//! Set bit steering on/off
	QComboBox bitSteering_;

	//! Enable/disable the bit steering element.
	QCheckBox bitSteeringEnabled_;
};

#endif // BUSIFGENERALDETAILS_H
