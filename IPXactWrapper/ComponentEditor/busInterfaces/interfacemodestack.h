/* 
 *  	Created on: 20.6.2012
 *      Author: Antti Kamppi
 * 		filename: interfacemodestack.h
 *		Project: Kactus 2
 */

#ifndef INTERFACEMODESTACK_H
#define INTERFACEMODESTACK_H

#include "busifinterfacemaster.h"
#include "busifinterfacemonitor.h"
#include "busifinterfacemslave.h"
#include "busifinterfaceslave.h"
#include "busifinterfacesystem.h"
#include <models/component.h>
#include <models/businterface.h>
#include <models/generaldeclarations.h>

#include <QStackedWidget>
#include <QSharedPointer>

class LibraryInterface;
class BusIfGeneralTab;

/*! \brief This widget stack contains the different editors to edit interface mode specific details of bus interface.
 *
 */
class InterfaceModeStack : public QStackedWidget {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param busif Pointer to the bus interface being edited.
	 * \param component Pointer to the component being edited.
	 * \param handler Pointer to the instance that manages the library.
	 * \param parent Pointer to the owner of the stack.
	 *
	*/
	InterfaceModeStack(QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
		LibraryInterface* handler,
		BusIfGeneralTab* parent);
	
	//! \brief The destructor
	virtual ~InterfaceModeStack();

	//! \brief Refresh the contents of the stack's editors.
	void refresh();

public slots:

	/*! \brief Select the correct editor for the given interface mode.
	 *
	 * \param mode The mode for the bus interface.
	 *
	*/
	void setMode(General::InterfaceMode mode);

signals:

	//! \brief Emitted when the state of one of the editors change.
	void contentChanged();

private:
	
	//! \brief No copying
	InterfaceModeStack(const InterfaceModeStack& other);

	//! \brief No assignment
	InterfaceModeStack& operator=(const InterfaceModeStack& other);

	//! \brief Pointer to the bus interface being edited.
	QSharedPointer<BusInterface> busif_;

	//! \brief Pointer to the owner of this stack.
	BusIfGeneralTab* parent_;

	//! \brief The current mode of the interface.
	General::InterfaceMode mode_;

	BusIfInterfaceMaster master_;
	BusIfInterfaceSlave slave_;
	BusIfInterfaceSystem system_;
	BusIfInterfaceMaster mirroredMaster_;
	BusIfInterfaceMSlave mirroredSlave_;
	BusIfInterfaceSystem mirroredSystem_;
	BusIfInterfaceMonitor monitor_;
};

#endif // INTERFACEMODESTACK_H
