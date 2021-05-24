//-----------------------------------------------------------------------------
// File: memorymapscene.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
#include <QVector>
#include <QSharedPointer>

class MemoryMapGraphItem;

//-----------------------------------------------------------------------------
//! The graphics scene that contains the memory map graphics items.
//-----------------------------------------------------------------------------
class MemoryMapScene : public QGraphicsScene
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of the graphics scene.
	 */
	explicit MemoryMapScene(QObject *parent);
	
	//! The destructor.
	virtual ~MemoryMapScene() = default;

    //! No copying.
    MemoryMapScene(const MemoryMapScene& other) = delete;

    //! No assignment.
    MemoryMapScene& operator=(const MemoryMapScene& other) = delete;

	/*!
     *  Add a new memory map graph item to the scene.
	 *
	 *      @param [in] memGraphItem    Pointer to the item.
	 */
	void addMemGraphItem(MemoryMapGraphItem* memGraphItem);

	/*!
     *  Remove a memory map graph item from the scene.
	 *
	 *      @param [in] memGraphItem    Pointer to the item to remove.
	 */
	void removeMemGraphItem(MemoryMapGraphItem* memGraphItem);

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
    virtual void wheelEvent(QGraphicsSceneWheelEvent * wheelEvent) override final;

private:	

	//! Contains the graph items for memory maps.
	QVector<MemoryMapGraphItem*> memGraphItems_;

    //! Width of top (memory map) items.
    int width_;
};

#endif // MEMORYMAPSCENE_H
