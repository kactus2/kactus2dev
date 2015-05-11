//-----------------------------------------------------------------------------
// File: AddressSpaceConflictedItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2013
//
// Description:
// Overlapping memory item for segments and local memory blocks.
//-----------------------------------------------------------------------------

#ifndef AddressSpaceConflictedItem_H
#define AddressSpaceConflictedItem_H

#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacegapitem.h>

class ExpressionParser;
//-----------------------------------------------------------------------------
//! Visual item for overlapping memory segments and local memory blocks.
//-----------------------------------------------------------------------------
class AddressSpaceConflictedItem : public AddressSpaceGapItem {
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] addrSpace   Pointer to the address space containing the item.
     *      @param [in] addrPos  Address alignment.
     *      @param [in] parent  Pointer to the parent graph item.
     */
	AddressSpaceConflictedItem(AddressPosition addrPos,
        QString const& addressSpaceWidth,
        QSharedPointer<ExpressionParser> expressionParser,
		QGraphicsItem* parent = 0);
	
	//! The destructor.
	virtual ~AddressSpaceConflictedItem();

	//! Refresh the item.
    virtual void refresh();

private:
	
	//! No copying
	AddressSpaceConflictedItem(const AddressSpaceConflictedItem& other);

	//! No assignment
	AddressSpaceConflictedItem& operator=(const AddressSpaceConflictedItem& other);

};

#endif // AddressSpaceConflictedItem_H
