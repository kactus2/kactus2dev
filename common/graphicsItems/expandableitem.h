/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: expandableitem.h
 *		Project: Kactus 2
 */

#ifndef EXPANDABLEITEM_H
#define EXPANDABLEITEM_H

#include "visualizeritem.h"

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

	//! \brief The item to show/hide the child items
	GraphicsExpandCollapseItem* expandCollapseItem_;

private:
	
	//! \brief No copying
	ExpandableItem(const ExpandableItem& other);

	//! \brief No assignment
	ExpandableItem& operator=(const ExpandableItem& other);
};

#endif // EXPANDABLEITEM_H
