//-----------------------------------------------------------------------------
// File: ArrayableMemoryGraphItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 24.06.2025
//
// Description:
// Describes a memory item that can be expressed as an array of identical items.
// Replicas of the item are identified with their index. 
//-----------------------------------------------------------------------------

#ifndef ARRAYABLEMEMORYGRAPHITEM_H
#define ARRAYABLEMEMORYGRAPHITEM_H

#include <QtGlobal>
#include <QString>

class ArrayableMemoryGraphItem
{
public:

	ArrayableMemoryGraphItem() = default;
	virtual ~ArrayableMemoryGraphItem() = default;

	// No copying or assigning
	ArrayableMemoryGraphItem(ArrayableMemoryGraphItem const& other) = delete;
	ArrayableMemoryGraphItem& operator=(ArrayableMemoryGraphItem const& other) = delete;

	/*!
	 *	Sets the replica index of the memory item
	 *  
	 *    @param [in] newIndex     The new replica index
	 */
	void setReplicaIndex(qint64 newIndex);

	/*!
	 *	Get the replica index of the memory item.
	 *  
	 *    @return The replica index.
	 */
	qint64 getReplicaIndex() const;

	/*!
	 *	Get the name of the item taking replica index into account
	 *  
	 *    @param [in] originalName     The name of the item without the replica index.
	 *	    
	 *    @return The name of the item.
	 */
	QString getReplicaName(QString const& originalName) const;

private:

	//! The replica index of the memory item.
	qint64 replicaIndex_ = -1;

};

#endif // ARRAYABLEMEMORYGRAPHITEM_H

