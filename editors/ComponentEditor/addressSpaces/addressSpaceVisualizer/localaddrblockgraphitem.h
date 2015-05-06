//-----------------------------------------------------------------------------
// File: localaddrblockgraphitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.01.2013
//
// Description:
// The graph item to visualize address blocks in local memory map.
//-----------------------------------------------------------------------------

#ifndef LOCALADDRBLOCKGRAPHITEM_H
#define LOCALADDRBLOCKGRAPHITEM_H

#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>
#include <IPXACTmodels/addressblock.h>

#include <QSharedPointer>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graph item to visualize address blocks in local memory map.
//-----------------------------------------------------------------------------
class LocalAddrBlockGraphItem : public AddressSpaceVisualizationItem
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *       @param [in] addrSpace          The address space containing the local address block.
     *       @param [in] block              The address block being visualized.
     *       @param [in] expressionParser   The expression parser to use for local memory maps.
	 *       @param [in] parent             The parent graph item.
	 *
	*/
	LocalAddrBlockGraphItem(QSharedPointer<AddressSpace> addrSpace,
		QSharedPointer<AddressBlock> block,
        QSharedPointer<ExpressionParser> expressionParser,
		QGraphicsItem* parent = 0);
	
	//! The destructor
	virtual ~LocalAddrBlockGraphItem();

	//! Refresh the item.
	virtual void refresh();

	/*! Get the offset of the item. 
	 *
	 *      @return int The offset of the item.
	*/
	virtual quint64 getOffset() const;

	/*! Get the last address contained in the item.
	 *
	 *      @return The last address.
	*/
	virtual quint64 getLastAddress() const;

private:
	
	//! No copying
	LocalAddrBlockGraphItem(const LocalAddrBlockGraphItem& other);

	//! No assignment
	LocalAddrBlockGraphItem& operator=(const LocalAddrBlockGraphItem& other);

	//! The address block being visualized.
	QSharedPointer<AddressBlock> addrBlock_;

    //! Expression parser for solving expressions.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // LOCALADDRBLOCKGRAPHITEM_H
