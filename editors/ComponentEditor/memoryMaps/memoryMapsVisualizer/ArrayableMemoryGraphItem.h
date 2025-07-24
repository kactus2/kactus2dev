//-----------------------------------------------------------------------------
// File: ArrayableMemoryGraphItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 24.06.2025
//
// Description:
// Represents a visualization of a memory item that can be expressed as an array of identical items.
// Replicas of the item are identified with their index. 
//-----------------------------------------------------------------------------

#ifndef ARRAYABLEMEMORYGRAPHITEM_H
#define ARRAYABLEMEMORYGRAPHITEM_H

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <QtGlobal>
#include <QString>

class ArrayableMemoryGraphItem : public MemoryVisualizationItem
{
public:

	ArrayableMemoryGraphItem(QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem* parent = nullptr);
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

	/*!
     *  Get the offset of the item. Offset is different for visualizers of item replicas, if memory item has dimension.
     *
     *    @return int The offset of the item from the parent item's base address.
     */
	quint64 getOffset() const override;
	
	/*!
     *	Set the offset of the item.
     *  
     *    @param [in] newOffset     The new offset value.
     */
	void setOffset(quint64 newOffset);

private:

	//! The replica index of the memory item.
	qint64 replicaIndex_ = -1;
	
	//! The item offset in either bits or address units.
	quint64 offset_ = 0;

};

#endif // ARRAYABLEMEMORYGRAPHITEM_H

