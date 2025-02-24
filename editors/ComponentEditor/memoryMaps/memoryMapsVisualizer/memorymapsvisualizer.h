//-----------------------------------------------------------------------------
// File: memorymapsvisualizer.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 17.04.2012
//
// Description:
// A widget to visualize a set of memory maps.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPSVISUALIZER_H
#define MEMORYMAPSVISUALIZER_H

#include <editors/ComponentEditor/itemvisualizer.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMap.h>

#include <QWidget>
#include <QList>
#include <QSharedPointer>
#include <QSize>

class MemoryMapView;
class MemoryMapScene;
class MemoryMapGraphItem;

//-----------------------------------------------------------------------------
//! The widget to visualize a set of memory maps.
//-----------------------------------------------------------------------------
class MemoryMapsVisualizer : public ItemVisualizer
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *    @param [in] parent    Pointer to the owner of the visualizer widget.
	 *
	*/
	explicit MemoryMapsVisualizer(QWidget *parent = 0);
	
	//! The destructor
	virtual ~MemoryMapsVisualizer() = default;

    //! No copying
    MemoryMapsVisualizer(const MemoryMapsVisualizer& other) = delete;

    //! No assignment
    MemoryMapsVisualizer& operator=(const MemoryMapsVisualizer& other) = delete;

	/*! Add a new memory map graphics item to the scene.
	 *
	 *    @param [in] memMapItem    Pointer to the graphics item.
	*/
	void addMemoryMapItem(MemoryMapGraphItem* memMapItem);

	/*! Remove a memory map graphics item from the scene.
	 *
	 *    @param [in] memMapItem    Pointer to the graphics item.
	*/
	void removeMemoryMapItem(MemoryMapGraphItem* memMapItem);

	/*! The minimum size hint for the visualizer.
	 *
	 *    @return  The minimum size hint.
	*/
	virtual QSize minimumSizeHint() const;

	/*! The size hint for the visualizer.
	 *
	 *    @return The size hint.
	*/
	virtual QSize sizeHint() const;

public slots:

    //! Handler for events requiring a redo of the layout.
    void redoLayout();

protected:

    //! Handler for events when the widget is shown.
    virtual void showEvent(QShowEvent* event);

private:
	

	//! The view to display the items.
	MemoryMapView* view_;

	//! The scene that contains the memory map items.
	MemoryMapScene* scene_;

    //! Contains the graph items for memory maps.
    QList<MemoryMapGraphItem*> memGraphItems_;
};

#endif // MEMORYMAPSVISUALIZER_H
