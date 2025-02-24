//-----------------------------------------------------------------------------
// File: localaddrblockgraphitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.01.2013
//
// Description:
// The graph item to visualize address blocks in local memory map.
//-----------------------------------------------------------------------------

#ifndef LOCALADDRBLOCKGRAPHITEM_H
#define LOCALADDRBLOCKGRAPHITEM_H

#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>

#include <QSharedPointer>

class ExpressionParser;
class AddressBlock;

//-----------------------------------------------------------------------------
//! The graph item to visualize address blocks in local memory map.
//-----------------------------------------------------------------------------
class LocalAddrBlockGraphItem : public AddressSpaceVisualizationItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] block               The address block being visualized.
	 *    @param [in] addressSpaceWidth   The width of the address space.
	 *    @param [in] expressionParser    The used expression parser.
	 *    @param [in] parent              The parent graph item.
	 */
	LocalAddrBlockGraphItem(QSharedPointer<AddressBlock> block,
        QString const& addressSpaceWidth,
        QSharedPointer<ExpressionParser> expressionParser,
		QGraphicsItem* parent = 0);
	
	//! The destructor.
	virtual ~LocalAddrBlockGraphItem() = default;

    //! No copying.
    LocalAddrBlockGraphItem(const LocalAddrBlockGraphItem& other) = delete;

    //! No assignment.
    LocalAddrBlockGraphItem& operator=(const LocalAddrBlockGraphItem& other) = delete;

	//! Refresh the item.
	virtual void refresh() override final;

	/*!
     *  Get the offset of the item. 
	 *
	 *    @return int The offset of the item.
	 */
	virtual quint64 getOffset() const override final;

	/*!
     *  Get the last address contained in the item.
	 *
	 *    @return The last address.
	 */
	virtual quint64 getLastAddress() const override final;

private:
		
	//! The address block being visualized.
	QSharedPointer<AddressBlock> addrBlock_;
};

#endif // LOCALADDRBLOCKGRAPHITEM_H
