/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemonitor.h
 */

#ifndef BUSIFINTERFACEMONITOR_H
#define BUSIFINTERFACEMONITOR_H

#include "busifinterfacemodeeditor.h"
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <common/widgets/interfaceModeSelector/interfacemodeselector.h>

#include <QSharedPointer>
#include <QComboBox>

class BusIfGeneralTab;
class LibraryInterface;

/*! \brief Editor the edit a monitor settings of a bus interface.
 *
 */
class BusIfInterfaceMonitor : public BusIfInterfaceModeEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param busif Pointer to the bus interface being edited.
	 * \param component Pointer to the component being edited.
	 * \param generalTab Pointer to the general tab.
	 * \param libHandler Pointer to the library handler instance.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfInterfaceMonitor(QSharedPointer<BusInterface> busif, 
		QSharedPointer<Component> component,
		BusIfGeneralTab* generalTab,
		LibraryInterface* libHandler,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfInterfaceMonitor();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh();

	/*! \brief Get the interface mode of the editor
	 * 
	 * \return General::InterfaceMode Specifies the interface mode.
	*/
	virtual General::InterfaceMode getInterfaceMode() const;

	//! \brief Save the interface mode-specific details to the bus interface.
	virtual void saveModeSpecific();

private slots:

	//! \brief When user changes the interface mode of the monitor.
	void onInterfaceModeChange(General::InterfaceMode mode);

	//! \brief Handler for system group changes.
	void onSystemGroupChange(const QString& groupName);

private:
	
	//! \brief No copying
	BusIfInterfaceMonitor(const BusIfInterfaceMonitor& other);

	//! No assignment
	BusIfInterfaceMonitor& operator=(const BusIfInterfaceMonitor& other);

	//! \brief Pointer to the widget that contains the selected bus Type
	BusIfGeneralTab* generalTab_;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! \brief Pointer to the monitor element being edited.
	QSharedPointer<BusInterface::MonitorInterface> monitor_;

	//! \brief Combo box to select the interface type for monitor.
	InterfaceModeSelector interfaceMode_;

	//! \brief Combo box to select the group for system interface types.
	QComboBox systemGroup_;
};

#endif // BUSIFINTERFACEMONITOR_H
