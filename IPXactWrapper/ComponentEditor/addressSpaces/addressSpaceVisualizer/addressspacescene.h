/* 
 *  	Created on: 22.12.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacescene.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACESCENE_H
#define ADDRESSSPACESCENE_H

#include <models/addressspace.h>

#include <QGraphicsScene>
#include <QSharedPointer>
#include <QMultiMap>

class AddressSpaceVisualizationItem;

/*! \brief The graphics scene containing the segments and local memory map of an address space.
 *
 */
class AddressSpaceScene : public QGraphicsScene {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param addrSpace Pointer to the address space being visualized.
	 * \param parent Pointer to the owner of the scene.
	 *
	*/
	AddressSpaceScene(QSharedPointer<AddressSpace> addrSpace, 
		QObject *parent);
	
	//! \brief The destructor
	virtual ~AddressSpaceScene();

	/*! \brief Refresh the address space visualization.
	 *
	*/
	virtual void refresh();

	/*! \brief Reposition the items on the visualization.
	 * 
	 * The items are repositioned based on the offsets and new items are not 
	 * created or old removed.
	 *
	*/
	virtual void rePosition();

private:
	
	//! \brief No copying
	AddressSpaceScene(const AddressSpaceScene& other);

	//! \brief No assignment
	AddressSpaceScene& operator=(const AddressSpaceScene& other);

	//! \brief Update the offsets of the segments and gaps
	void updateSegments();

	//! \brief Pointer to the address space being visualized.
	QSharedPointer<AddressSpace> addrSpace_;

	//! \brief Contains the segments and segments gaps ordered by offsets
	QMultiMap<quint64, AddressSpaceVisualizationItem*> segmentItems_;
};

#endif // ADDRESSSPACESCENE_H
