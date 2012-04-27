/* 
 *  	Created on: 24.4.2012
 *      Author: Antti Kamppi
 * 		filename: registergraphitem.h
 *		Project: Kactus 2
 */

#ifndef REGISTERGRAPHITEM_H
#define REGISTERGRAPHITEM_H

#include "memoryvisualizeritem.h"
#include <models/register.h>
#include "fieldgraphitem.h"

#include <QSharedPointer>
#include <QList>

/*! \brief The graphical item that represents one register.
 *
 */
class RegisterGraphItem : public MemoryVisualizerItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param reg Pointer to the register that this graph item visualizes.
	 * \param parent Pointer to the parent of this graph item.
	 * \param maxWidth The maximum width of the register in bits.
	 *
	*/
	RegisterGraphItem(QSharedPointer<Register> reg,
		QGraphicsItem* parent, 
		int maxWidth);
	
	//! \brief The destructor
	virtual ~RegisterGraphItem();

	/*! \brief Get the width of the register.
	 *
	 * This width counts all the field-elements together and returns their
	 * combined width.
	 *
	 * \return The width of the register.
	*/
	virtual qreal itemTotalWidth() const;

	/*! \brief Get the offset of the register from the address block's base address.
	 *
	 * \return The number of addressable units from the beginning of the address block.
	*/
	virtual int getOffset() const;

protected:

	//! \brief Set new positions for the register graph items
	virtual void reorganizeChildren();

private:
	
	//! \brief No copying
	RegisterGraphItem(const RegisterGraphItem& other);

	//! \brief No assignment
	RegisterGraphItem& operator=(const RegisterGraphItem& other);

	//! \brief Pointer to the register being visualized.
	QSharedPointer<Register> register_;

	//! \brief Contains pointers to the editors of the fields in this register.
	QList<QSharedPointer<FieldGraphItem> > fields_;
};

#endif // REGISTERGRAPHITEM_H
