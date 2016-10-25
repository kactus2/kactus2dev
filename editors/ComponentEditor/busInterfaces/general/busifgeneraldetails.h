//-----------------------------------------------------------------------------
// File: busifgeneraldetails.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 05.04.2011
//
// Description:
// Editor to edit the bus interface details.
//-----------------------------------------------------------------------------

#ifndef BUSIFGENERALDETAILS_H
#define BUSIFGENERALDETAILS_H

#include <common/widgets/interfaceModeSelector/interfacemodeselector.h>

#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSharedPointer>

class BusInterface;

//-----------------------------------------------------------------------------
//! Editor to edit the bus interface details.
//-----------------------------------------------------------------------------
class BusIfGeneralDetails : public QGroupBox
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] busif   The bus interface being edited.
	 *      @param [in] parent  The owner of this editor.
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
	void onAddressableUnitChanged();

	//! Handler for changes in the endianness.
	void onEndiannessChanged();

	//! Handler for changes in bit steering.
	void onBitSteeringChanged();

	//! Handler for changes in connection required check box
	void onConnectionRequiredChanged();

private:
	
	//! No copying
	BusIfGeneralDetails(const BusIfGeneralDetails& other);

	//! No assignment
	BusIfGeneralDetails& operator=(const BusIfGeneralDetails& other);

    //! Sets the editor layout.
    void setupLayout();

	//! The bus interface being edited.
	QSharedPointer<BusInterface> busif_;

    //! The selector to select the interface mode.
    InterfaceModeSelector modeSelector_;

	//! Set the connection required to true or false
	QCheckBox connectionRequired_;

	//! Sets the value for bits in lau.
	QLineEdit bitsInLauEditor_;

	//! Set the endianness of interface
	QComboBox endiannessSelector_;

	//! Set bit steering on/off
	QComboBox bitSteeringSelector_;
};

#endif // BUSIFGENERALDETAILS_H
