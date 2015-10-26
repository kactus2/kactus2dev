/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemonitor.h
 */

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

/*! Editor the edit a monitor settings of a bus interface.
 *
 */
class BusIfInterfaceMonitor : public BusIfInterfaceModeEditor {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] busif       The bus interface being edited.
	 *      @param [in] component   The component being edited.
	 *      @param [in] libHandler  The library handler instance.
	 *      @param [in] parent      The owner of this editor.
	 *
	*/
	BusIfInterfaceMonitor(QSharedPointer<BusInterface> busif, 
		QSharedPointer<Component> component,
		LibraryInterface* libHandler,
		QWidget *parent);
	
	//! The destructor
	virtual ~BusIfInterfaceMonitor();

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

	//! When user changes the interface mode of the monitor.
	void onInterfaceModeChange(General::InterfaceMode mode);

	//! Handler for system group changes.
	void onSystemGroupChange(QString const& groupName);

private:
	
	//! No copying
	BusIfInterfaceMonitor(const BusIfInterfaceMonitor& other);

	//! No assignment
	BusIfInterfaceMonitor& operator=(const BusIfInterfaceMonitor& other);

	//! The instance that manages the library.
	LibraryInterface* libHandler_;

	//! The monitor element being edited.
	QSharedPointer<BusInterface::MonitorInterface> monitor_;

	//! Combo box to select the interface type for monitor.
	InterfaceModeSelector interfaceMode_;

	//! Combo box to select the group for system interface types.
	QComboBox systemGroup_;
};

#endif // BUSIFINTERFACEMONITOR_H
