/* 
 *  	Created on: 11.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapseditor.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSEDITOR_H
#define MEMORYMAPSEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <models/component.h>
#include <models/memorymap.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <QWidget>
#include <QSharedPointer>

class LibraryInterface;

/*! \brief Editor to edit the memory maps of a component.
 *
 */
class MemoryMapsEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	MemoryMapsEditor(QSharedPointer<Component> component,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~MemoryMapsEditor();

	/*! \brief Check for the validity of the edited memory maps.
	*
	* \return True if all memory maps are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

private:
	
	//! \brief No copying
	MemoryMapsEditor(const MemoryMapsEditor& other);

	//! \brief No assignment
	MemoryMapsEditor& operator=(const MemoryMapsEditor& other);

	//! \brief The view to display the table
	EditableTableView view_;
};

#endif // MEMORYMAPSEDITOR_H
