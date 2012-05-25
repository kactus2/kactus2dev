/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: businterfaceeditor.h
 */

#ifndef BUSINTERFACEEDITOR_H
#define BUSINTERFACEEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include "busifgeneraltab.h"
#include "busifportmaptab.h"
#include "busifinterfacemodetab.h"
#include "busifparameterstab.h"

#include <QSharedPointer>
#include <QTabWidget>

class Component;
class BusInterface;
class LibraryInterface;

/*! \brief Container for editors to edit a bus interface.
 *
 */
class BusInterfaceEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the library handler instance.
	 * \param component Pointer to the component being edited.
	 * \param dataPointer Pointer  to the bus interface being edited.
	 * \param parent Pointer to the owner of this container.
	 *
	*/
	BusInterfaceEditor(LibraryInterface* libHandler,
		QSharedPointer<Component> component, 
		void* dataPointer,
		QWidget *parent = 0);

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the library handler instance.
	 * \param component Pointer to the component being edited.
	 * \param busif Pointer  to the bus interface being edited.
	 * \param parent Pointer to the owner of this container.
	 *
	*/
	BusInterfaceEditor(LibraryInterface* libHandler,
		QSharedPointer<Component> component, 
		QSharedPointer<BusInterface> busif,
		QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~BusInterfaceEditor();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Remove the edited bus interface from the model.
	 *
	 * This function should be used when user wants to remove an element from 
	 * the component.
	 *
	*/
	virtual void removeModel();

	/*! \brief Make the changes from the widgets editors to the IPXact model.
	 *
	 * The base class implementation does nothing.
	*/
	virtual void makeChanges();

private slots:

	//! \brief When tab page changes
	void onTabChange(int index);

private:
	
	//! \brief No copying
	BusInterfaceEditor(const BusInterfaceEditor& other);

	//! No assignment
	BusInterfaceEditor& operator=(const BusInterfaceEditor& other);

	//! \brief Set up the the editor.
	void initialize();

	//! \brief Pointer to the bus interface being edited.
	BusInterface* busif_;

	//! \brief The widget that contains the editor for bus interface.
	QTabWidget tabs_;

	//! \brief The tab for general settings of bus interface
	BusIfGeneralTab general_;

	//! \brief The tab for port maps of bus interface
	BusIfPortmapTab portmaps_;

	//! \brief The tab for interface mode of bus interface
	BusIfInterfaceModeTab interfaceMode_;

	//! \brief The tab for parameters of bus interface
	BusIfParametersTab parameters_;
};

#endif // BUSINTERFACEEDITOR_H
