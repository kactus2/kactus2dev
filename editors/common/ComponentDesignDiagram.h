//-----------------------------------------------------------------------------
// File: ComponentDesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 23.6.2014
//
// Description:
// Base class for all component designs.
//-----------------------------------------------------------------------------

#ifndef COMPONENTDESIGNDIAGRAM_H
#define COMPONENTDESIGNDIAGRAM_H

#include <editors/common/DesignDiagram.h>

#include <QVector>
#include <QSharedPointer>
#include <QUndoCommand>
#include <QScrollBar>

class ComponentItem;
class ConnectionEndpoint;
class DesignWidget;
class GraphicsColumn;
class GraphicsConnection;
class LibraryInterface;
class IEditProvider;
class AutoConnectorItem;
class ComponentItemAutoConnector;

//-----------------------------------------------------------------------------
// ComponentDesignDiagram is a base class for component designs.
//-----------------------------------------------------------------------------
class ComponentDesignDiagram : public DesignDiagram
{
    Q_OBJECT

public:

    /*!
     *  Constructor.
     */
    ComponentDesignDiagram(LibraryInterface* lh, QSharedPointer<IEditProvider> editProvider, DesignWidget* parent = 0);

	//! \brief The destructor
	virtual ~ComponentDesignDiagram();

    /*!
     *  Clears the scene.
     */
    virtual void clearScene();

    /*!
     *  Sets the draw mode.
     */
    virtual void setMode(DrawMode mode);

    /*!
     *  Checks if a single item has been selected.
     *
     *      @return True, if only one item is selected, otherwise false.
     */
    bool singleSelection() const;

signals:

    /*!
     *  Delete the selected graphics items.
     */
    void deleteSelectedItems();

public slots:

    //! Called when the selection changes in the diagram.
    virtual void onSelectionChanged();
    
    //! Sets the selected connection above other connections.
    virtual void raiseSelectedConnectionToFront();

    //! Called when an item has been selected in the diagram. Updates the context help.
    virtual void onSelected(QGraphicsItem* newSelection) = 0;

	//! Called when copy is selected from the context menu.
	virtual void onCopyAction() = 0;

	//!  Called when paste is selected from the context menu.
	virtual void onPasteAction() = 0;

	//! Called when add to library is selected from the context menu.
    virtual void onAddToLibraryAction() = 0;

	//! Called when open component is selected from the context menu.
	virtual void onOpenComponentAction();

    /*!
     *  Called when open design is selected from the context menu.
     *
     *      @param [in] selectedAction   The action used to select the opened design.
     */
    virtual void onOpenDesignAction(QAction* selectedAction);

    /*!
     *  Called when the auto connected is selected from the context menu.
     */
    virtual void onOpenAutoConnector();

protected:

    /*!
     *  Opens a design for a given component.
     *
     *      @param [in] component   The component whose design to open.
     *      @param [in] viewName    The name of the view to open.
     */
    virtual void openDesignForComponent(ComponentItem* component, QString const& viewName) = 0;

    //! Handler for mouse click events.
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //! Handler for mouse move events.
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //! Handler for mouse release events.
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //! Called when a key has been release.
    virtual void keyReleaseEvent(QKeyEvent *event);

    //! Ends the drawing of current connection.
    void endConnect();

