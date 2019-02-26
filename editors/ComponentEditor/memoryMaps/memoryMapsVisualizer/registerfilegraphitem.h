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

#include <IPXACTmodels/Component/RegisterFile.h>

#include <QGraphicsItem>
#include <QSharedPointer>

class MemoryVisualizationItem;
class ExpressionParser;


//-----------------------------------------------------------------------------
//! Graphical item to represent register file in visualization.
//-----------------------------------------------------------------------------
class RegisterFileGraphItem : public MemoryVisualizationItem
{
public:

    /*!
     * The constructor.
     *
     *     @param [in] regFile              The register file represented by the item.
     *     @param [in] expressionParser     Parser for expressions.
     *     @param [in] parent               The parent item.
     */
    RegisterFileGraphItem(QSharedPointer<RegisterFile> regFile,
        QSharedPointer<ExpressionParser> expressionParser,
        QGraphicsItem *parent);

    //! The destructor.
    virtual ~RegisterFileGraphItem() = default;

    //! Disable copying.
    RegisterFileGraphItem(const RegisterFileGraphItem& other) = delete;
    RegisterFileGraphItem& operator=(const RegisterFileGraphItem& other) = delete;


    //! Refresh the item and the parent item.
    virtual void refresh() override final;

    //! Updates the labels and tooltip for the item.
    virtual void updateDisplay() override final;

    /*!
    *  Get the offset of the item.
    *
    *      @return int The offset of the item from the parent item's base address.
    */
    virtual quint64 getOffset() const override final;

    /*!
    *  Get the last bit contained in the field.
    *
    *      @return The last bit.
    */
    virtual quint64 getLastAddress() const override final;

    /*!
    *  Get the bit width of the item.
    *
    *      @return The bit width of the item.
    */
    virtual int getBitWidth() const override final;

    /*!
    *  Get number of bits the addressable unit contains.
    *
    *      @return The size of least addressable unit.
    */
    virtual unsigned int getAddressUnitSize() const override final;

    /*!
    *  Checks if the item is to be used in the visualization.
    *
    *      @return True, if the item should be used, otherwise false.
    */
    virtual bool isPresent() const override final;

private:

    //! Pointer to the register being visualized.
    QSharedPointer<RegisterFile> regFile_;

};

#endif // REGISTERFILEGRAPHITEM_H
