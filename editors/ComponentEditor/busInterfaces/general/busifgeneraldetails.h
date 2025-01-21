//-----------------------------------------------------------------------------
// File: busifgeneraldetails.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 05.04.2011
//
// Description:
// Editor to edit the bus interface details.
//-----------------------------------------------------------------------------

#ifndef BUSIFGENERALDETAILS_H
#define BUSIFGENERALDETAILS_H

#include <common/widgets/interfaceModeSelector/interfacemodeselector.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>

#include <KactusAPI/include/ExpressionParser.h>


#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSharedPointer>

class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Editor to edit the bus interface details.
//-----------------------------------------------------------------------------
class BusIfGeneralDetails : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *    @param [in] busInterface    Interface for accessing bus interfaces.
     *    @param [in] busName         Name of the bus interface being edited.
     *    @param [in] docRevision     The IP-XACT version to comply to.
	 *    @param [in] parent          The owner of this editor.
     */
    BusIfGeneralDetails(BusInterfaceInterface* busInterface, std::string const& busName, 
		Document::Revision docRevision, QSharedPointer<ParameterFinder> finder, 
		QSharedPointer<ExpressionParser> expressionParser,
		QWidget* parent);

	/*!
     *  The destructor
     */
	virtual ~BusIfGeneralDetails() = default;

    //! No copying
    BusIfGeneralDetails(const BusIfGeneralDetails& other) = delete;

    //! No assignment
    BusIfGeneralDetails& operator=(const BusIfGeneralDetails& other) = delete;

	/*! Check for the validity of the edited item.
	*
	*    @return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh();

    /*!
     *  Change the name of the bus interface being edited.
     *
     *    @param [in] newName     The new bus interface name.
     */
    void changeName(std::string const& newName);

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

	//! Handler for changes in bit steering expression.
	void onBitSteeringExpressionChanged();

	//! Handler for changes in connection required check box
	void onConnectionRequiredChanged();

    /*!
     *  Change the colouring of bit steering combobox determined by the currently selected interface mode.
     */
    void updateBitSteeringControls(General::InterfaceMode mode);

private:
	
    //! Sets the editor layout.
    void setupLayout(Document::Revision docRevision);

	//! Interface for accesssing bus  interfaces.
    BusInterfaceInterface* busInterface_;

    //! Name of the edited bus interface.
    std::string busName_;

    //! The selector to select the interface mode.
    InterfaceModeSelector modeSelector_;

	//! Set the connection required to true or false
	QCheckBox connectionRequired_;

	//! Sets the value for bits in lau.
	ExpressionEditor bitsInLauEditor_;

	//! Set the endianness of interface
	QComboBox endiannessSelector_;

	//! Set bit steering on/off
	QComboBox bitSteeringSelector_;

	//! 
	ExpressionEditor bitSteeringEditor_;

};

#endif // BUSIFGENERALDETAILS_H
