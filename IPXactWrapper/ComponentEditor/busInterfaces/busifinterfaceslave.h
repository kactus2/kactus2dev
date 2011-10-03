/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfaceslave.h
 */

#ifndef BUSIFINTERFACESLAVE_H
#define BUSIFINTERFACESLAVE_H

#include "busifinterfacemodeeditor.h"

class BusInterface;

/*! \brief Editor to the slave details of a bus interface.
 *
 */
class BusIfInterfaceSlave : public BusIfInterfaceModeEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param busif Pointer to the bus interface being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfInterfaceSlave(BusInterface* busif, QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfInterfaceSlave();

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
	BusIfInterfaceSlave(const BusIfInterfaceSlave& other);

	//! No assignment
	BusIfInterfaceSlave& operator=(const BusIfInterfaceSlave& other);
	
};

#endif // BUSIFINTERFACESLAVE_H
