/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacesystem.h
 */

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

	/*! The constructor
	 *
	 *      @param [in] mode        The interface mode of the editor (may be system or mirrored system)
	 *      @param [in] libHandler  The library handler instance.
	 *      @param [in] busif       The bus interface being edited.
	 *      @param [in] component   The component being edited.
	 *      @param [in] parent      The owner of this editor.
	 *
	*/
	BusIfInterfaceSystem(General::InterfaceMode mode,
		LibraryInterface* libHandler,
		QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
		QWidget *parent);
	
	//! The destructor
	virtual ~BusIfInterfaceSystem();

	/*! Check for the validity of the edited item.
	*
	*      @return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh();

	/*! Get the interface mode of the editor
	 * 
	 *      @return General::InterfaceMode Specifies the interface mode.
	*/
	virtual General::InterfaceMode getInterfaceMode() const;

	//! Save the interface mode-specific details to the bus interface.
	virtual void saveModeSpecific();

private slots:

	//! Handler for changes in group selector.
	void onGroupChange(QString const& newGroup);

private:

	//! No copying
	BusIfInterfaceSystem(const BusIfInterfaceSystem& other);

	//! No assignment
	BusIfInterfaceSystem& operator=(const BusIfInterfaceSystem& other);

	//! Specifies if the edited mode is system or mirrored system
	General::InterfaceMode mode_;

	//! The monitor interface mode being edited.
	QSharedPointer<BusInterface::MonitorInterface> monitor_;

	//! The general tab to ask for bus type.
	BusIfGeneralTab* generalTab_;

	//! The library handling interface.
	LibraryInterface* libHandler_;

	//! Select the group this interface belongs to.
	QComboBox groupEditor_;
};

#endif // BUSIFINTERFACESYSTEM_H
