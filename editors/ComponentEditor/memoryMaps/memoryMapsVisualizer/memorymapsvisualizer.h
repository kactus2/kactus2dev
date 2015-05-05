//-----------------------------------------------------------------------------
// File: memorymapsvisualizer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 17.04.2012
//
// Description:
// A widget to visualize a set of memory maps.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPSVISUALIZER_H
#define MEMORYMAPSVISUALIZER_H

#include <editors/ComponentEditor/itemvisualizer.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/component.h>

#include <QWidget>
#include <QList>
#include <QSharedPointer>
#include <QSize>

class MemoryMapView;
class MemoryMapScene;
class MemoryMapGraphItem;

/*! The widget to visualize a set of memory maps.
 *
 */
class MemoryMapsVisualizer : public ItemVisualizer
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent    Pointer to the owner of the visualizer widget.
	 *
	*/
	MemoryMapsVisualizer(QWidget *parent = 0);
	
	//! The destructor
	virtual ~MemoryMapsVisualizer();

	/*! Add a new memory map graphics item to the scene.
	 *
	 *      @param [in] memMapItem    Pointer to the graphics item.
	*/
	void addMemoryMapItem(MemoryMapGraphItem* memMapItem);

	/*! Remove a memory map graphics item from the scene.
	 *
	 *      @param [in] memMapItem    Pointer to the graphics item.
	*/
	void removeMemoryMapItem(MemoryMapGraphItem* memMapItem);

	/*! The minimum size hint for the visualizer.
	 *
	 *      @return  The minimum size hint.
	*/
	virtual QSize minimumSizeHint() const;

	/*! The size hint for the visualizer.
	 *
	 *      @return The size hint.
	*/
	virtual QSize sizeHint() const;

protected:

    //! Handler for events when the widget is shown.
    virtual void showEvent(QShowEvent* event);

private:
	
	//! No copying
	MemoryMapsVisualizer(const MemoryMapsVisualizer& other);

	//! No assignment
	MemoryMapsVisualizer& operator=(const MemoryMapsVisualizer& other);

	//! The view to display the items.
	MemoryMapView* view_;

	//! The scene that contains the memory map items.
	MemoryMapScene* scene_;

    //! Contains the graph items for memory maps.
    QList<MemoryMapGraphItem*> memGraphItems_;
};

#endif // MEMORYMAPSVISUALIZER_H
