//-----------------------------------------------------------------------------
// File: graphicsexpandcollapseitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.10.2012
//
// Description:
// This item provides an expand/collapse item.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSEXPANDCOLLAPSEITEM_H
#define GRAPHICSEXPANDCOLLAPSEITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QGraphicsPixmapItem>
#include <QObject>

//-----------------------------------------------------------------------------
//! This item provides an expand/collapse item.
//-----------------------------------------------------------------------------
class GraphicsExpandCollapseItem : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT

public:

	//! The length of one side of the rectangle.
	enum Bounds
    {
		SIDE = 16
	};

	enum { Type = GFX_TYPE_EXPAND_COLLAPSE_ITEM};

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this graphics item.
	 */
	GraphicsExpandCollapseItem(QGraphicsItem* parent = 0);
	
	//! The destructor.
	virtual ~GraphicsExpandCollapseItem();

	//! Set the item to expanded state.
	virtual void expand();

	/*!
     *  Check if the item is in expanded state.
	 *
	 *      @return True if the item is in expanded state.
	 */
	virtual bool isExpanded() const;

	//! Set the item to collapsed state.
	virtual void collapse();

	/*!
     *  Check if the item is in collapsed state.
	 *
	 *      @return True if the item is in collapsed state.
	 */
	virtual bool isCollapsed() const;

	/*!
	 *  Get the type.
	 */
	virtual int type() const;

signals:

	/*!
     *  Emitted when the state of the item changes.
	 * 
	 *  This is emitted only when user clicks on the item.
	 *  The expand() and collapse() functions do not emit this signal.
	 * 
	 *      @param [in] expanded    True if the item changed to expanded state.
	 */
	void stateChanged(bool expanded);

protected:

	//! Called when the user presses a mouse button.
	 virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
	
	//! No copying.
	GraphicsExpandCollapseItem(const GraphicsExpandCollapseItem& other);

	//! No assignment.
	GraphicsExpandCollapseItem& operator=(const GraphicsExpandCollapseItem& other);

	//! Contains the current state of the item.
	bool expanded_;
};

#endif // GRAPHICSEXPANDCOLLAPSEITEM_H
