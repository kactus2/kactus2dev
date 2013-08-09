/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: expandableitem.h
 *		Project: Kactus 2
 */

#ifndef EXPANDABLEITEM_H
#define EXPANDABLEITEM_H

#include "visualizeritem.h"

#include <QGraphicsRectItem>
#include <QBrush>

class GraphicsExpandCollapseItem;

/*! \brief The base class to visualize IP-Xact models that contain child items.
 *
 */
class ExpandableItem : public VisualizerItem {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of the item.
	 *
	*/
	ExpandableItem(QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~ExpandableItem();

	/*! \brief Check if the item has its children visible or not.
	 *
	 * \return True if children are visible.
	*/
	virtual bool isExpanded() const;

protected slots:

	/*! \brief Handler for expandCollapseItem's stateChanged()-signal.
	 *
	 * \param expanded If true then child items are shown.
	 *
	*/
	void onExpandStateChange(bool expanded);

	/*! \brief Set the expand collapse item to hidden/shown state.
	 *
	 * \param show If true then the item is shown.
	 *
	*/
	void setShowExpandableItem(bool show);

protected:

	/*! \brief Set new positions for child items.
	 * 
	 * The implementation updates the position of the expand/collapse button and
	 * calls base class implementation.
	*/
	virtual void reorganizeChildren();

	/*! \brief Set the brush used to color the expansion column.
	 *
	 * \param brush The brush to use.
	 *
	*/
	virtual void setExpansionBrush(const QBrush& brush);


    virtual void setExpansionPen( const QPen& pen);

	/*! \brief Hide/show the rectangle that contains the expand/collapse icon.
	 *
	 * \param visible If true then the rectangle is shown.
	 *
	*/
	virtual void setExpansionRectVisible(bool visible);

private:
	
	//! \brief No copying
	ExpandableItem(const ExpandableItem& other);

	//! \brief No assignment
	ExpandableItem& operator=(const ExpandableItem& other);

	//! \brief The item to show/hide the child items
	GraphicsExpandCollapseItem* expandCollapseItem_;

	//! \brief Pointer to the rect item which displays which children belong to parent.
	QGraphicsRectItem* expansionRect_;
};

#endif // EXPANDABLEITEM_H