    //! Called when mouse moves while dragging an item. Delegates to updateDropAction.
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);

    //! Updates the dropAction and highlight according to underlying element.
    virtual void updateDropAction(QGraphicsSceneDragDropEvent* event) = 0;
   
	/*!
     *  Creates the context menu for function contextMenuEvent().
     *
     *      @param [in] pos Mouse position when the menu is requested.
	 *
	 *      @return The menu with allowed actions or 0 if no menu is allowed or nothing to show.
     */
	virtual QMenu* createContextMenu(QPointF const& pos);

    /*!
     *  Updates the open design sub-menu according to item views.
     *
     *      @param [in] compItem   The component item whose views to display in the sub-menu.
     */
    virtual void updateOpenDesignMenuFor(ComponentItem* compItem);

    //! Enables/disable context menu actions based on the current selection.
    virtual void prepareContextMenuActions();

    /*!
     *  Checks if add to library action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool addToLibraryActionEnabled() const;

    /*!
     *  Checks if open component action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool openComponentActionEnabled() const = 0;

    /*!
     *  Check if the delete action can be enabled.
     *
     *      @return True, if the delete action should be enabled, otherwise false.
     */
    virtual bool deleteActionEnabled() const;

    /*!
     *  Check if the selected items are of the correct type for the design diagram.
     *
     *      @return True, if the selected items are used in the design, false otherwise.
     */
    virtual bool selectedItemIsCorrectType() const = 0;

    /*!
     *  Checks if open design action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool openDesignEnabled() const;

    /*!
     *  Checks if the auto connector action is enabled.
     *
     *      @return True, if the action is enabled, otherwise false.
     */
    virtual bool autoConnectorEnabled() const;

    /*!
     *  Checks if the given item is a hierarchical component.
     *
     *      @param [in] item   The item to check.
     *
     *      @return True, if item is a hierarchical component, otherwise false.
     */
    virtual bool isHierarchicalComponent(QGraphicsItem* item) const = 0;

    /*!
     *  Checks if copy action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool copyActionEnabled() const = 0;

    /*!
     *  Checks if paste action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool pasteActionEnabled() const = 0;

    /*!
     *  Checks if current selection is a draft component.
     *
     *      @return Ture, if the current selection is a draft component, otherwise false.
     */
    bool draftSelected() const;

    /*!
     *  Opens the given component in component editor or design according to the active view of the component.
     *
     *      @param [in] comp   The component to open.
     */
    void openComponentItem(ComponentItem* comp);

    /*!
     *  Opens the given component in component editor.
     *
     *      @param [in] comp   The component to open.
     */
    void openInComponentEditor(ComponentItem* comp);

    /*!
     *  Opens the given component according to the active view of the component.
     *
     *      @param [in] comp   The component to open.
     */
    virtual void openComponentByActiveView(ComponentItem* comp) = 0;

   /*!
    *  Gets the names of hierarchical views of a component.
    *
    *      @param [in] component   The component whose hierarchical views to get.
    *
    *      @return The names of the hierarchical views.
    */
   virtual QStringList hierarchicalViewsOf(ComponentItem* component) const = 0;

    /*!
     *  Gets the name of the active view for the given component item.
     *
     *      @param [in] compItem   The item whose active view to get.
     *
     *      @return The name of the active view.
     */
    QString getActiveViewOf(ComponentItem* compItem) const;

    /*!
     *  Get the active view for the item containing the selected end point item.
     *
     *      @param [in] endPoint    The selected end point item.
     *
     *      @return The active view for the item containing the selected end point item.
     */
    QString getActiveViewForEndPoint(ConnectionEndpoint* endPoint) const;

    /*!
     *  Gets the position where context menu was activated.     
     *
     *      @return The position of the context menu activation.
     */
    QPoint contextMenuPosition() const;

    /*!
     *  Gets the graphics item type of the components in the diagram.
     *     
     *      @return The type of the components.
     */
    virtual int componentType() const = 0;

    /*!
     *  Gets the graphics item type of the off page connectors in the diagram.
     *     
     *      @return The type of the components.
     */
    virtual int offpageConnectorType() const = 0;

    /*!
     *  Gets the graphics item type of the connections in the diagram.
     *     
     *      @return The type of the components.
     */
    virtual int connectionType() const = 0;

    /*!
     *  Find the cursor position mapped to the current scene.
     *
     *      @return The cursor position mapped to the current scene.
     */
    QPointF findCursorPositionMappedToScene() const;

    /*!
     *  Get the visible name for the selected component item.
     *
     *      @param [in] item    The selected component item.
     *
     *      @return Visible name of the selected component item.
     */
    QString getVisibleNameForComponentItem(ComponentItem* item) const;

