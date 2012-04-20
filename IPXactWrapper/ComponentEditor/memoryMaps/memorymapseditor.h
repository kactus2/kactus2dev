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
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>

#include <QWidget>
#include <QSharedPointer>
#include <QList>

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
	 * \param dataPointer Pointer to the list containing the memory maps.
	 * \param handler Pointer to the instance that manages the library.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	MemoryMapsEditor(QSharedPointer<Component> component,
		void* dataPointer, 
		LibraryInterface* handler,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~MemoryMapsEditor();

	/*! \brief Check for the validity of the edited memory maps.
	*
	* \return True if all memory maps are in valid state.
	*/
	virtual bool isValid() const;

private:
	
	//! \brief No copying
	MemoryMapsEditor(const MemoryMapsEditor& other);

	//! \brief No assignment
	MemoryMapsEditor& operator=(const MemoryMapsEditor& other);

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Contains pointers to the memory maps being edited.
	QList<QSharedPointer<MemoryMap> >* memoryMaps_;

	//! \brief The widget to display the contents of memory maps.
	MemoryMapsVisualizer visualizer_;
};

#endif // MEMORYMAPSEDITOR_H
