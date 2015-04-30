//-----------------------------------------------------------------------------
// File: registergraphitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.04.2012
//
// Description:
// The graphical item that represents one register.
//-----------------------------------------------------------------------------

#ifndef REGISTERGRAPHITEM_H
#define REGISTERGRAPHITEM_H

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <IPXACTmodels/register.h>

#include <QSharedPointer>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! The graphical item that represents one register.
//-----------------------------------------------------------------------------
class RegisterGraphItem : public MemoryVisualizationItem
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] reg                 The register that this graph item visualizes.
     *      @param [in] expressionParser    Parser for expressions in register fields.
	 *      @param [in] parent              The parent of this graph item.
	*/
	RegisterGraphItem(QSharedPointer<Register> reg, QSharedPointer<ExpressionParser> expressionParser,
        QGraphicsItem* parent);
	
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

    /*!
     *  Sets the index of the register dimension the item represents.
     *
     *      @param [in] index   The dimension index to set.
     */
    void setDimensionIndex(unsigned int index);
        
    /*!
     *  Called to notify the register item that the dimension of the register has changed..
     */
    void dimensionChanged();

protected:

    //! Update the child items in the map. Field items are organized according to last address.
    virtual void updateChildMap();
    
    /*!
     *  Repositions the child items and fills the empty gaps between them.
     */
    virtual void repositionChildren();

private:
	
	//! No copying
	RegisterGraphItem(const RegisterGraphItem& other);
	RegisterGraphItem& operator=(const RegisterGraphItem& other);

    /*!
     *  Gets the register size in AUB units.
     *
     *      @return The size of the register in AUBs.
     */
    quint64 getSizeInAUB() const;

    /*!
     *  Finds the width for the given child item.
     *
     *      @param [in] child   The child whose width to find.
     *
     *      @return The width for the child.
     */
    virtual qreal findWidthFor(MemoryVisualizationItem* child) const;
    
    /*!
     *  Finds the position for the given child item.
     *
     *      @param [in] child   The child whose position within the register item to find.
     *
     *      @return The position.
     */
    QPointF findPositionFor(MemoryVisualizationItem* child) const;
    
    /*!
     *  Checks if there is empty memory space between the beginning of the register item and its first child.
     *
     *      @param [in] current   The currently iterated child.
     *
     *      @return True, if there is empty space, otherwise false.
     */
    bool emptySpaceBeforeLeftmostChild(MemoryVisualizationItem* current);
        
    /*!
     *  Checks if there is empty memory space between the given child and the last known used bit index.
     *
     *      @param [in] current             The currently iterated child.
     *      @param [in] lastBitIndexInUse   The last known used bit index.
     *
     *      @return True, if there is empty space, otherwise false.
     */
    bool emptySpaceBeforeChild(MemoryVisualizationItem* current, quint64 lastBitIndexInUse);
    
    /*!
     *  Checks if the two consecutive children overlap.
     *
     *      @param [in] current     The currently iterated child.
     *      @param [in] previous    The previously iterated child.
     *
     *      @return True, if the children overlap, otherwise false.
     */
    bool childrenOverlap(MemoryVisualizationItem* previous, MemoryVisualizationItem* current);
        
    /*!
     *  Creates a new child for representing a free memory slot.
     *
     *      @param [in] startAddress    The offset of the free memory slot.
     *      @param [in] lastAddress     The last address of the free memory slot.
     */
    void createMemoryGap(quint64 startAddress, quint64 endAddress);

    /* Sorting function for fields with same last address.
     *
     *      @param [in] s1  The left field in comparison s1 < s2.
     *      @param [in] s2  The right field in comparison s1 < s2.
     *
     *      @return True, if s1 precedes s2, otherwise false.
     */
    static bool compareItems(const MemoryVisualizationItem* s1, const MemoryVisualizationItem* s2);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the register being visualized.
	QSharedPointer<Register> register_;

    //! The index in the dimension the item represents.
    unsigned int dimensionIndex_;

    //! Parser for expressions in register fields.
    QSharedPointer<ExpressionParser> expresionParser_;
};

#endif // REGISTERGRAPHITEM_H
