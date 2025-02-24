//-----------------------------------------------------------------------------
// File: MemoryDesignerDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.07.2016
//
// Description:
// Declares the memory design diagram class.
//-----------------------------------------------------------------------------

#ifndef MEMORYDESIGNERDIAGRAM_H
#define MEMORYDESIGNERDIAGRAM_H

#include <IPXACTmodels/common/VLNV.h>

#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>

#include <QGraphicsScene>
#include <QSharedPointer>

class LibraryInterface;
class MemoryDesignDocument;
class ConnectivityGraph;
class MemoryDesignConstructor;

//-----------------------------------------------------------------------------
//! Declares the memory design diagram class.
//-----------------------------------------------------------------------------
class MemoryDesignerDiagram : public QGraphicsScene
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] library     The library interface.
     *    @param [in] parent      The parent widget.
     */    
    MemoryDesignerDiagram(LibraryInterface* library, MemoryDesignDocument* parent = 0);

    /*!
     *  The destructor.
     */
    ~MemoryDesignerDiagram();

    /*!
     *  Change the condensing of memory graphics items.
     *
     *    @param [in] condenseItems   Value for condensing memory graphics items.
     */
    void setCondenseMemoryItems(bool condenseMemoryItems);

    /*!
     *  Check whether the memory graphics items are condensed or not.
     *
     *    @return True, if the memory graphics items are condensed, otherwise false.
     */
    bool memoryItemsAreCondensed() const;

    /*!
     *  Set the filtering for chained address space memory connections.
     *
     *    @param [in] filterChains    Value for the chained address space memory connection filtering.
     */
    void setFilterAddressSpaceChains(bool filterChains);

    /*!
     *  Check whether the chained address space memory connections are filtered or not.
     *
     *    @return True, if the chained address space memory connections are filtered, false otherwise.
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
     *    @param [in] filterSegments  Value for filtering address space segments.
     */
    void setFilterAddressSpaceSegments(bool filterSegments);

    /*!
     *  Check if the address space segments are filtered.
     *
     *    @return True, if the address space segments are filtered, false otherwise.
     */
    bool addressSpaceSegmentsAreFiltered() const;

    /*!
     *  Set a new value for filtering the address blocks.
     *
     *    @param [in] filterBlocks    Value for filtering address blocks.
     */
    void setFilterAddressBlocks(bool filterBlocks);

    /*!
     *  Check if the address blocks are filtered.
     *
     *    @return True, if the address blocks are filtered, false otherwise.
     */
    bool addressBlocksAreFiltered() const;

    /*!
     *  Set a new value for filtering the address block registers.
     *
     *    @param [in] filterSegments  Value for filtering address block registers.
     */
    void setFilterAddressBlockRegisters(bool filterRegisters);

    /*!
     *  Check if the address block registers are filtered.
     *
     *    @return True, if the address block registers are filtered, false otherwise.
     */
    bool addressBlockRegistersAreFiltered() const;

    /*!
     *  Set a new value for filtering the register fields.
     *
     *    @param [in] filterFields    Value for filtering register fields.
     */
    void setFilterFields(bool filterFields);

    /*!
     *  Check if the register fields are filtered.
     *
     *    @return True, if the register fields are filtered, false otherwise.
     */
    bool fieldsAreFiltered() const;

    /*!
     *  Set a new value for filtering the unconnected memory items.
     *
     *    @param [in] filterUnconnected   Value for filtering unconnected memory items.
     */
    void filterUnconnectedMemoryItems(bool filterUnconnected);

    /*!
     *  Check if the unconnected memory items are filtered.
     *
     *    @return True, if the unconnected memory items are filtered, false otherwise.
     */
    bool unconnectedMemoryItemsAreFiltered() const;
    
    /*!
     *  Set a new value for filtering the memory overlap items.
     *
     *    @param [in] filterOverlap       Value for filtering memory overlap.
     */
    void filterMemoryOverlapItems(bool filterOverlap);

    /*!
     *  Check if the memory overlap items are filtered.
     *
     *    @return True, if the memory overlap items are filtered, false otherwise.
     */
    bool memoryOverlapItemsAreFiltered() const;

    /*!
     *  Load a design using the selected view.
     *
     *    @param [in] component   Component containing the view.
     *    @param [in] viewName    The name of the selected view.
     */
    bool loadDesignFromCurrentView(QSharedPointer<const Component> component, QString const& viewName);

    /*!
     *  Clear the graphics scene.
     */
    virtual void clearScene();

    /*!
     *  Handle the display of the diagram.
     */
    void onShow();

    /*!
     *  Handle the vertical scroll.
     *
     *    @param [in] y   The amount of vertical scrolling.
     */
    void onVerticalScroll(qreal y);

protected:

    /*!
     *  Handle the mouse wheel event.
     *
     *    @param [in] event   The wheel event.
     */
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event);

    /*!
     *  Draw the diagram background.
     *
     *    @param [in] painter     The used painter.
     *    @param [in] rect        The containing rectangle of the diagram.
     */
    virtual void drawBackground(QPainter *painter, const QRectF &rect);

signals:

    /*!
     *  Open the component document for the selected VLNV.
     *
     *    @param [in] vlnv                VLNV of the containing component.
     *    @param [in] identifierChain     List of strings to identify the component item.
     */
    void openComponentDocument(VLNV const& vlnv, QVector<QString> identifierChain);

private:
    // Disable copying.
    MemoryDesignerDiagram(MemoryDesignerDiagram const& rhs);
    MemoryDesignerDiagram& operator=(MemoryDesignerDiagram const& rhs);

    /*!
     *  Clear the layout.
     */
    void clearLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Document containing the memory design diagram.
    MemoryDesignDocument* parentDocument_;

    //! Layout of the memory design diagram.
    QSharedPointer<GraphicsColumnLayout> layout_;

    //! The used library interface.
    LibraryInterface* libraryHandler_;

    //! Factory for creating the connectivity graph.
    ConnectivityGraphFactory graphFactory_;

    //! Constructor for memory design graphics items.
    MemoryDesignConstructor* memoryConstructor_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNERDIAGRAM_H
