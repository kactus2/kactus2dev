//-----------------------------------------------------------------------------
// File: ComponentDesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.6.2014
//
// Description:
// Base class for all component designs.
//-----------------------------------------------------------------------------

#ifndef COMPONENTDESIGNDIAGRAM_H
#define COMPONENTDESIGNDIAGRAM_H

#include <designEditors/common/DesignDiagram.h>

#include <QVector>
#include <QSharedPointer>
#include <QUndoCommand>

class ComponentItem;
class GenericEditProvider;
class GraphicsColumn;
class DesignWidget;
class LibraryInterface;
class ConnectionEndpoint;
class GraphicsConnection;

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
    ComponentDesignDiagram(LibraryInterface* lh, GenericEditProvider& editProvider, DesignWidget* parent = 0);

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
     *  Checks if open design action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool openDesignEnabled() const;

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
    virtual unsigned int componentType() const = 0;

    /*!
     *  Gets the graphics item type of the off page connectors in the diagram.
     *     
     *      @return The type of the components.
     */
    virtual unsigned int offpageConnectorType() const = 0;

    /*!
     *  Gets the graphics item type of the connections in the diagram.
     *     
     *      @return The type of the components.
     */
    virtual unsigned int connectionType() const = 0;

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
    void setConnectionStaringPoint(QPointF const& cursorPosition);

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
    bool isPossibleEndpointForCurrentConnection(ConnectionEndpoint* endpoint);

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
     *
     *      @return The created connection.
     */
    virtual GraphicsConnection* createConnection(ConnectionEndpoint* startPoint, ConnectionEndpoint* endPoint) = 0;

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
    virtual QSharedPointer<QUndoCommand> createAddCommandForConnection(GraphicsConnection* connection) = 0;

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
     *      @param [in] column The column where to add the interface.
     *      @param [in] pos    The interface position.
     */
    virtual void addTopLevelInterface(GraphicsColumn* column, QPointF const& pos) = 0;

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

    //! Removes highlight from the current connection end point.
    void disableCurrentHighlight();

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

    //! Context menu sub-menu for opening a component design.
    QMenu openDesignMenu_;

    //! Cursor position where the user right-presses to open the context menu.
    QPoint clickedPosition_;

};

#endif // COMPONENTDESIGNDIAGRAM_H
