//-----------------------------------------------------------------------------
// File: busifinterfaceslave.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to the slave details of a bus interface.
//-----------------------------------------------------------------------------

#ifndef BUSIFINTERFACESLAVE_H
#define BUSIFINTERFACESLAVE_H

#include "busifinterfacemodeeditor.h"
#include "bridgeseditor.h"

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/slaveinterface.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Editor to the slave details of a bus interface.
//-----------------------------------------------------------------------------
class BusIfInterfaceSlave : public BusIfInterfaceModeEditor 
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *        @param [in]   busif Pointer to the bus interface being edited.
	 *        @param [in]   component Pointer to the component being edited.
	 *        @param [in]   parent Pointer to the owner of this editor.
	 *
	*/
	BusIfInterfaceSlave(QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
		QWidget *parent);
	
	//! The destructor
	virtual ~BusIfInterfaceSlave();

	/*! Check for the validity of the edited item.
	*
	*       @return True if item is valid.
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

	//! Handler for changes in the memory map reference.
	void onMemoryMapChange(const QString& newMemoryMapName);

private:

	//! No copying
	BusIfInterfaceSlave(const BusIfInterfaceSlave& other);

	//! No assignment
	BusIfInterfaceSlave& operator=(const BusIfInterfaceSlave& other);

	//! Pointer to the slave interface mode being edited.
	QSharedPointer<SlaveInterface> slave_;

	//! The editor to select a memory map from the component.
	ReferenceSelector memoryMapReferenceSelector_;

	//! The editor to edit the bridges of a slave interface.
	BridgesEditor bridges_;
};

#endif // BUSIFINTERFACESLAVE_H
