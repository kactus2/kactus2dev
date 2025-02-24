/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: expandableitem.h
 *		Project: Kactus 2
 */

#ifndef EXPANDABLEITEM_H
#define EXPANDABLEITEM_H

#include <editors/ComponentEditor/visualization/AddressableItem.h>

#include "graphicsexpandcollapseitem.h"

#include <QGraphicsRectItem>
#include <QBrush>


/*! The base class to visualize IP-Xact models that contain child items.
 *
 */
class ExpandableItem : public AddressableItem {
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 * \param parent Pointer to the owner of the item.
	 *
	*/
	ExpandableItem(QGraphicsItem* parent = 0);
	
	//! The destructor
	virtual ~ExpandableItem() = default;

    //! No copying
    ExpandableItem(const ExpandableItem& other) = delete;

    //! No assignment
    ExpandableItem& operator=(const ExpandableItem& other) = delete;

	/*! Check if the item is expanded and has its children visible or not.
	 *
	 * \return True if children are visible.
	*/
	bool isExpanded() const;

    void resizeToContent();

    /*! Get the rectangle reserved by this item and it's sub-items recursively.
     *
     *    @return The rectangle of this item and sub-items and their children recursively.
    */
    virtual QRectF itemTotalRect() const = 0;


	void setConflicted(bool conflicted) override;

signals:
    //! Emitted when the item is expanded or collapsed.
    void expandStateChanged();

protected slots:

	/*! Handler for expandCollapseItem's stateChanged()-signal.
	 *
	 * \param expanded If true then child items are shown.
	 *
	*/
	void onExpandStateChange(bool expanded);

	/*! Set the expand collapse item to hidden/shown state.
	 *
	 * \param show If true then the item is shown.
	 *
	*/
	void setShowExpandableItem(bool show);

protected:

    /*!
     *  Sets the default coloring brush for the item.
     *
     *    @param [in] brush   The brush to set.
     */
    void setDefaultBrush(QBrush brush) final;

    void setExpansionPen(QPen const& pen);

	/*! Hide/show the rectangle that contains the expand/collapse icon.
	 *
	 * \param visible If true then the rectangle is shown.
	 *
	*/
	void setExpansionRectVisible(bool visible);

private:	

    /*!
     * Update the expand arrow direction based on expansion state.
     *
     *    @param [in] expanded     Indicates if the item is expanded or not.
     */
     void updateExpandArrow(bool expanded);

    //! Updates the item rectangle to correct size.
    void updateRectangle();

	//! The item to show/hide the child items
	GraphicsExpandCollapseItem* expandCollapseItem_ = new GraphicsExpandCollapseItem(this);

	//! Pointer to the image used to denote the expandability of the item.
    QGraphicsPixmapItem* expansionArrow_ = new QGraphicsPixmapItem(this);

};

#endif // EXPANDABLEITEM_H
