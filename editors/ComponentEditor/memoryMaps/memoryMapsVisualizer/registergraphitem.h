/* 
 *  	Created on: 24.4.2012
 *      Author: Antti Kamppi
 * 		filename: registergraphitem.h
 *		Project: Kactus 2
 */

#ifndef REGISTERGRAPHITEM_H
#define REGISTERGRAPHITEM_H

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <IPXACTmodels/register.h>

#include <QSharedPointer>

/*! The graphical item that represents one register.
 *
 */
class RegisterGraphItem : public MemoryVisualizationItem {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 * \param reg Pointer to the register that this graph item visualizes.
	 * \param parent Pointer to the parent of this graph item.
	 *
	*/
	RegisterGraphItem(QSharedPointer<Register> reg, QGraphicsItem* parent);
	
	//! The destructor
	virtual ~RegisterGraphItem();

	//! Refresh the item, refresh and re-layout the sub-items and refresh parent item.
    virtual void refresh();

    //! Updates the labels and tooltip for the item.
   virtual  void updateDisplay();
    
	/*! Add a child visualization item for this item.
	 *
	 *      @param [in] childItem   Pointer to the child to add.
	*/
	virtual void addChild(MemoryVisualizationItem* childItem);

	//! Remove the item.
    virtual void removeChild(MemoryVisualizationItem* childItem );

	/*! Get the offset of the item. 
	 *
	 *      @return The offset of the item from the parent item's base address.
	*/
	virtual quint64 getOffset() const;

	/*! Get the last address contained in the item.
	 *
	 *      @return The last address.
	*/
	virtual quint64 getLastAddress() const;

	/*! Get the bit width of the item.
	 * 
	 *      @return The bit width of the item.
	*/
	virtual int getBitWidth() const;

	/*! Get number of bits the addressable unit contains.
	 *
	 *      @return The size of least addressable unit.
	*/
	virtual unsigned int getAddressUnitSize() const;

    /*! Set the width for the item.
	 *
	 *       @param [in] width The new width of the item.
	*/
    virtual void setWidth(qreal width);

protected slots:

	/*! Set new positions for child field items.
	 * 
	 * The child items are organized in the order of their last address (MSB).
	*/
	virtual void reorganizeChildren();

protected:

    //! Update the child items in the map. Field items are organized according to last address.
    void updateChildMap();

private:
	
	//! No copying
	RegisterGraphItem(const RegisterGraphItem& other);

	//! No assignment
	RegisterGraphItem& operator=(const RegisterGraphItem& other);
    
    /*!
     *  Finds the width for the given child item.
     *
     *      @param [in] child   The child whose width to find.
     *
     *      @return The width for the child.
     */
    virtual qreal widthForChild(MemoryVisualizationItem* child) const;

    /* Sorting function for fields with same last address.
     *
     * \param s1 The left field in comparison s1 < s2.
     *
     * \param s2 The right field in comparison s1 < s2.
     *
     * \return True, if s1 precedes s2, otherwise false.
     */
    static bool compareItems(const MemoryVisualizationItem* s1, const MemoryVisualizationItem* s2);
   
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the register being visualized.
	QSharedPointer<Register> register_;

};

#endif // REGISTERGRAPHITEM_H
