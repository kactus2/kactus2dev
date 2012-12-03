/* 
 *  	Created on: 30.11.2012
 *      Author: Antti Kamppi
 * 		filename: fieldgapitem.h
 *		Project: Kactus 2
 */

#ifndef FIELDGAPITEM_H
#define FIELDGAPITEM_H

#include <IPXactWrapper/ComponentEditor/visualization/memorygapitem.h>

#include <QGraphicsItem>

class FieldGapItem : public MemoryGapItem {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the parent graphics item.
	 *
	*/
	FieldGapItem(QGraphicsItem* parent);
	
	//! \brief The destructor
	virtual ~FieldGapItem();

	//! \brief Refresh the gap item.
	virtual void refresh();

	/*! \brief Get the bit width of the item.
	 * 
	 * \return The bit width of the item.
	*/
	virtual int getBitWidth() const;

	/*! \brief Get the width of the item.
	 *
	 * \return The width of the gap item.
	*/
	virtual qreal itemTotalWidth() const;

private:
	//! \brief No copying
	FieldGapItem(const FieldGapItem& other);

	//! \brief No assignment
	FieldGapItem& operator=(const FieldGapItem& other);
};

#endif // FIELDGAPITEM_H
