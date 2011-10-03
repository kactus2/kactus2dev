/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemonitor.h
 */

#ifndef BUSIFINTERFACEMONITOR_H
#define BUSIFINTERFACEMONITOR_H

#include "busifinterfacemodeeditor.h"

#include <QComboBox>

class BusInterface;
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
	 * \param generalTab Pointer to the general tab.
	 * \param libHandler Pointer to the library handler instance.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfInterfaceMonitor(BusInterface* busif, 
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
	virtual void restoreChanges();

	/*! \brief Applies the changes made with the editor to the model.
	*
	* After calling this function it is no longer possible to automatically 
	* restore the previous state of the model.
	* 
	* Note: if the editor is not in valid state nothing is changed.
	*/
	virtual void applyChanges();

	/*! \brief Update the display for the editor
	 *
	 * This function should be called when the editor is displayed to the user.
	*/
	virtual void updateDisplay();

	/*! \brief Get the interface mode of the editor
	 * 
	 * \return General::InterfaceMode Specifies the interface mode.
	*/
	virtual General::InterfaceMode getInterfaceMode() const;

private slots:

	//! \brief When user changes the interface mode of the monitor.
	void onInterfaceModeChange(int index);

private:
	
	//! \brief No copying
	BusIfInterfaceMonitor(const BusIfInterfaceMonitor& other);

	//! No assignment
	BusIfInterfaceMonitor& operator=(const BusIfInterfaceMonitor& other);

	//! \brief Pointer to the widget that contains the selected bus Type
	BusIfGeneralTab* generalTab_;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! \brief Combo box to select the interface type for monitor.
	QComboBox interfaceMode_;

	//! \brief Combo box to select the group for system interface types.
	QComboBox systemGroup_;
};

#endif // BUSIFINTERFACEMONITOR_H
