/* 
 *  	Created on: 21.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceeditor.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEEDITOR_H
#define ADDRESSSPACEEDITOR_H

#include "addressspacegeneraleditor.h"
#include "addressspaceparameterseditor.h"
#include "segmenteditor.h"

#include <models/component.h>
#include <models/addressspace.h>

#include <common/widgets/nameGroupBox/namegroupbox.h>

#include <IPXactWrapper/ComponentEditor/parameters/parameterseditor.h>

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <QSharedPointer>

/*! \brief Editor to edit and save settings of an address space within component editor.
 *
 */
class AddressSpaceEditor : public ItemEditor {
	Q_OBJECT

public:

	AddressSpaceEditor(QSharedPointer<Component> component,
		void* dataPointer,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~AddressSpaceEditor();

	/*! \brief Check for the validity of the address space.
	* 
	* \return True if address space is valid and can be saved.
	*/
	virtual bool isValid() const;

	/*! \brief Make the changes from the widgets editors to the IPXact model.
	 *
	 * The base class implementation does nothing.
	*/
	virtual void makeChanges();

	/*! \brief Remove the edited address space from the component.
	 *
	 * This function should be used when user wants to remove an address space from 
	 * the component.
	 *
	*/
	virtual void removeModel();

	/*! \brief Restore the settings from the address space to the editor fields.
	 *
	*/
	void restoreChanges();

private:

	//! \brief No copying
	AddressSpaceEditor(const AddressSpaceEditor& other);

	//! \brief No assignment
	AddressSpaceEditor& operator=(const AddressSpaceEditor& other);

	//! \brief Pointer to the address space being edited.
	AddressSpace* addrSpace_;

	//! \brief The editor to set the name, display name and description.
	NameGroupBox nameBox_;

	//! \brief The editor to set the general settings of address space.
	AddressSpaceGeneralEditor general_;

	//! \brief The editor to set the segments of the address space.
	SegmentEditor segments_;

	//! \brief Editor to set the parameters of an address space.
	AddressSpaceParametersEditor parameterEditor_;
};

#endif // ADDRESSSPACEEDITOR_H
