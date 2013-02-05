/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: businterfaceeditor.h
 */

#ifndef BUSINTERFACEEDITOR_H
#define BUSINTERFACEEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <models/businterface.h>
#include <models/component.h>
#include "busifgeneraltab.h"
#include "busifportmaptab.h"

#include <QSharedPointer>
#include <QTabWidget>

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
	 * \param busif Pointer  to the bus interface being edited.
	 * \param parent Pointer to the owner of this container.
	 *
	*/
	BusInterfaceEditor(LibraryInterface* libHandler,
		QSharedPointer<Component> component, 
		QSharedPointer<BusInterface> busif,
		QWidget* parent,
        QWidget* parentWnd);
	
	//! \brief The destructor
	virtual ~BusInterfaceEditor();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

private slots:
	//! \brief When tab page changes
	void onTabChange(int index);

private:
	
	//! \brief No copying
	BusInterfaceEditor(const BusInterfaceEditor& other);

	//! No assignment
	BusInterfaceEditor& operator=(const BusInterfaceEditor& other);

	//! \brief Pointer to the bus interface being edited.
	QSharedPointer<BusInterface> busif_;

	//! \brief The widget that contains the editor for bus interface.
	QTabWidget tabs_;

	//! \brief The tab for general settings of bus interface
	BusIfGeneralTab general_;

	//! \brief The tab for port maps of bus interface
	BusIfPortmapTab portmaps_;
};

#endif // BUSINTERFACEEDITOR_H
