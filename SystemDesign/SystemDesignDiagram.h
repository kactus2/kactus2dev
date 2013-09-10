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


    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a single API/COM port.
    //-----------------------------------------------------------------------------
	struct PortCopyData
	{
        QString name;                              //!< The name of the interface.
        // Only one of these is valid at one time.
		QSharedPointer<ApiInterface> apiInterface; //!< API interface.
		QSharedPointer<ComInterface> comInterface; //!< COM interface.
        QPointF pos;                               //!< The original position.

		/*!
         *  Default constructor.
         */
        PortCopyData() : apiInterface(), comInterface()
        {
        }

        /*!
         *  Constructor.
         */
		PortCopyData(QSharedPointer<ApiInterface> apiInterface, QSharedPointer<ComInterface> comInterface)
            : apiInterface(apiInterface),
              comInterface(comInterface)
        {
        }
	};

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a collection of API/COM ports.
    //-----------------------------------------------------------------------------
    struct PortCollectionCopyData
    {
        QList<PortCopyData> ports;

        /*!
         *  Constructor.
         */
        PortCollectionCopyData()
            : ports()
        {
        }
    };

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a single component instance.
    //-----------------------------------------------------------------------------
    struct ComponentInstanceCopyData
    {
        QSharedPointer<Component> component;            //!< The referenced component.
        QString instanceName;                           //!< The instance name.
        QString displayName;                            //!< The display name.
        QString description;                            //!< The description of the instance.
        QPointF pos;                                    //!< Original position of the instance.
        QMap<QString, QString> propertyValues;          //!< Property values.
        QString fileSetRef;                             //!< File set reference.
        QMap<QString, QPointF> apiInterfacePositions;   //!< API interface positions.
        QMap<QString, QPointF> comInterfacePositions;   //!< COM interface positions.

        ComponentInstanceCopyData()
            : component(),
              instanceName(),
              displayName(),
              description(),
              propertyValues(),
              fileSetRef(),
              apiInterfacePositions(),
              comInterfacePositions()
        {
        }
    };

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a collection of copied component instances.
    //-----------------------------------------------------------------------------
    struct ComponentCollectionCopyData
    {
        QList<ComponentInstanceCopyData> instances;

        /*!
         *  Constructor.
         */
        ComponentCollectionCopyData()
            : instances()
        {
        }
    };

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a single column.
    //-----------------------------------------------------------------------------
    struct ColumnCopyData
    {
        ColumnDesc desc;                            //!< Column description.
        ComponentCollectionCopyData components;     //!< Components.
        PortCollectionCopyData interfaces;          //!< COM/API interfaces.

        /*!
         *  Constructor.
         */
        ColumnCopyData()
            : desc(),
              components()/*,
              interfaces()*/
        {
        }
    };

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a collection of columns.
    //-----------------------------------------------------------------------------
    struct ColumnCollectionCopyData
    {
        QList<ColumnCopyData> columns;

        /*!
         *  Constructor.
         */
        ColumnCollectionCopyData()
            : columns()
        {
        }
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

    /*!
     *  Called when copy is selected from the context menu.
     */
	void onCopyAction();

    /*!
     *  Called when paste is selected from the context menu.
     */
	void onPasteAction();


    /*!
     *  Called when add to library is selected from the context menu.
     */
	virtual void onAddAction();

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

    //! Called when a key has been pressed.
    void keyPressEvent(QKeyEvent *event);

    //! Called when a key has been released.
    void keyReleaseEvent(QKeyEvent *event);

    /*!
     *  Creates the context menu for function contextMenuEvent().
     *
     *      @param [in] pos Mouse position when the menu is requested.
	 *
	 *      @return The menu with allowed actions or 0 if no menu is allowed or nothing to show.
     */
	virtual QMenu* createContextMenu(QPointF const& pos);

    void prepareContextMenuActions();

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
     *  Returns the HW component instance with the given UUID.
     */
    HWMappingItem* getHWComponentByUUID(QString const& uuid);

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
     *  Updates the drop action for a drag-dropped object.
     */
    void updateDropAction(QGraphicsSceneDragDropEvent* event);
    
    /*!
     *  Copies SW component instances in a format which can be saved to clipboard.
     *
     *      @param [in]  items       The component instance items to copy.
     *      @param [out] collection  The resulted collection of component instance copy data.
     */
    void copySWInstances(QList<QGraphicsItem*> const& items, ComponentCollectionCopyData &collection);

    /*!
     *  Copies interfaces in a format which can be saved to clipboard.
     *
     *      @param [in]  items       The interface items to copy.
     *      @param [out] collection  The resulted collection of interface copy data.
     */
    void copyInterfaces(QList<QGraphicsItem*> const& items, PortCollectionCopyData& collection);

    /*!
     *  Pastes component instances from a copy data collection.
     *
     *      @param [in] collection     The collection of component instance copy data.
     *      @param [in] column         The item stack where to place the instances.
     *      @param [in] cmd            The parent undo command for the paste undo commands.
     *      @param [in] userCursorPos  If true, the instances are placed close to the cursor position.
     *                                 Otherwise the original positions are used.
     */
    void pasteSWInstances(ComponentCollectionCopyData const& collection,
                          IGraphicsItemStack* stack, QUndoCommand* cmd, bool useCursorPos);

    /*!
     *  Pastes interfaces from a copy data collection.
     *
     *      @param [in] collection  The collection of interface copy data.
     *      @param [in] targetComp  The target component. If null, the interfaces are pasted to the top level component.
     *      @param [in] cmd         The parent undo command for the paste undo commands.
     */
    void pasteInterfaces(PortCollectionCopyData const& collection, SWComponentItem* targetComp, QUndoCommand* cmd);

    /*!
     *  Pastes interfaces from a copy data collection to a top-level item stack.
     *
     *      @param [in] collection     The collection of interface copy data.
     *      @param [in] stack          The target item tack.
     *      @param [in] cmd            The parent undo command for the paste undo commands.
     *      @param [in] userCursorPos  If true, the interfaces are placed close to the cursor position.
     *                                 Otherwise the original positions are used.
     */
    void pasteInterfaces(PortCollectionCopyData const& collection, IGraphicsItemStack* stack, QUndoCommand* cmd,
                         bool useCursorPos);

    /*!
     *  Initializes the context menu actions.
     */
	void setupActions();

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

    //! Context menu copy action.
    QAction copyAction_;

    //! Context menu paste action.
    QAction pasteAction_;

    //! Context menu action for adding a draft component to library.
    QAction addAction_;

    //! If true, context menu is enabled.
    bool showContextMenu_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDESIGNDIAGRAM_H
