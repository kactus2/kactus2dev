/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacesystem.h
 */

#ifndef BUSIFINTERFACESYSTEM_H
#define BUSIFINTERFACESYSTEM_H

#include "busifinterfacemodeeditor.h"
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QComboBox>
#include <QSharedPointer>

class BusIfGeneralTab;
class LibraryInterface;

/*! \brief Editor to edit system or mirrored system details of a bus interface.
 *
 */
class BusIfInterfaceSystem : public BusIfInterfaceModeEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param mode The interface mode of the editor (may be system or mirrored system)
	 * \param generalTab Pointer to the general tab of bus interface editor.
	 * \param libHandler Pointer to the library handler instance.
	 * \param busif Pointer to the bus interface being edited.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BusIfInterfaceSystem(General::InterfaceMode mode,
		BusIfGeneralTab* generalTab,
		LibraryInterface* libHandler,
		QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfInterfaceSystem();

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

	//! \brief Handler for changes in group selector.
	void onGroupChange(const QString& newGroup);

private:

	//! \brief No copying
	BusIfInterfaceSystem(const BusIfInterfaceSystem& other);

	//! No assignment
	BusIfInterfaceSystem& operator=(const BusIfInterfaceSystem& other);

	//! \brief Specifies if the edited mode is system or mirrored system
	General::InterfaceMode mode_;

	//! \brief Pointer to the monitor interface mode being edited.
	QSharedPointer<BusInterface::MonitorInterface> monitor_;

	//! \brief Pointer to the general tab to ask for bus type.
	BusIfGeneralTab* generalTab_;

	//! \brief Pointer to the library handling interface.
	LibraryInterface* libHandler_;

	//! \brief Select the group this interface belongs to.
	QComboBox group_;
};

#endif // BUSIFINTERFACESYSTEM_H
