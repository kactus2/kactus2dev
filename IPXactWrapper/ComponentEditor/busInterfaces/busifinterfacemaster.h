/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemaster.h
 */

#ifndef BUSIFINTERFACEMASTER_H
#define BUSIFINTERFACEMASTER_H

#include "busifinterfacemodeeditor.h"

#include <models/generaldeclarations.h>
#include <common/widgets/attributeBox/attributebox.h>
#include <models/component.h>
#include <models/businterface.h>

#include <QComboBox>
#include <QLineEdit>
#include <QString>

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
		BusInterface* busif,
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

	//! \brief When user changes the selected value in combo box
	void onIndexChanged();

private:

	//! \brief No copying
	BusIfInterfaceMaster(const BusIfInterfaceMaster& other);

	//! No assignment
	BusIfInterfaceMaster& operator=(const BusIfInterfaceMaster& other);
	
	//! \brief Specifies if the edited mode is master or mirrored master
	General::InterfaceMode mode_;

	//! \brief Combo box to select an address space within component
	QComboBox addressSpaceRef_;

	//! \brief Line edit to set the base address of master's address space.
	QLineEdit baseAddress_;

	//! \brief Editor to set the string for the configuration.
	QLineEdit prompt_;

	//! \brief Editor to set the attributes for the base address.
	AttributeBox attributes_;

	//! \brief Pointer to the containing component.
	QSharedPointer<Component> component_;
};

#endif // BUSIFINTERFACEMASTER_H
