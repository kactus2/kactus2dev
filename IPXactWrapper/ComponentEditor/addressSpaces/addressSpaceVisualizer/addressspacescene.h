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

private:
	
	//! \brief No copying
	AddressSpaceScene(const AddressSpaceScene& other);

	//! \brief No assignment
	AddressSpaceScene& operator=(const AddressSpaceScene& other);

	//! \brief Pointer to the address space being visualized.
	QSharedPointer<AddressSpace> addrSpace_;
};

#endif // ADDRESSSPACESCENE_H
