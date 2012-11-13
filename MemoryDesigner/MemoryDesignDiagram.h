//-----------------------------------------------------------------------------
// File: MemoryDesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 14.9.2012
//
// Description:
// Declares the memory design diagram class.
//-----------------------------------------------------------------------------

#ifndef MEMORYDESIGNDIAGRAM_H
#define MEMORYDESIGNDIAGRAM_H

#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/IDFactory.h>
#include <common/KactusAttribute.h>
#include <common/DesignDiagram.h>

#include <QVector>

class MainWindow;
class Component;
class Design;
class LibraryInterface;
class VLNV;
class GenericEditProvider;
class MemoryDesignWidget;
class AddressSpaceItem;
class MemoryItem;
class BusInterface;

//-----------------------------------------------------------------------------
//! MemoryDesignDiagram class.
//-----------------------------------------------------------------------------
class MemoryDesignDiagram : public DesignDiagram
{
    Q_OBJECT

public:
    // Column width.
    enum
    {
        COLUMN_WIDTH = 219,
    };

    /*!
     *  Constructor.
     *
     *      @param [in] lh            The library interface.
     *      @param [in] mainWnd       The main window.
     *      @param [in] editProvider  The edit provider.
     *      @param [in] parent        The parent widget.
     */    
    MemoryDesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                        GenericEditProvider& editProvider, MemoryDesignWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~MemoryDesignDiagram();

    /*!
     *  Clears the scene.
     */
    virtual void clearScene();

    /*! 
     *  Creates a design based on the contents in the diagram.
     *
     *      @param [in] vlnv The vlnv for the design.
     *
     *      @return The created design.
     */
    virtual QSharedPointer<Design> createDesign(VLNV const& vlnv) const;

    /*!
     *  Reflects the changes in the design to the top-level component.
     */
    virtual void updateHierComponent();

    /*!
     *  Adds a new column to the diagram.
     *
     *      @param [in] desc The column description.
     */
    void addColumn(ColumnDesc const& desc);

    /*!
     *  Returns the parent widget.
     */
    MemoryDesignWidget* parent() const;

    /*!
     *  Returns the column layout.
     */
    GraphicsColumnLayout* getColumnLayout();

    void onVerticalScroll(qreal y);

    /*!
     *  Begins a resize of an address (sub)section.
     *  
     *      @param [in] dual    If true, two helper lines will be drawn.
     *      @param [in] bottom  The bottom coordinate for the section being resized.
     */
    void beginResizeSubsection(bool dual, qreal bottom);

    /*!
     *  Updates the resize of an address (sub)section.
     *  
     *      @param [in] bottom The bottom coordinate for the section being resized.
     */
    void updateResizeSubsection(qreal bottom);

    /*!
     *  Ends the resize of an address (sub)section.
     */
    void endResizeSubsection();

    /*!
     *  Returns true if the given address space is connected to the given memory map.
     *
     *      @param [in]  addrSpaceItem  The address space item.
     *      @param [in]  memoryItem     The memory item.
     *      @param [out] baseAddress    If non-null, the retrieved base address for the connection.
     *
     *      @return True if a connection was found; otherwise false.
     */
    bool isConnected(AddressSpaceItem const* addrSpaceItem, MemoryItem const* memoryItem,
                     quint64* baseAddress) const;

public slots:
    /*!
     *  Called when the selection changes in the diagram.
     */
    void onSelectionChanged();

protected:
    //! Called when the user presses a mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user moves the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release a mouse button.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the mouse is double-clicked.
    //void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //! Called when the mouse wheel has scrolled.
    void wheelEvent(QGraphicsSceneWheelEvent *event);

    //! Draws the foreground.
    void drawForeground(QPainter* painter, const QRectF& rect);

    void drawMemoryDividers(QPainter* painter, QRectF const& rect);

private:
    // Disable copying.
    MemoryDesignDiagram(MemoryDesignDiagram const& rhs);
    MemoryDesignDiagram& operator=(MemoryDesignDiagram const& rhs);

    /*!
     *  Called when an item has been selected in the diagram.
     *
     *      @param [in] newSelection The selected item.
     */
    virtual void onSelected(QGraphicsItem* newSelection);

    /*!
     *  Opens the system design for editing.
     */
    void loadDesign(QSharedPointer<Design> design);

    /*!
     *  Creates a default column object.
     */
    static GraphicsColumn* createDefaultColumn(GraphicsColumnLayout* layout, QGraphicsScene* scene);

    bool findRoute(QString const& instanceName, QSharedPointer<BusInterface const> busIf,
                   MemoryItem const* memoryItem, quint64& addressOffset) const;

    QSharedPointer<Component const> getComponentByInstanceName(QString const& componentRef) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent widget.
    MemoryDesignWidget* parent_;

    //! The design.
    QSharedPointer<Design> design_;

    // Graphics column layout.
    QSharedPointer<GraphicsColumnLayout> layout_;

    //! The old item selection.
    QGraphicsItem* oldSelection_;

    //! If true, an address (sub)section is being resized.
    bool resizingSubsection_;

    //! If true, the dual lines will be drawn for the subsection resize.
    bool dualSubsectionResize_;

    //! The current y coordinate for the subsection resize.
    qreal subsectionResizeBottom_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNDIAGRAM_H
