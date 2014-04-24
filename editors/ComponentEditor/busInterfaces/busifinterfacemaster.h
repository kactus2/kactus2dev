/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemaster.h
 */

#ifndef BUSIFINTERFACEMASTER_H
#define BUSIFINTERFACEMASTER_H

#include "busifinterfacemodeeditor.h"
#include <common/widgets/addressSpaceSelector/addressspaceselector.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/businterface.h>

#include <QString>
#include <QSharedPointer>
#include <QLineEdit>

/*! \brief Editor to edit master or mirrored master settings of a bus interface.
 *
 */
class BusIfInterfaceMaster : public BusIfInterfaceModeEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param mode The mode of the bus interface (may be master or mirrored master).
	 * \param busif Pointer to the bus interface being edited.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfInterfaceMaster(General::InterfaceMode mode, 
		QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfInterfaceMaster();

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

	//! \brief Handler for changes in address space reference.
	void onAddressSpaceChange(const QString& addrSpaceName);

	//! \brief Handler for changes in base address.
	void onBaseAddressChange(const QString& newBase);

private:

	//! \brief No copying
	BusIfInterfaceMaster(const BusIfInterfaceMaster& other);

	//! No assignment
	BusIfInterfaceMaster& operator=(const BusIfInterfaceMaster& other);

	//! \brief Pointer to the master element being edited.
	QSharedPointer<MasterInterface> master_;
	
	//! \brief Specifies if the edited mode is master or mirrored master
	General::InterfaceMode mode_;

	//! \brief Combo box to select an address space within component
	AddressSpaceSelector addressSpaceRef_;

	//! \brief Line edit to set the base address of master's address space.
	QLineEdit baseAddress_;
};

#endif // BUSIFINTERFACEMASTER_H
