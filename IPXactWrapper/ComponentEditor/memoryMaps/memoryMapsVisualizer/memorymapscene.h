/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapscene.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSCENE_H
#define MEMORYMAPSCENE_H

#include <models/memorymap.h>

#include <QGraphicsScene>
#include <QList>
#include <QSharedPointer>

class MemoryGapItem;
class MemoryMapGraphItem;

/*! \brief The graphics scene that contains the memory map graphics items.
 *
 */
class MemoryMapScene : public QGraphicsScene {
	Q_OBJECT

public:

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
	void addMemGraphItem(MemoryMapGraphItem* memGraphItem);

	/*! \brief Remove a memory map graph item from the scene.
	 *
	 * \param memGraphItem Pointer to the item to remove.
	 *
	*/
	void removeMemGraphItem(MemoryMapGraphItem* memGraphItem);

	/*! \brief Reposition the memory map graphs items in the scene.
	 *
	*/
	void rePosition();

signals:

	//! \brief This signal is emitted when the contents of the scene changes.
	void contentChanged();

private:
	
	//! \brief No copying
	MemoryMapScene(const MemoryMapScene& other);

	//! \brief No assignment
	MemoryMapScene& operator=(const MemoryMapScene& other);

	//! \brief Contains the memory maps that are shown.
	QList<QSharedPointer<MemoryMap> > memoryMaps_;

	//! \brief Contains the graph items for memory maps.
	QList<MemoryMapGraphItem*> memGraphItems_;
};

#endif // MEMORYMAPSCENE_H
