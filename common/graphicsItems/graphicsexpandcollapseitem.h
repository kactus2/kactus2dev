/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: graphicsexpandcollapseitem.h
 *		Project: Kactus 2
 */

#ifndef GRAPHICSEXPANDCOLLAPSEITEM_H
#define GRAPHICSEXPANDCOLLAPSEITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QGraphicsPixmapItem>
#include <QObject>

/*! \brief This item provides an expand/collapse item.
 *
 */
class GraphicsExpandCollapseItem : public QObject, public QGraphicsPixmapItem {
	Q_OBJECT

public:

	//! \brief The length of one side of the rectangle.
	enum Bounds {
		SIDE = 10
	};

	enum { Type = GFX_TYPE_EXPAND_COLLAPSE_ITEM};

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of this graphics item.
	 *
	*/
	GraphicsExpandCollapseItem(QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~GraphicsExpandCollapseItem();

	//! \brief Set the item to expanded state.
	virtual void expand();

	/*! \brief Check if the item is in expanded state.
	 *
	 * \return True if the item is in expanded state.
	*/
	virtual bool isExpanded() const;

	//! \brief Set the item to collapsed state.
	virtual void collapse();

	/*! \brief Check if the item is in collapsed state.
	 *
	 * \return True if the item is in collapsed state.
	*/
	virtual bool isCollapsed() const;

	virtual int type() const;

signals:

	/*! \brief Emitted when the state of the item changes.
	 * 
	 * This is emitted only when user clicks on the item.
	 * The expand() and collapse() functions do not emit this signal.
	 * 
	 * \param expanded True if the item changed to expanded state.
	 *
	*/
	void stateChanged(bool expanded);

protected:

	//! Called when the user presses a mouse button.
	 virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
	
	//! \brief No copying
	GraphicsExpandCollapseItem(const GraphicsExpandCollapseItem& other);

	//! \brief No assignment
	GraphicsExpandCollapseItem& operator=(const GraphicsExpandCollapseItem& other);

	//! \brief Contains the current state of the item.
	bool expanded_;
};

#endif // GRAPHICSEXPANDCOLLAPSEITEM_H
