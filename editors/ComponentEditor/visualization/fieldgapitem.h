/* 
 *  	Created on: 30.11.2012
 *      Author: Antti Kamppi
 * 		filename: fieldgapitem.h
 *		Project: Kactus 2
 */

#ifndef FIELDGAPITEM_H
#define FIELDGAPITEM_H

#include <editors/ComponentEditor/visualization/memorygapitem.h>

#include <QGraphicsItem>

class FieldGapItem : public MemoryGapItem {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the parent graphics item.
	 *
	*/
	FieldGapItem(QGraphicsItem* parent);
	
	/*! \brief The constructor.
	 *
     * \param name   Name to display for the gap item.
	 * \param parent Pointer to the parent graphics item.
	 *
	*/
	FieldGapItem(QString name, QGraphicsItem* parent);

	//! \brief The destructor
	virtual ~FieldGapItem();

	//! \brief Refresh the gap item.
	virtual void refresh();

	/*! \brief Get the bit width of the item.
	 * 
	 * \return The bit width of the item.
	*/
	virtual int getBitWidth() const;

	/*! \brief Get the width of the item.
	 *
	 * \return The width of the gap item.
	*/
	virtual qreal itemTotalWidth() const;

    /*! \brief Set lowest bit for the gap.
	 *
	 * \param address The lowest bit that limits the gap.
	 * \param contains If true then the address is contained in the gap, 
	 * otherwise the gap starts from next address.
	 *
	*/
	void setStartAddress(quint64 address, bool contains = true);

	/*! \brief Set highest bit for the gap.
	 *
	 * \param address The highest bit that limits the gap end.
	 * \param contains If true then the address is contained in the gap,
	 * otherwise the gap ends before the specified address.
	 *
	*/
	void setEndAddress(quint64 address, bool contains = true);

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
	FieldGapItem(const FieldGapItem& other);

	//! \brief No assignment
	FieldGapItem& operator=(const FieldGapItem& other);
};

#endif // FIELDGAPITEM_H
