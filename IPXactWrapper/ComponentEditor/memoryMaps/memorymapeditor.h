/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapeditor.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPEDITOR_H
#define MEMORYMAPEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <models/memorymap.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class EditableTableView;
class MemoryMapModel;

/*! \brief The editor to edit the details of a single memory map.
 *
 */
class MemoryMapEditor : public ItemEditor {
	Q_OBJECT

public:
	
	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param memoryMap Pointer to the memory map being edited.
	 * \param parent Pointer to the parent of this editor.
	 *
	*/
	MemoryMapEditor(QSharedPointer<Component> component,
		QSharedPointer<MemoryMap> memoryMap,
				QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~MemoryMapEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
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
	MemoryMapEditor(const MemoryMapEditor& other);

	//! \brief No assignment
	MemoryMapEditor& operator=(const MemoryMapEditor& other);

	//! \brief The view to show the details of a memory map.
	EditableTableView* view_;

	//! \brief The proxy that does the sorting of items.
	QSortFilterProxyModel* proxy_;

	//! \brief The model that manages the items.
	MemoryMapModel* model_;
};

#endif // MEMORYMAPEDITOR_H
