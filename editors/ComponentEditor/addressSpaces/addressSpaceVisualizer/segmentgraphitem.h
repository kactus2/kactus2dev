/* 
 *  	Created on: 20.12.2012
 *      Author: Antti Kamppi
 * 		filename: segmentgraphitem.h
 *		Project: Kactus 2
 */

#ifndef SEGMENTGRAPHITEM_H
#define SEGMENTGRAPHITEM_H

#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>
#include <IPXACTmodels/segment.h>

#include <QSharedPointer>

/*! \brief The graph item to visualize segments in address space editor.
 *
 */
class SegmentGraphItem : public AddressSpaceVisualizationItem {
	Q_OBJECT

public:

	/*! \brief The constructor 
	 *
	 * \param addrSpace Pointer to the address space being visualized.
	 * \param segment Pointer to the segment represented by this graph item.
	 * \param parent Pointer to the parent graph item.
	 *
	*/
	SegmentGraphItem(QSharedPointer<AddressSpace> addrSpace,
		QSharedPointer<Segment> segment,
		QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~SegmentGraphItem();

	//! \brief Refresh the item.
	virtual void refresh();

	/*! \brief Get the offset of the item. 
	 *
	 * \return int The offset of the item.
	*/
	virtual quint64 getOffset() const;

	/*! \brief Get the last address contained in the item.
	 *
	 * \return The last address.
	*/
	virtual quint64 getLastAddress() const;

    /*! \brief Sets the first non-overlapping address to display.
	 *
	 * \param The first address to set.
	 */
    virtual void setOverlappingTop(quint64 const& address);

	/*! \brief Sets the last non-overlapping address to display.
	 *
	 * \param The last address to set.
	 */
    virtual void setOverlappingBottom(quint64 const& address);


private:
	
	//! \brief No copying
	SegmentGraphItem(const SegmentGraphItem& other);

	//! \brief No assignment
	SegmentGraphItem& operator=(const SegmentGraphItem& other);

	//! \brief Pointer to the segment being visualized.
	QSharedPointer<Segment> segment_;
};

#endif // SEGMENTGRAPHITEM_H