private:
    // Disable copying.
    ComponentDesignDiagram(ComponentDesignDiagram const& rhs);
    ComponentDesignDiagram& operator=(ComponentDesignDiagram const& rhs);

    //!  Initializes the context menu actions.     
	void setupActions();

    /*!
     *  Destroys all connections in the diagram.
     */
    void destroyConnections();

    /*!
     *  Handler for connection tool clicks. Creates a connection to the given position by either 
     *  beginning a new connection or ending currently drawn connection to it.     
     *
     *      @param [in] cursorPosition      The position to connect.
     */
    void connectAt(QPointF const& cursorPosition);

    /*!
     *  Checks if a connection is being drawn.
     *     
     *      @return True, if a connection is being drawn, otherwise false.
     */
    bool creatingConnection() const;

    /*!
     *  Sets the starting end point for a new connection. If offpage mode is on, the previous starting point
     *  is used, otherwise the starting point is selected based on the clicked cursor position.
     *
     *      @param [in] cursorPosition   The cursor position.     
     */
    void setConnectionStartingPoint(QPointF const& cursorPosition);

    /*!
     *  Checks if the starting point of current connection is set.
     *
     *      @return True, if the staring point has been set, otherwise false.
     */
    bool hasConnectionStartingPoint() const;

   /*!
    *  Begins drawing a connection from the given point.
    *
    *      @param [in] startingPoint   The point to start the connection from.    
    */
    void beginCreateConnection(QPointF const& startingPoint);

    //! Highlights all endpoints that can be connected to the current connection.     
    void highlightConnectableEndpoints();

    /*!
     *  Checks if an endpoint can be used as the ending point for current connection.
     *
     *      @param [in] endpoint   The endpoint to check.
     *
     *      @return True, if endpoint can be connected to the current connection, otherwise false.
     */
    bool isPossibleEndpointForCurrentConnection(ConnectionEndpoint* endpoint) const;

    /*!
     *  Creates the currently drawn connection by ending it to the given point.
     *
     *      @param [in] event The ending point.
     */
    void endConnectionTo(QPointF const& point);

    /*!
     *  Creates a connection between the given two endpoints.
     *
     *      @param [in] startPoint  The starting connection end point.
     *      @param [in] endPoint    The ending connection end point.
     *      @param [in] isOffPage   Determines if connection to be created is off-page or not.
     *      @return The created connection.
     */
    virtual GraphicsConnection* createConnection(ConnectionEndpoint* startPoint, ConnectionEndpoint* endPoint, bool isOffPage = false) = 0;

    /*!
     *  Creates a connection between the given endpoint and a coordinate point.
     *
     *      @param [in] startPoint  The starting connection end point.
     *      @param [in] endPoint    The ending coordinate point.
     *
     *      @return The created connection.
     */
    virtual GraphicsConnection* createConnection(ConnectionEndpoint* startPoint, QPointF const& endPoint) = 0;

    /*!
     *  Creates an add command for a given connection.
     *
     *      @param [in] connection  The connection to create a command for.     
     *
     *      @return The created add command.
     */
    virtual QUndoCommand* createAddCommandForConnection(GraphicsConnection* connection, QUndoCommand* parentCommand = nullptr) = 0;
    
    /*!
     *	Get the connection between two given endpoints.
     *  
     *      @param [in] startPoint  The start endpoint.
     *      @param [in] endPoint    The ending endpoint.   
     *	    
     * 	    @return The connection between the endpoints, or nullptr if not found or if either endpoint doesn't exist.
     */
    [[nodiscard]] virtual GraphicsConnection* getConnectionBetweenEndpoints(ConnectionEndpoint* startPoint, ConnectionEndpoint* endPoint);

    /*!
     *  Checks if the ending point of current connection is set.
     *
     *      @return True, if the ending point has been set, otherwise false.
     */
    bool hasConnectionEndingPoint() const;

    //! Deletes the currently drawn connection.
    void discardConnection();

    //! Removes all highlights.
    void disableHighlights();

    //! Clears the highlighting of the currently highlighted endpoint.
    void clearHighlightedEndpoint();

    //! Removes highlight from possible endpoints for current connection.
    void clearPotentialEndpoints();

    /*!
     *  Handler for interface tool clicks.
     *
     *      @param [in] position   The interface position.
     */
    void addInterfaceAt(QPointF const& position);

    /*!
     *  Adds a new top-level interface to the given diagram column.
     *
     *      @param [in] column      The column where to add the interface.
     *      @param [in] pos         The interface position.
     *      @param [in] isDraft     Determines if Top level interface is draft or not.
     */
    virtual void addTopLevelInterface(GraphicsColumn* column, QPointF const& pos, bool isDraft) = 0;

    /*!
     *  Handler for draft tool clicks. Creates a draft component instance or a draft interface according to the
     *  clicked position.
     *
     *      @param [in] clickedPosition   The position to create the draft item to.     
     */
    virtual void draftAt(QPointF const& clickedPosition) = 0;

    /*!
     *  Handler for offpage tool clicks. Toggles the connection offpage mode for clicked interface or connection.
     *
     *      @param [in] position   The point to toggle at.     
     */
    void toggleOffPageAt(QPointF const& position);

    /*!
     *  Toggles the connection style of the given connection between normal and off-page style.
     *
     *      @param [in] conn      The connection.
     *      @param [in] parentCmd The parent undo command.
     */
    void toggleConnectionStyle(GraphicsConnection* conn, QUndoCommand* parentCmd);

    //! Hides all off-page connections.
    void hideOffPageConnections();

    /*!
     *  Begins replacing another component with a component at the given position.
     *
     *      @param [in] startpoint   The position to start replacing from.     
     */
    void beginComponentReplaceDrag(QPointF const& startpoint);

    /*!
     *  Updates the cursor according to design content at the given position while replacing.
     *
     *      @param [in] cursorPosition   The cursor position.
     */
    void updateComponentReplaceDragCursor(QPointF const& cursorPosition);

    /*!
     *  Check if a component item is allowed in the column located at the selected position.
     *
     *      @param [in] cursorPosition  The selected position.
     *
     *      @return True, if the item is allowed in the column, false otherwise.
     */
    bool componentItemIsAllowedInColumnAtPosition (QPointF const& cursorPosition) const;

    /*!
     *  Ends the component replace at the given position. If another component is in the given position,
     *  it is replaced with the component being dragged.
     *
     *      @param [in] endpoint   The point to end the replacing.
     */
    void endComponentReplaceDrag(QPointF const& endpoint);

    /*!
     *  Performs the replacing of destination component with source component.
     *
     *      @param [in] destComp        The component to replace.
     *      @param [in] sourceComp      The replacing component.     
     */
    virtual void replace(ComponentItem* destComp, ComponentItem* sourceComp) = 0;

    /*!
     *  Updates the currently drawn connection and its highlights.
     *
     *      @param [in] cursorPosition   The position of the cursor.     
     */
    virtual void updateConnectionDisplay(QPointF const& cursorPosition);

    /*!
     *  Updates the drawing of current connection.
     *
     *      @param [in] cursorPosition   The position of the cursor. Connection ends to it.
     */
    virtual void updateConnectionDraw(QPointF const& cursorPosition);

    /*!
     *  Updates highlighting of possible endpoints for a new connection.
     *
     *      @param [in] cursorPosition   The position of the cursor. Connection begins from it.
     */
    virtual void highlightEndpointUnderCursor(QPointF const& cursorPosition);

    /*!
     *  Updates the highlighting of endpoint near the cursor for current connection.
     *
     *      @param [in] cursorPosition   The position of the cursor.
     */
    void highlightPotentialEndpointUnderCursor(QPointF const& cursorPosition);

    /*!
     *  Removes highlight from the current connection end point.
     */
    void disableCurrentHighlight();

    /*!
     *  Setup the text for the auto connector action.
     *
     *      @param [in] componentItem   The target of the connection.
     */
    void setupAutoconnectText(ComponentItem* componentItem);

    /*!
     *  Get the connection end point for the selected auto connector item.
     *
     *      @param [in] connectorItem   The selected auto connector item.
     *
     *      @return The end point item for the selected auto connector item.
     */
    virtual ConnectionEndpoint* getEndPointForItem(AutoConnectorItem* connectorItem, QUndoCommand* parentUndoCommand = nullptr) = 0;

    /*!
     *  Create connection between the selected end points.
     *
     *      @param [in] startPoint  Start point for the connection.
     *      @param [in] endPoint    End point for the connection.
     */
    virtual void createConnectionBetweenEndPoints(ConnectionEndpoint* startPoint,
        ConnectionEndpoint* endPoint) = 0;

    /*!
     *  Check if the auto connector should be added to the context menu.
     *
     *      @return True, if the auto connected should be added, false otherwise.
     */
    virtual bool addAutoConnectorActionToContextMenu() const;

    /*!
     *  Create auto connection dialog.
     *
     *      @param [in] firstItem   The first item for the auto connector.
     *
     *      @return The auto connector dialog.
     */
    virtual ComponentItemAutoConnector* createAutoConnector(ComponentItem* firstItem) const = 0;

    /*!
     *  Ensure visibility of a moving item.
     *
     *      @param [in] mouseEvent  The mouse event.
     */
    void ensureMovedItemVisibility(QGraphicsSceneMouseEvent* mouseEvent);

    /*!
     *  Get the margin for keeping selection in view.
     *	
     *      @param [in] scrollBar       Scrollbar for margin calculations.
     *      @param [in] position        Current position on the same axis as the scrollbar.
     *      @param [in] availableArea   The maximum coordinates of the area.
     *
     *      @return Current margin for the current selection.
     */
    qreal getMarginForVisibility(QScrollBar const* scrollBar, int const& position, qreal const& availableArea) const;

    /*!
     *  Get the margin modifier for the current margin.
     *	
     *      @param [in] scrollBar       Scrollbar for margin calculations.
     *      @param [in] position        Current position on the same axis as the scrollbar.
     *      @param [in] availableArea   The maximum coordinates of the area.
     *
     *      @return Modifier for the margin, between 0 and 1.
     */
    qreal getMarginModifierForVisibility(QScrollBar const* scrollBar, int const& position, qreal const& availableArea) const;

    /*!
     *  Get the clicked position modifier for visibility calculation. 
     *	
     *      @param [in] clickStartPosition  Coordinates of the clicked position.
     *      @param [in] followWidth         Width of the area to be visible.
     *      @param [in] availableWidth      Available width of the area.
     *      @param [in] margin              The margin for visibility check.
     *      @param [in] marginMultiplier    Multiplier for margin.
     *
     *      @return Modifier for visibility coordinates according to the position of the click.
     */
    qreal getPositionModifierForVisibility(qreal const& clickStartPosition, int const& followWidth, int const& availableWidth, qreal const& margin, qreal const& marginMultiplier) const;

    /*!
     *  Calculate coordinate modifiers for both x and y axis.
     */
    void calculatePositionVisibilityModifiers();

    /*!
     *	Removes pre-existing connections from list returned by connection editor.
     *  
     *      @param [in/out] originalConnections     List containing the original connections. 
     *                                              Gets modified to contain only connections to be removed.
     *      @param [in/out] updatedConnections      List containing the connections returned by connection editor. 
     *                                              Gets modified to contain only new connections.
     */
    void removeUnchangedConnectionEditorConnections(QList<QPair<AutoConnectorItem*, AutoConnectorItem*> >& originalConnections,
        QList<QPair<AutoConnectorItem*, AutoConnectorItem*> >& updatedConnections) const;

    /*!
     *	Creates add commands for given connections.
     *  
     *      @param [in] connections     Connections to create add commands for.
     *      @param [in] parentCommand   The parent command for the created add commands.
     */
    void createAddCommandsForGivenConnections(QList<QPair<AutoConnectorItem*, AutoConnectorItem*> > const& connections, QUndoCommand* parentCommand);

    /*!
     *	Creates remove commands for given connections.
     *
     *      @param [in] connections     Connections to create remove commands for.
     *      @param [in] parentCommand   The parent command for the created remove commands.
     */
    void createRemoveCommandsForGivenConnections(QList<QPair<AutoConnectorItem*, AutoConnectorItem*> > const& connections, QUndoCommand* parentCommand);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connection that is being drawn.
    GraphicsConnection* tempConnection_;

    //! The starting end point of a connection that is being drawn.
    ConnectionEndpoint* connectionStartPoint_;

    //! The potential end points that can be connected to the starting end point.
    QVector<ConnectionEndpoint*> tempPotentialEndingEndPoints_;

    //! The highlighted end point to which the connection could be snapped automatically.
    ConnectionEndpoint* connectionEndPoint_;

    //! The component that is used to replace another component in replace mode.
    ComponentItem* sourceComp_;
    
    //! Old selected items.
    QList<QGraphicsItem*> previouslySelectedItems_;

    //! Context menu select all action.
    QAction selectAllAction_;

    //! Context menu copy action.
    QAction copyAction_;

    //! Context menu paste action.
    QAction pasteAction_;

    //! Context menu action for adding a draft component to library.
    QAction addAction_;

    //! Context menu action for opening a component.
    QAction openComponentAction_;

    //! Context menu action for deleting an item.
    QAction deleteAction_;

    //! Context menu sub-menu for opening a component design.
    QMenu openDesignMenu_;

    //! Context menu action for automatically connecting connection end points of two selected components.
    QAction openAutoConnector_;

    //! Cursor position where the user right-presses to open the context menu.
    QPoint clickedPosition_;

    //! Modifier for the x-axis in item visibility.
    qreal visibleModifierX_ = 0;

    //! Modifier for the y-axis in item visibility.
    qreal visibleModifierY_ = 0;

    //! Width of the visible rectangle in mouse position.
    int const VISIBLERECTANGLEWIDTH_ = 100;

    //! Height of the visible rectangle in mouse position.
    int const VISIBLEWRECTANGLEHEIGHT_ = 100;

    //! Multiplier for calculating the position of the mouse.
    int const VISIBLEMULTIPLIER_ = 20;

    //! The component item that was selected as the origin point of the context menu event.
    ComponentItem* contextMenuItem_;

    //! The pressed mouse button.
    Qt::MouseButton lastMousePress_;

    //! Check for moving item to be located in the right edge.
    bool lastSelectedItemIsAtRightEdge_;

    //! The endpoint which was last hovered over while creating a connection. Used to highligh the enpoint again
    //  after not connecting to it. 
    ConnectionEndpoint* lastHoveredEndpoint_ = nullptr;
};

#endif // COMPONENTDESIGNDIAGRAM_H
