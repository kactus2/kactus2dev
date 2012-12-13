/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsvisualizer.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSVISUALIZER_H
#define MEMORYMAPSVISUALIZER_H

#include <IPXactWrapper/ComponentEditor/itemvisualizer.h>
#include <models/memorymap.h>
#include <models/component.h>

#include <QWidget>
#include <QList>
#include <QSharedPointer>
#include <QSize>

class MemoryMapView;
class MemoryMapScene;
class MemoryMapGraphItem;

/*! \brief The widget to visualize a set of memory maps.
 *
 */
class MemoryMapsVisualizer : public ItemVisualizer {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component which contains the memory maps.
	 * \param parent Pointer to the owner of the visualizer widget.
	 *
	*/
	MemoryMapsVisualizer(QSharedPointer<Component> component, QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~MemoryMapsVisualizer();

	/*! \brief Add a new memory map graphics item to the scene.
	 *
	 * \param memMapItem Pointer to the graphics item.
	 *
	*/
	void addMemoryMapItem(MemoryMapGraphItem* memMapItem);

	/*! \brief Remove a memory map graphics item from the scene.
	 *
	 * \param memMapItem Pointer to the graphics item.
	 *
	*/
	void removeMemoryMapItem(MemoryMapGraphItem* memMapItem);

	/*! \brief The minimum size hint for the visualizer.
	 *
	 * \return QSize contains the minimum size hint.
	*/
	virtual QSize minimumSizeHint() const;

	/*! \brief The size hint for the visualizer.
	 *
	 * \return QSize contains the size hint.
	*/
	virtual QSize sizeHint() const;

private:
	
	//! \brief No copying
	MemoryMapsVisualizer(const MemoryMapsVisualizer& other);

	//! \brief No assignment
	MemoryMapsVisualizer& operator=(const MemoryMapsVisualizer& other);

	//! \brief The view to display the items.
	MemoryMapView* view_;

	//! \brief The scene that contains the memory map items.
	MemoryMapScene* scene_;
};

#endif // MEMORYMAPSVISUALIZER_H
