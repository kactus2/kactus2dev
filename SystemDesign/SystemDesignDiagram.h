//-----------------------------------------------------------------------------
// File: SystemDesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.4.2012
//
// Description:
// System design diagram to work as a drawing canvas.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDESIGNDIAGRAM_H
#define SYSTEMDESIGNDIAGRAM_H

#include "SWConnectionEndpoint.h"

#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/IDFactory.h>
#include <common/KactusAttribute.h>
#include <common/DesignDiagram.h>

#include <QVector>

class MainWindow;
class Component;
class Design;
class DesignConfiguration;
class LibraryInterface;
class VLNV;
class GenericEditProvider;
class SystemDesignWidget;
class HWMappingItem;
class SystemComponentItem;
class GraphicsConnection;
class SystemComponentItem;
class SWComponentItem;
class SWPortItem;
class SWInstance;

//-----------------------------------------------------------------------------
//! SystemDesignDiagram class.
//-----------------------------------------------------------------------------
class SystemDesignDiagram : public DesignDiagram
{
    Q_OBJECT

public:
    // Column widths.
    enum
    {
        SYSTEM_COLUMN_WIDTH = 319,
        SW_COLUMN_WIDTH = 259,
        IO_COLUMN_WIDTH = 119
    };

    /*!
     *  Constructor.
     *
     *      @param [in] lh            The library interface.
     *      @param [in] mainWnd       The main window.
     *      @param [in] editProvider  The edit provider.
     *      @param [in] parent        The parent widget.
     */
    SystemDesignDiagram(bool onlySW, LibraryInterface* lh, MainWindow* mainWnd,
                        GenericEditProvider& editProvider, SystemDesignWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemDesignDiagram();

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
     *  Sets the draw mode.
     */
    virtual void setMode(DrawMode mode);

    /*!
     *  Adds a new column to the diagram.
     *
     *      @param [in] desc The column description.
     */
    void addColumn(ColumnDesc const& desc);

    /*!
     * Returns the parent widget.
     */
    SystemDesignWidget* parent() const;

    /*!
     *  Returns the column layout.
     */
    GraphicsColumnLayout* getColumnLayout();

    void onVerticalScroll(qreal y);

public slots:
    //! Called when a component instance is added to the diagram.
    virtual void onComponentInstanceAdded(ComponentItem* item);

    //! Called when a component instance is remove from the diagram.
    virtual void onComponentInstanceRemoved(ComponentItem* item);

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
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //! Called when the mouse wheel has scrolled.
    void wheelEvent(QGraphicsSceneWheelEvent *event);

    //! Called when an drag enters the diagram.
    void dragEnterEvent(QGraphicsSceneDragDropEvent * event);

    //! Called when an object is dragged within the diagram.
    void dragMoveEvent(QGraphicsSceneDragDropEvent * event);

    //! Called when the drag leaves the diagram.
    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);

    //! Called when an object is dropped to the diagram.
    void dropEvent(QGraphicsSceneDragDropEvent *event);

    //! Called when a key has been released.
    void keyReleaseEvent(QKeyEvent *event);

private:
    // Disable copying.
    SystemDesignDiagram(SystemDesignDiagram const& rhs);
    SystemDesignDiagram& operator=(SystemDesignDiagram const& rhs);

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
     *  Loads the COM connections from the given design.
     */
    void loadComConnections(QSharedPointer<Design> design);

    /*!
     *  Loads the API dependencies from the given design.
     */
    void loadApiDependencies(QSharedPointer<Design> design);

    /*!
     *  Returns the HW component instance with the given name.
     */
    HWMappingItem* getHWComponent(QString const& instanceName);

    /*!
     *  Returns the component with the given name (either HW or SW).
     *
     *      @param [in] instanceName The name of the instance to search for.
     *
     *      @return The corresponding component, or null if not found.
     */
    SystemComponentItem* getComponent(QString const& instanceName);

    /*!
     *  Creates the currently drawn connection.
     *
     *      @param [in] event The ending mouse press event.
     */
    void createConnection(QGraphicsSceneMouseEvent* event);

    /*!
     *  Discards and destroys the temporary (currently drawn) connection.
     */
    void discardConnection();

    /*!
     *  Destroys all connections in the diagram.
     */
    void destroyConnections();

    /*!
     *  Disables all highlights.
     */
    void disableHighlights();

    /*!
     *  Disables the current highlight. In case of a potential ending endpoint, the highlight is
     *  set to allowed state.
     */
    void disableCurrentHighlight();

    /*!
     *  Creates a missing port to the given component item.
     *
     *      @param [in] portName   The name of the port to create.
     *      @param [in] type       The port endpoint type.
     *      @param [in] component  The parent component.
     *      @param [in] design     The design containing related information.
     */
    SWPortItem* createMissingPort(QString const& portName, ConnectionEndpoint::EndpointType type,
                                  SystemComponentItem* component, QSharedPointer<Design> design);

    /*!
     *  Hides all off-page connections.
     */
    void hideOffPageConnections();

    /*!
     *  Ends connection mode.
     */
    void endConnect();

    /*!
     *  Toggles the connection style of the given connection between normal and off-page style.
     *
     *      @param [in] conn      The connection.
     *      @param [in] parentCmd The parent undo command.
     */
    void toggleConnectionStyle(GraphicsConnection* conn, QUndoCommand* parentCmd);

    /*!
     *  Imports SW instances and related connections from the given design to the diagram.
     *
     *      @param [in] design    The design to import.
     *      @param [in] stack     The stack where to place the imported elements.
     *      @param [in] guidePos  Position used as a guide where to place the imported instances.
     */
    void importDesign(QSharedPointer<Design> design, IGraphicsItemStack* stack, QPointF const& guidePos);

    /*!
     *  Recalls port positions for an SW instance.
     *
     *      @param [in] instance The instance metadata.
     *      @param [in] item     The corresponding graphics item in the diagram.
     */
    void recallPortPositions(SWInstance const &instance, SWComponentItem* item);

    /*!
     *  Updates the drop action for a drag-dropped object.
     */
    void updateDropAction(QGraphicsSceneDragDropEvent* event);

    //-----------------------------------------------------------------------------
    //! Drag type enumeration.
    //-----------------------------------------------------------------------------
    enum DragType
    {
        DRAG_TYPE_NONE = 0,
        DRAG_TYPE_SW,
        DRAG_TYPE_HW,
        DRAG_TYPE_DEFINITION,
        DRAG_TYPE_DESIGN
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! If true, the widget is used for editing SW designs.
    bool onlySW_;

    //! The parent widget.
    SystemDesignWidget* parent_;

    // Graphics column layout.
    QSharedPointer<GraphicsColumnLayout> layout_;

    //! Indicates what type of object is being dragged into the diagram.
    DragType dragType_;

    //! The connection that is being drawn.
    GraphicsConnection* tempConnection_;

    //! The starting endpoint of a connection that is being drawn.
    SWConnectionEndpoint* tempConnEndpoint_;

    //! The potential endpoints that can be connected to the starting end point.
    QVector<SWConnectionEndpoint*> tempPotentialEndingEndpoints_;
    
    //! The highlighted endpoint to which the connection could be snapped automatically.
    SWConnectionEndpoint* highlightedEndpoint_;

    //! If true, the off-page connection mode is active.
    bool offPageMode_;

    //! If true, we're in replace component mode.
    bool replaceMode_;

    //! The component that is used to replace another component in replace mode.
    SystemComponentItem* sourceComp_;

    //! The old item selection.
    QGraphicsItem* oldSelection_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDESIGNDIAGRAM_H
