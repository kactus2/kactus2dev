/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapscene.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSCENE_H
#define MEMORYMAPSCENE_H

#include <IPXACTmodels/memorymap.h>

#include <QGraphicsScene>
#include <QList>
#include <QSharedPointer>

class VisualizerItem;

/*! \brief The graphics scene that contains the memory map graphics items.
 *
 */
class MemoryMapScene : public QGraphicsScene {
	Q_OBJECT

public:

    //! \brief Scrolling sensitivity. Bigger value results in smaller steps.
    static const int WHEEL_SENSITIVITY = 6;

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the graphics scene.
	 *
	*/
	MemoryMapScene(QObject *parent);
	
	//! \brief The destructor
	virtual ~MemoryMapScene();

	/*! \brief Add a new memory map graph item to the scene.
	 *
	 * \param memGraphItem Pointer to the item.
	 *
	*/
	void addMemGraphItem(VisualizerItem* memGraphItem);

	/*! \brief Remove a memory map graph item from the scene.
	 *
	 * \param memGraphItem Pointer to the item to remove.
	 *
	*/
	void removeMemGraphItem(VisualizerItem* memGraphItem);



	/*! \brief Set the scene width.
	 *
     * \param width Width to set.
     *
	 */
    void setWidth(int width);

    public slots:
        	/*! \brief Reposition the memory map graphs items in the scene.
	 *
	*/
	void rePosition();

protected:
    
    //! \brief Resizes memory map when user turns the mouse wheel.
    void wheelEvent(QGraphicsSceneWheelEvent * wheelEvent);

private:
	
	//! \brief No copying
	MemoryMapScene(const MemoryMapScene& other);

	//! \brief No assignment
	MemoryMapScene& operator=(const MemoryMapScene& other);

	//! \brief Contains the graph items for memory maps.
	QList<VisualizerItem*> memGraphItems_;

    //! \brief Width of top (memory map) items.
    int width_;
};

#endif // MEMORYMAPSCENE_H
