//-----------------------------------------------------------------------------
// File: ConflictedItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 08.08.2013
//
// Description:
// Graphical item representing an overlapping memory section.
//-----------------------------------------------------------------------------

#ifndef CONFLICTEDITEM_H
#define CONFLICTEDITEM_H

#include <IPXactWrapper/ComponentEditor/visualization/memorygapitem.h>

#include <QGraphicsItem>

//-----------------------------------------------------------------------------
//! Graphical item representing an overlapping memory section.
//-----------------------------------------------------------------------------

class ConflictedItem : public MemoryGapItem {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the parent graphics item.
	 *
	*/
	ConflictedItem(QGraphicsItem* parent);
	
	//! \brief The destructor
	virtual ~ConflictedItem();

private:
	//! \brief No copying
	ConflictedItem(const ConflictedItem& other);

	//! \brief No assignment
	ConflictedItem& operator=(const ConflictedItem& other);
};

#endif // CONFLICTEDITEM_H
