//-----------------------------------------------------------------------------
// File: registerfilegraphitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// Graphical item to represent register file in visualization.
//-----------------------------------------------------------------------------
#ifndef REGISTERFILEGRAPHITEM_H
#define REGISTERFILEGRAPHITEM_H

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include "ArrayableMemoryGraphItem.h"

#include <IPXACTmodels/Component/RegisterFile.h>

#include <QGraphicsItem>
#include <QSharedPointer>

class MemoryVisualizationItem;
class ExpressionParser;


//-----------------------------------------------------------------------------
//! Graphical item to represent register file in visualization.
//-----------------------------------------------------------------------------
class RegisterFileGraphItem : public MemoryVisualizationItem, public ArrayableMemoryGraphItem
{
public:

    /*!
     * The constructor.
     *
     *    @param [in] regFile              The register file represented by the item.
     *    @param [in] expressionParser     Parser for expressions.
     *    @param [in] parent               The parent item.
     */
    RegisterFileGraphItem(QSharedPointer<RegisterFile> regFile,
        QSharedPointer<ExpressionParser> expressionParser,
        QGraphicsItem *parent);

    //! The destructor.
    virtual ~RegisterFileGraphItem() = default;

    //! Disable copying.
    RegisterFileGraphItem(const RegisterFileGraphItem& other) = delete;
    RegisterFileGraphItem& operator=(const RegisterFileGraphItem& other) = delete;

    //! Updates the labels and tooltip for the item.
    void updateDisplay() final;

    /*!
    *  Get the offset of the item.
    *
    *    @return int The offset of the item from the parent item's base address.
    */
    quint64 getOffset() const final;

    /*!
     *	Get the offset of the item.
     *  
     *    @param [in] newOffset     The new offset value.
     */
    void setOffset(quint64 newOffset);

    /*!
    *  Get the last bit contained in the field.
    *
    *    @return The last bit.
    */
    quint64 getLastAddress() const final;

    /*!
    *  Get the bit width of the item.
    *
    *    @return The bit width of the item.
    */
    int getBitWidth() const final;

    /*!
    *  Get number of bits the addressable unit contains.
    *
    *    @return The size of least addressable unit.
    */
    unsigned int getAddressUnitSize() const final;

    /*!
    *  Checks if the item is to be used in the visualization.
    *
    *    @return True, if the item should be used, otherwise false.
    */
    bool isPresent() const final;

private:

    //! The register file being visualized.
    QSharedPointer<RegisterFile> regFile_;

    //! The offset of the register file
    quint64 offset_;
};

#endif // REGISTERFILEGRAPHITEM_H
