/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemslave.h
 */

#ifndef BUSIFINTERFACEMSLAVE_H
#define BUSIFINTERFACEMSLAVE_H

#include "busifinterfacemodeeditor.h"

class BusInterface;

/*! \brief Editor to edit mirrored slave details of a bus interface.
 *
 */
class BusIfInterfaceMSlave : public BusIfInterfaceModeEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param busif Pointer to the bus interface being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfInterfaceMSlave(BusInterface* busif, QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfInterfaceMSlave();

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

	/*! \brief Get the interface mode of the editor
	 * 
	 * \return General::InterfaceMode Specifies the interface mode.
	*/
	virtual General::InterfaceMode getInterfaceMode() const;

private:
	
	//! \brief No copying
	BusIfInterfaceMSlave(const BusIfInterfaceMSlave& other);

	//! No assignment
	BusIfInterfaceMSlave& operator=(const BusIfInterfaceMSlave& other);
};

#endif // BUSIFINTERFACEMSLAVE_H
