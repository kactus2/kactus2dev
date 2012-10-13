/* 
 *  	Created on: 21.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceeditor.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEEDITOR_H
#define ADDRESSSPACEEDITOR_H

#include "addressspacegeneraleditor.h"
#include "segmenteditor.h"
#include "addressspacevisualizer.h"
#include "localMemoryMap/localmemorymapeditor.h"

#include <models/component.h>
#include <models/addressspace.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/ParameterGroupBox/parametergroupbox.h>

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <QSharedPointer>

/*! \brief Editor to edit and save settings of an address space within component editor.
 *
 */
class AddressSpaceEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param addrSpace Pointer to the address space being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	AddressSpaceEditor(QSharedPointer<Component> component,
		QSharedPointer<AddressSpace> addrSpace,
		QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~AddressSpaceEditor();

	/*! \brief Check for the validity of the address space.
	* 
	* \return True if address space is valid and can be saved.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! \brief No copying
	AddressSpaceEditor(const AddressSpaceEditor& other);

	//! \brief No assignment
	AddressSpaceEditor& operator=(const AddressSpaceEditor& other);

	//! \brief Pointer to the address space being edited.
	QSharedPointer<AddressSpace> addrSpace_;

	//! \brief The editor to set the name, display name and description.
	NameGroupEditor nameEditor_;

	//! \brief The editor to set the general settings of address space.
	AddressSpaceGeneralEditor general_;

	//! \brief The editor to set the segments of the address space.
	SegmentEditor segments_;

	//! \brief The editor to set the local memory map.
	LocalMemoryMapEditor localMemMap_;

	//! \brief The editor for the parameters of the address space.
	//ParameterGroupBox parameterEditor_;

	//! \brief The widget to visualize the address space.
	AddressSpaceVisualizer visualizer_;
};

#endif // ADDRESSSPACEEDITOR_H
