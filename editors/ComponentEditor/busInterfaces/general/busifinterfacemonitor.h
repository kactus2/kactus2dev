//-----------------------------------------------------------------------------
// File: busIfInterfaceMonitor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor the edit a monitor settings of a bus interface.
//-----------------------------------------------------------------------------

#ifndef BUSIFINTERFACEMONITOR_H
#define BUSIFINTERFACEMONITOR_H

#include "busifinterfacemodeeditor.h"

#include <common/widgets/interfaceModeSelector/interfacemodeselector.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <QSharedPointer>
#include <QComboBox>

class Component;
class BusIfGeneralTab;
class LibraryInterface;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Editor the edit a monitor settings of a bus interface.
//-----------------------------------------------------------------------------
class BusIfInterfaceMonitor : public BusIfInterfaceModeEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] busInterface    Interface for accessing bus interfaces.
     *      @param [in] busName         Name of the edited bus interface.
	 *      @param [in] libHandler      The library handler instance.
	 *      @param [in] parent          The owner of this editor.
	 *
     */
    BusIfInterfaceMonitor(BusInterfaceInterface* busInterface,
        std::string const& busName,
        LibraryInterface* libHandler,
        QWidget *parent);

	/*!
     *  The destructor.
     */
	virtual ~BusIfInterfaceMonitor() = default;

	/*!
     *  Check for the validity of the edited item.
     *
     *      @return True if item is valid.
     */
	virtual bool isValid() const;

	/*!
     *  Restore the changes made in the editor back to ones in the model.
     */
	virtual void refresh();

	/*!
     *  Get the interface mode of the editor.
	 *
     *      @return General::InterfaceMode Specifies the interface mode.
     */
	virtual General::InterfaceMode getInterfaceMode() const;

	/*!
     *  Save the interface mode-specific details to the bus interface.
     */
	virtual void saveModeSpecific();

private slots:

	/*!
     *  Handles the monitor interface mode change.
     *
     *      @param [in] mode    New interface mode.
     */
	void onInterfaceModeChange(General::InterfaceMode mode);

	/*!
     *  Handler for monitor system group changes.
     *
     *      @param [in] groupName   New system group.
     */
	void onSystemGroupChange(QString const& groupName);

private:
	
    //! No copying. No assignment.
	BusIfInterfaceMonitor(const BusIfInterfaceMonitor& other);
	BusIfInterfaceMonitor& operator=(const BusIfInterfaceMonitor& other);

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

	//! The instance that manages the library.
	LibraryInterface* libHandler_;

	//! Combo box to select the interface type for monitor.
	InterfaceModeSelector interfaceMode_;

	//! Combo box to select the group for system interface types.
	QComboBox systemGroup_;
};

#endif // BUSIFINTERFACEMONITOR_H
