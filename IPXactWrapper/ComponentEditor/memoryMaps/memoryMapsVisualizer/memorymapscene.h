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

	/*! \brief Set the memory maps to be displayed.
	 * 
	 * Any previous memory maps being displayed are removed.
	 * 
	 * \param memoryMaps QList containing pointers to the memory maps to display.
	 *
	*/
	void setMemoryMaps(QList<QSharedPointer<MemoryMap> >& memoryMaps);

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
};

#endif // MEMORYMAPSCENE_H
