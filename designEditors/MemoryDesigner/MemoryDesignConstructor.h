//-----------------------------------------------------------------------------
// File: MemoryDesignConstructor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.01.2017
//
// Description:
// Constructs the items for the memory design diagram.
//-----------------------------------------------------------------------------

#ifndef MEMORYDESIGNCONSTRUCTOR_H
#define MEMORYDESIGNCONSTRUCTOR_H

#include <IPXACTmodels/common/VLNV.h>

#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <designEditors/MemoryDesigner/ConnectivityGraphFactory.h>

#include <QSharedPointer>
#include <QObject>

class MemoryColumnHandler;
class MemoryGraphicsItemHandler;
class MemoryConnectionHandler;

//-----------------------------------------------------------------------------
//! Constructs the items for the memory design diagram.
//-----------------------------------------------------------------------------
class MemoryDesignConstructor : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] layout  Layout for the memory graphics items.
     */    
    MemoryDesignConstructor(QSharedPointer<GraphicsColumnLayout> layout);

    /*!
     *  The destructor.
     */
    ~MemoryDesignConstructor();

    /*!
     *  Change the condensing of memory graphics items.
     *
     *      @param [in] condenseItems   Value for condensing memory graphics items.
     */
    void setCondenseMemoryItems(bool condenseMemoryItems);

    /*!
     *  Check whether the memory graphics items are condensed or not.
     *
     *      @return True, if the memory graphics items are condensed, otherwise false.
     */
    bool memoryItemsAreCondensed() const;

    /*!
     *  Set the filtering for chained address space memory connections.
     *
     *      @param [in] filterChains    Value for the chained address space memory connection filtering.
     */
    void setFilterAddressSpaceChains(bool filterChains);

    /*!
     *  Check whether the chained address space memory connections are filtered or not.
     *
     *      @return True, if the chained address space memory connections are filtered, false otherwise.
     */
    bool addressSpaceChainsAreFiltered() const;

    /*!
     *  Condense all the field graphics items to their original width.
     */
    void condenseFieldItems();

    /*!
     *  Extend all the field graphics items to fully display their name label.
     */
    void extendFieldItems();

    /*!
     *  Set a new value for filtering the address space segments.
     *
     *      @param [in] filterSegments  Value for filtering address space segments.
     */
    void setFilterAddressSpaceSegments(bool filterSegments);

    /*!
     *  Check if the address space segments are filtered.
     *
     *      @return True, if the address space segments are filtered, false otherwise.
     */
    bool addressSpaceSegmentsAreFiltered() const;

    /*!
     *  Set a new value for filtering the address blocks.
     *
     *      @param [in] filterBlocks    Value for filtering address blocks.
     */
    void setFilterAddressBlocks(bool filterBlocks);

    /*!
     *  Check if the address blocks are filtered.
     *
     *      @return True, if the address blocks are filtered, false otherwise.
     */
    bool addressBlocksAreFiltered() const;

    /*!
     *  Set a new value for filtering the address block registers.
     *
     *      @param [in] filterSegments  Value for filtering address block registers.
     */
    void setFilterAddressBlockRegisters(bool filterRegisters);

    /*!
     *  Check if the address block registers are filtered.
     *
     *      @return True, if the address block registers are filtered, false otherwise.
     */
    bool addressBlockRegistersAreFiltered() const;

    /*!
     *  Set a new value for filtering the register fields.
     *
     *      @param [in] filterFields    Value for filtering register fields.
     */
    void setFilterFields(bool filterFields);

    /*!
     *  Check if the register fields are filtered.
     *
     *      @return True, if the register fields are filtered, false otherwise.
     */
    bool fieldsAreFiltered() const;

    /*!
     *  Set a new value for filtering the unconnected memory items.
     *
     *      @param [in] filterUnconnected   Value for filtering unconnected memory items.
     */
    void filterUnconnectedMemoryItems(bool filterUnconnected);

    /*!
     *  Check if the unconnected memory items are filtered.
     *
     *      @return True, if the unconnected memory items are filtered, false otherwise.
     */
    bool unconnectedMemoryItemsAreFiltered() const;

    /*!
     *  Construct memory design items from the selected connection graph.
     *
     *      @param [in] connectionGraph     Connection graph containing connection routes.
     *
     *      @return True, if memory items can be created from the graph, false otherwise.
     */
    bool constructMemoryDesignItems(QSharedPointer<ConnectivityGraph> connectionGraph);

    /*!
     *  Set a new graphics layout for the memory graphics items.
     *
     *      @param [in] newLayout   The new graphics layout.
     */
    void setNewLayout(QSharedPointer<GraphicsColumnLayout> newLayout);

    /*!
     *  Get the current width change from the original widths.
     */
    int getWidthBoundary() const;
    
    /*!
     *  Set a new width change.
     *
     *      @param [in] newWidthBoundary    The new width change.
     */
    void setNewWidthBoundary(int newWidthBoundary);

    /*!
     *  Change the width of the memory map columns and their contained memory map graphics items.
     *
     *      @param [in] deltaWidth  The change in width.
     */
    void changeMemoryMapWidths(qreal deltaWidth);

signals:

    /*!
     *  Open the component document for the selected VLNV.
     *
     *      @param [in] vlnv                VLNV of the containing component.
     *      @param [in] identifierChain     List of strings to identify the component item.
     */
    void openComponentDocument(VLNV const& vlnv, QVector<QString> identifierChain);

private:
    // Disable copying.
    MemoryDesignConstructor(MemoryDesignConstructor const& rhs);
    MemoryDesignConstructor& operator=(MemoryDesignConstructor const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Handler for memory columns.
    QSharedPointer<MemoryColumnHandler> columnHandler_;

    //! Handler for memory graphics items.
    QSharedPointer<MemoryGraphicsItemHandler> itemHandler_;

    //! Handler for memory connections.
    QSharedPointer<MemoryConnectionHandler> connectionHandler_;

    //! The current width change from the original widths.
    qreal widthBoundary_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNCONSTRUCTOR_H
