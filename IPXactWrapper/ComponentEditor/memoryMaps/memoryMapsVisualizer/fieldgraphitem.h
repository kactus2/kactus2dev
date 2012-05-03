/* 
 *  	Created on: 25.4.2012
 *      Author: Antti Kamppi
 * 		filename: fieldgraphitem.h
 *		Project: Kactus 2
 */

#ifndef FIELDGRAPHITEM_H
#define FIELDGRAPHITEM_H

#include "memoryvisualizeritem.h"
#include <models/field.h>

#include <QSharedPointer>

/*! \brief The graphical item that represents a bit field within a register.
 *
 */
class FieldGraphItem : public MemoryVisualizerItem {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param field Pointer to the field to visualize.
	 * \param parent Pointer to the parent graph item.
	 * \param maxWidth The maximum width of the field.
	 *
	*/
	FieldGraphItem(QSharedPointer<Field> field, 
		QGraphicsItem* parent,
		int maxWidth);
	
	/*! \brief The constructor for an undefined bit field.
	 *
	 * \param leftBound Left bound of the undefined field.
	 * \param rightBound Right bound of the undefined field.
	 * \param parent The pointer to the register that owns this field.
	 *
	*/
	FieldGraphItem(unsigned int leftBound, 
		unsigned int rightBound, 
		QGraphicsItem* parent);

	//! \brief The destructor
	virtual ~FieldGraphItem();

	/*! \brief Get the offset of the field from the beginning of the register.
	 *
	 * \return The offset of the field.
	*/
	virtual int getOffset() const;

	/*! \brief Get the width of the field.
	 *
	 * \return The number of the bits this field contains.
	*/
	virtual int getWidth() const;

protected:

	/*! \brief Set new positions for child items.
	 *
	*/
	virtual void reorganizeChildren();

private:

	//! \brief No copying
	FieldGraphItem(const FieldGraphItem& other);

	//! \brief No assignment
	FieldGraphItem& operator=(const FieldGraphItem& other);

	//! \brief Pointer to the field being visualized.
	QSharedPointer<Field> field_;
};

#endif // FIELDGRAPHITEM_H
