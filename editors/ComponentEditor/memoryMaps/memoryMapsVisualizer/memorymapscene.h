//-----------------------------------------------------------------------------
// File: memorymapscene.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 17.04.2012
//
// Description:
// The graphics scene that contains the memory map graphics items.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPSCENE_H
#define MEMORYMAPSCENE_H

#include <IPXACTmodels/Component/MemoryMap.h>

#include <QGraphicsScene>
#include <QList>
#include <QSharedPointer>

class VisualizerItem;

//-----------------------------------------------------------------------------
//! The graphics scene that contains the memory map graphics items.
//-----------------------------------------------------------------------------
class MemoryMapScene : public QGraphicsScene
{
	Q_OBJECT

public:

    //! Scrolling sensitivity. Bigger value results in smaller steps.
    static const int WHEEL_SENSITIVITY = 6;

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of the graphics scene.
	 */
	MemoryMapScene(QObject *parent);
	
	//! The destructor.
	virtual ~MemoryMapScene();

	/*!
     *  Add a new memory map graph item to the scene.
	 *
	 *      @param [in] memGraphItem    Pointer to the item.
	 */
	void addMemGraphItem(VisualizerItem* memGraphItem);

	/*!
     *  Remove a memory map graph item from the scene.
	 *
	 *      @param [in] memGraphItem    Pointer to the item to remove.
	 */
	void removeMemGraphItem(VisualizerItem* memGraphItem);



	/*!
     *  Set the scene width.
	 *
     *      @param [in] width   Width to set.
	 */
    void setWidth(int width);

    public slots:

    /*!
     *  Reposition the memory map graphs items in the scene.
	 */
	void rePosition();

protected:
    
    //! Resizes memory map when user turns the mouse wheel.
    void wheelEvent(QGraphicsSceneWheelEvent * wheelEvent);

private:
	
	//! No copying.
	MemoryMapScene(const MemoryMapScene& other);

	//! No assignment.
	MemoryMapScene& operator=(const MemoryMapScene& other);

	//! Contains the graph items for memory maps.
	QList<VisualizerItem*> memGraphItems_;

    //! Width of top (memory map) items.
    int width_;
};

#endif // MEMORYMAPSCENE_H
