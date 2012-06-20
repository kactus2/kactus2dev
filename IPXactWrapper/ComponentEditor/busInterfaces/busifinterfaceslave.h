/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfaceslave.h
 */

#ifndef BUSIFINTERFACESLAVE_H
#define BUSIFINTERFACESLAVE_H

#include "busifinterfacemodeeditor.h"
#include <models/businterface.h>
#include <models/component.h>
#include <models/slaveinterface.h>

#include <QSharedPointer>

/*! \brief Editor to the slave details of a bus interface.
 *
 */
class BusIfInterfaceSlave : public BusIfInterfaceModeEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param busif Pointer to the bus interface being edited.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfInterfaceSlave(QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
		QWidget *parent);
	
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
	virtual void refresh();

	/*! \brief Get the interface mode of the editor
	 * 
	 * \return General::InterfaceMode Specifies the interface mode.
	*/
	virtual General::InterfaceMode getInterfaceMode() const;

	//! \brief Save the interface mode-specific details to the bus interface.
	virtual void saveModeSpecific();

private slots:

	//! \brief Handler for changes in the memory map reference.
	void onMemoryMapChange(const QString& newMemoryMapName);

private:

	//! \brief No copying
	BusIfInterfaceSlave(const BusIfInterfaceSlave& other);

	//! No assignment
	BusIfInterfaceSlave& operator=(const BusIfInterfaceSlave& other);

	//! \brief Pointer to the slace interface mode being edited.
	QSharedPointer<SlaveInterface> slave_;
};

#endif // BUSIFINTERFACESLAVE_H
