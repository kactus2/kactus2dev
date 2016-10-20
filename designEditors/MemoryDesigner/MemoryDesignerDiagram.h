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

#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <designEditors/MemoryDesigner/ConnectivityGraphFactory.h>

#include <QGraphicsScene>
#include <QSharedPointer>

class MemoryDesignDocument;
class GraphicsColumnLayout;
class Design;
class DesignConfiguration;
class Component;
class View;
class LibraryInterface;
class MemoryColumn;
class ConnectivityGraph;
class ConnectivityComponent;
class MemoryItem;
class MainMemoryGraphicsItem;
class MemoryConnectionItem;
class MemoryMapGraphicsItem;

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
     *      @param [in] library     The library interface.
     *      @param [in] parent      The parent widget.
     */    
    MemoryDesignerDiagram(LibraryInterface* library, MemoryDesignDocument* parent = 0);

    /*!
     *  The destructor.
     */
    ~MemoryDesignerDiagram();

    /*!
     *  Load a design using the selected view.
     *
     *      @param [in] component   Component containing the view.
     *      @param [in] viewName    The name of the selected view.
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
     *      @param [in] y   The amount of vertical scrolling.
     */
    void onVerticalScroll(qreal y);

protected:

    /*!
     *  Handle the mouse wheel event.
     *
     *      @param [in] event   The wheel event.
     */
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event);

    /*!
     *  Draw the diagram background.
     *
     *      @param [in] painter     The used painter.
     *      @param [in] rect        The containing rectangle of the diagram.
     */
    virtual void drawBackground(QPainter *painter, const QRectF &rect);

