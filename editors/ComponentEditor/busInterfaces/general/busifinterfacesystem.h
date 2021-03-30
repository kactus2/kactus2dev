//-----------------------------------------------------------------------------
// File: busifinterfacesystem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to edit system or mirrored system details of a bus interface.
//-----------------------------------------------------------------------------

#ifndef BUSIFINTERFACESYSTEM_H
#define BUSIFINTERFACESYSTEM_H

#include "busifinterfacemodeeditor.h"

#include <IPXACTmodels/Component/BusInterface.h>

#include <QComboBox>
#include <QSharedPointer>

class Component;
class BusIfGeneralTab;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor to edit system or mirrored system details of a bus interface.
//-----------------------------------------------------------------------------
class BusIfInterfaceSystem : public BusIfInterfaceModeEditor
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
     */
    BusIfInterfaceSystem(BusInterfaceInterface* busInterface,
        std::string const& busName,
        LibraryInterface* libHandler,
        QWidget *parent);

	/*!
     *  The destructor.
     */
	virtual ~BusIfInterfaceSystem() = default;

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
     *  Get the interface mode of the editor
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
     *  Handler for changes in system group selector.
     *
     *      @param [in] newGroup    New system group.
     */
	void onGroupChange(QString const& newGroup);

private:

    //! No copying. No assignment.
	BusIfInterfaceSystem(const BusIfInterfaceSystem& other);
	BusIfInterfaceSystem& operator=(const BusIfInterfaceSystem& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The general tab to ask for bus type.
	BusIfGeneralTab* generalTab_;

	//! The library handling interface.
	LibraryInterface* libHandler_;

	//! Select the group this interface belongs to.
	QComboBox groupEditor_;
};

#endif // BUSIFINTERFACESYSTEM_H