private:
    // Disable copying.
    MemoryDesignerDiagram(MemoryDesignerDiagram const& rhs);
    MemoryDesignerDiagram& operator=(MemoryDesignerDiagram const& rhs);

    /*!
     *  Clear the layout.
     */
    void clearLayout();

    /*!
     *  Create the connection graph the selected view.
     *
     *      @param [in] component   The component containing the selected view.
     *      @param [in] viewName    Name of the selected view.
     *
     *      @return True, if the connection graph could be created, false otherwise.
     */
    bool createConnectionGraph(QSharedPointer<const Component> component, QString const& viewName);

    /*!
     *  Get the design referenced by the selected view.
     *
     *      @param [in] component   Component containing the selected view.
     *      @param [in] viewName    The name of the selected view.
     *
     *      @return The design referenced by the selected view.
     */
    QSharedPointer<Design> getContainingDesign(QSharedPointer<const Component> component, QString const& viewName)
        const;

    /*!
     *  Get the design configuration referenced by the selected view.
     *
     *      @param [in] component   Component containing the selected view.
     *      @param [in] viewName    The name of the selected view.
     *
     *      @return The design configuration referenced by the selected view.
     */
    QSharedPointer<const DesignConfiguration> getContainingDesignConfiguration(
        QSharedPointer<const Component> component, QString const& viewName) const;

    /*!
     *  Create the initial memory columns.
     */
    void createColumns();

    /*!
     *  Create an address space column.
     *
     *      @return The created address space column.
     */
    MemoryColumn* createAddressSpaceColumn();

    /*!
     *  Create the initial memory columns.
     */
    void createMemoryColumns();

    /*!
     *  Create a new memory overlap column.
     *
     *      @return The created memory overlap column.
     */
    MemoryColumn* createMemoryOverlapColumn();

    /*!
     *  Create the memory items found in the design.
     */
    void createMemoryItems();

    /*!
     *  Create an address space item.
     *
     *      @param [in] spaceItem           Memory item containing address space data.
     *      @param [in] containingInstance  The component instance containing the selected address space.
     *      @param [in] containingColumn    The address space column.
     */
    void createAddressSpaceItem(QSharedPointer<MemoryItem> spaceItem,
        QSharedPointer<ConnectivityComponent> containingInstance, MemoryColumn* containingColumn);

    /*!
     *  Create a memory map item.
     *
     *      @param [in] mapItem             Memory item containing memory map data.
     *      @param [in] containingInstance  The component instance containing the selected memory map.
     *      @param [in] containingColumn    The memory map column.
     */
    void createMemoryMapItem(QSharedPointer<MemoryItem> mapItem,
        QSharedPointer<ConnectivityComponent> containingInstance, MemoryColumn* containingColumn);

    /*!
     *  Create the memory connections.
     */
    void createMemoryConnections();

    /*!
     *  Get the selected memory column.
     *
     *      @param [in] columnName  Name of the selected memory column.
     *
     *      @return The found memory column.
     */
    MemoryColumn* findColumnByName(QString const& columnName) const;

    /*!
     *  Create a memory connection between an address space and a memory map.
     *
     *      @param [in] connectionPath      The path of the memory connection.
     *      @param [in] placedMapItems      A list of placed memory map items.
     *      @param [in] memoryMapColumn     The column containing the memory maps.
     *      @param [in] spaceYPlacement     The Y placement of the next address space item.
     *      @param [in] placedSpaceItems    A list of placed address space items.
     *      @param [in] spaceColumn         The column containing the address spaces.
     */
    void createConnection(QVector<QSharedPointer<ConnectivityInterface> > connectionPath,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn,
        int& spaceYPlacement, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MemoryColumn* spaceColumn);

    /*!
     *  Check and reposition a memory map to a memory map overlap column if needed.
     *
     *      @param [in] placedMaps      A list of placed memory map items.
     *      @param [in] memoryItem      The selected memory map item.
     *      @param [in] originalColumn  The original column of the memory map item.
     *      @param [in] startAddress    The start address of the connection.
     *      @param [in] endAddress      The end address of the connection.
     */
    void checkMemoryMapRepositionToOverlapColumn(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps,
        MainMemoryGraphicsItem* memoryItem, MemoryColumn* originalColumn, QString const& startAddress,
        QString const& endAddress);

    /*!
     *  Check if a memory map overlaps another memory map in the same column.
     *
     *      @param [in] selectedMapRect         Bounding rectangle of the selected memory map item.
     *      @param [in] selectedMapPenWidth     Line width of the selected memory map item.
     *      @param [in] selectedExtensionRect   Bounding rectangle of the selected memory map extension.
     *      @param [in] comparisonMemoryItem    The compared memory map item.
     */
    bool memoryMapOverlapsAnotherMemoryMap(QRectF selectedMapRect, int selectedMapPenWidth,
        QRectF selectedExtensionRect, MainMemoryGraphicsItem* comparisonMemoryItem) const;

    /*!
     *  Reposition the selected memory map item.
     *
     *      @param [in] placedMapItems          List of the placed memory map items.
     *      @param [in] placedSpaceItems        List of the placed address space items.
     *      @param [in] startConnectionItem     The connection start item.
     *      @param [in] addressSpaceColumn      Column containing the address spaces.
     *      @param [in] endConnectionItem       The connection end item.
     *      @param [in] memoryMapYTransfer      Y transfer of the memory map.
     */
    void repositionMemoryMap(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MainMemoryGraphicsItem* startConnectionItem, MemoryColumn* addressSpaceColumn,
        MainMemoryGraphicsItem* endConnectionItem, int memoryMapYTransfer);

    /*!
     *  Reposition an address space item to match a selected memory map.
     *
     *      @param [in] placedSpaceItems    List of the placed address space items.
     *      @param [in] startItem           The connection start item.
     *      @param [in] endItem             The connection end item.
     *      @param [in] spaceColumn         Column containing the address spaces.
     *      @param [in] memoryMapYTransfer  Y transfer of the memory map.
     */
    void repositionSpaceItemToMemoryMap(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        MainMemoryGraphicsItem* startItem, MainMemoryGraphicsItem* endItem, MemoryColumn* spaceColumn,
        int memoryMapYTransfer);

    /*!
     *  Get the base address of the memory map.
     *
     *      @param [in] memoryItem  The selected memory map item.
     *
     *      @return The base address of the memory map item.
     */
    quint64 getMemoryMapBeginAddress(MainMemoryGraphicsItem* memoryItem) const;

    /*!
     *  Get the end address of the memory map.
     *
     *      @param [in] memoryItem  The selected memory map item.
     *
     *      @return The end address of the memory map item.
     */
    quint64 getMemoryMapEndAddress(MainMemoryGraphicsItem* memoryItem) const;

    /*!
     *  Redraw the memory connections.
     */
    void reDrawConnections();

    /*!
     *  Move an address space item.
     *
     *      @param [in] spaceItem           The selected address space item.
     *      @param [in] placedSpaceItems    List of placed address space items.
     *      @param [in] spaceColumn         Column containing the address spaces.
     *      @param [in] spaceYPlacement     Y placement of the address space item.
     */
    void moveAddressSpaceItem(MainMemoryGraphicsItem* spaceItem,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, MemoryColumn* spaceColumn,
        int& spaceYPlacement);

    /*!
     *  Place the address space item to another address space column.
     *
     *      @param [in] spaceItem       The selected address space item.
     *      @param [in] originalColumn  The original column of the selected address space item.
     *      @param [in] targetItem      The other item of the memory connection.
     *      @param [in] yTransfer       Y transfer of the address space item.
     */
    void placeSpaceItemToOtherColumn(MainMemoryGraphicsItem* spaceItem, MemoryColumn* originalColumn,
        MainMemoryGraphicsItem* targetItem, int yTransfer);

    /*!
     *  Extend a memory item.
     *
     *      @param [in] memoryGraphicsItem  The selected memory graphics item.
     *      @param [in] connectionItem      The connection of the selected memory graphics item.
     *      @param [in] spaceYplacement     Y placement of the address space item.
     */
    void extendMemoryItem(MainMemoryGraphicsItem* memoryGraphicsItem, MemoryConnectionItem* connectionItem,
        int& spaceYplacement);

    /*!
     *  Move the unconnected address space items to the bottom of the address space column.
     *
     *      @param [in] placedSpaceItems    List of the placed address space items.
     *      @param [in] spaceYPlacement     Y placement of the last address space item.
     *      @param [in] spaceColumn         Column containing the address space items.
     */
    void moveUnconnectedAddressSpaces(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        int& spaceYPlacement, MemoryColumn* spaceColumn);

    /*!
     *  Move the unconnected memory map items to the bottom of the memory map column.
     *
     *      @param [in] placedMapItems      List of the placed memory map items.
     *      @param [in] memoryMapColumn     Column containing the memory map items.
     */
    void moveUnconnectedMemoryMaps(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn* memoryMapColumn);

    /*!
     *  Create markers for the overlapping connections.
     *
     *      @param [in] placedSpaceItems    List of the placed address space items.
     */
    void createOverlappingConnectionMarkers(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems);

    /*!
     *  Check if an items bounding rectangle collides with another item.
     *
     *      @param [in] firstRectangle      Bounding rectangle of the selected item.
     *      @param [in] firstLineWidth      Line width of the selected item.
     *      @param [in] secondRectangle     Bounding rectangle of the comparison item.
     *      @param [in] secondLineWidth     Line width of the comparison item.
     */
    bool itemCollidesWithAnotherItem(QRectF firstRectangle, int firstLineWidth, QRectF secondRectangle,
        int secondLineWidth) const;

    /*!
     *  Compress the graphics items.
     *
     *      @param [in] placedSpaceItems    A list of the placed address space items.
     *      @param [in] spaceYPlacement     The last y-coordinate of the address space items.
     *      @param [in] spaceColumn   [Description].
     */
    void compressGraphicsItems(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
        int& spaceYPlacement, MemoryColumn* spaceColumn);

    /*!
     *  Reposition compressed memory maps to previous columns.
     *
     *      @param [in] placedMapItems      A list of the placed memory map items.
     *      @param [in] memoryMapColumn     The main memory map column.
     *      @param [in] spaceColumn         The address space column.
     */
    void repositionCompressedMemoryMaps(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
        MemoryColumn* memoryMapColumn, MemoryColumn* spaceColumn);

    /*!
     *  Check if a memory map collides.
     *
     *      @param [in] mapRectangle        Rectangle of the selected memory map.
     *      @param [in] mapPenWidth         Pen width used to draw the selected memory map item.
     *      @param [in] mapItem             The selected memory map item.
     *      @param [in] comparisonColumn    The column whose items are being compared to.
     *      @param [in] placedMapItems      A list of the placed memory map items.
     *
     *      @return True, if the memory map collides with memory maps in the selected column, false otherwise.
     */
    bool memoryMapCollidesWithMemoryMapsInColumn(QRectF mapRectangle, int mapPenWidth,
        MainMemoryGraphicsItem* mapItem, GraphicsColumn* comparisonColumn,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems);

    /*!
     *  Get the specified columns.
     *
     *      @param [in] columnSpecification     Identifier for the wanted columns.
     *
     *      @return A vector containing the specified columns.
     */
    QVector<MemoryColumn*> getSpecifiedColumns(QString const& columnSpecification);

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
    ConnectivityGraphFactory instanceLocator_;

    //! Width of the memory columns.
    static const int COLUMNWIDTH = 619;

    //! The created connection graph.
    QSharedPointer<ConnectivityGraph> connectionGraph_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNERDIAGRAM_H
