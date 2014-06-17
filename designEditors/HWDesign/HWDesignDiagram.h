/* 
 *
 * 		filename: HWDesignDiagram.h
 */

#ifndef HWDESIGNDIAGRAM_H
#define HWDESIGNDIAGRAM_H

#include "AdHocEnabled.h"

#include <designEditors/common/DesignDiagram.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/ColumnDesc.h>

#include <QMap>
#include <QVector>
#include <QSharedPointer>
#include <QUndoCommand>

class LibraryInterface;
class Component;
class Design;
class DesignConfiguration;
class HWComponentItem;
class ComponentItem;
class BusPortItem;
class BusInterfaceItem;
class AdHocPortItem;
class AdHocInterfaceItem;
class HWConnection;
class HWConnectionEndpoint;
class GraphicsColumn;
class GraphicsColumnLayout;
class AbstractionDefinition;
class VLNV;
class GenericEditProvider;
class HWDesignWidget;
class ConnectionEndpoint;
class GraphicsConnection;

/*! \brief HWDesignDiagram is a graphical view to a design
 *
 */
class HWDesignDiagram : public DesignDiagram
{
    Q_OBJECT

public:
    // Column widths.
    enum
    {
        COMPONENT_COLUMN_WIDTH = 259,
        IO_COLUMN_WIDTH = 119
    };
		

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a single bus interface instance.
    //-----------------------------------------------------------------------------
    struct BusInterfaceCopyData
    {
        QSharedPointer<Component> srcComponent;         //!< The origin component.
        QSharedPointer<BusInterface> busInterface;      //!< The bus interface.
        General::InterfaceMode mode;                    //!< The bus interface mode.
        QString instanceName;                           //!< The bus instance name.
        QString description;                            //!< The description of the instance.
        QPointF pos;                                    //!< Original position of the instance.
        bool topLevelIf;                                //!< Top-level or bus port interface.

        /*!
        *  Constructor.
        */
        BusInterfaceCopyData()
            : srcComponent(),
            busInterface(),
            mode(),
            instanceName(),
            description(),
            topLevelIf()
        {
        }
    };

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a collection of copied bus interface instances.
    //-----------------------------------------------------------------------------
    struct BusInterfaceCollectionCopyData
    {
        QList<BusInterfaceCopyData> instances;

        /*!
        *  Constructor.
        */
        BusInterfaceCollectionCopyData()
            : instances()
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
        QMap<QString, QString> configurableElements;    //!< Configurable element values.
        QMap<QString, QPointF> busInterfacePositions;   //!< Bus interface positions.
        QMap<QString, QPointF> adHocPortPositions;      //!< Ad-hoc port positions.
        QMap<QString, bool> adHocVisibilities;          //!< Ad-hoc visibilities.

        ComponentInstanceCopyData()
            : component(),
              instanceName(),
              displayName(),
              description(),
              configurableElements(),
              busInterfacePositions(),
              adHocPortPositions(),
              adHocVisibilities()
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
        BusInterfaceCollectionCopyData interfaces;  //!< Top-level bus interfaces.

        /*!
         *  Constructor.
         */
        ColumnCopyData()
            : desc(),
              components(),
              interfaces()
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
     */
    HWDesignDiagram(LibraryInterface *lh, GenericEditProvider& editProvider, HWDesignWidget *parent = 0);

	//! \brief The destructor
	virtual ~HWDesignDiagram();

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
     *  Sets the draw mode.
     */
    virtual void setMode(DrawMode mode);


    /*! \brief Set the IP-XACT document that is viewed in HWDesignDiagram
     *
     */
    void loadDesign(QSharedPointer<Design> design);

    /*! \brief Get HWComponentItem that has the given instance name
     *
     */
    HWComponentItem *getComponent(const QString &instanceName);

	/*! \brief Get the component instances contained in this scene.
	 *
	 * \return QList containing pointers to the component instances.
	*/
	QList<ComponentItem*> getInstances() const;

    /*!
     *  Changes the state of a visibility control.
     *
     *      @param [in] name   The name of the visibility control.
     *      @param [in] state  The new state for the visibility control.
     */
    virtual void setVisibilityControlState(QString const& name, bool state);

	/*!
     *  Reflects the changes in the design to the top-level component.
     */
    virtual void updateHierComponent();

    /*!
     *  Adds a column to the diagram's layout.
     *
     *      @param [in] desc The column description.
     */
    void addColumn(ColumnDesc const& desc);

    /*!
     *  Removes a column from the diagram's layout.
     *
     *      @param [in] column The column to remove.
     */
    void removeColumn(GraphicsColumn* column);

    /*! \brief Get pointer to the parent of this scene.
	 *
	 * \return Pointer to the design widget that owns this scene.
	*/
	virtual HWDesignWidget* parent() const;

    /*!
     *  Called when a port's ad-hoc visibility has been changed.
     *
     *      @param [in] portName  The name of the port.
     *      @param [in] visible   The new ad-hoc visibility.
     */
    virtual void onAdHocVisibilityChanged(QString const& portName, bool visible);

    /*!
     *  Returns the ad-hoc port with the given name or null if not found.
     */
    virtual HWConnectionEndpoint* getDiagramAdHocPort(QString const& portName);

public slots:

    /*!
     *  Brings the selected item to front.
     */
    void selectionToFront();

    /*!
     *  Called when the selection changes in the diagram.
     */
    void onSelectionChanged();

	/*!
     *  Called when copy is selected from the context menu.
     */
	virtual void onCopyAction();

    /*!
     *  Called when paste is selected from the context menu.
     */
	virtual void onPasteAction();

    /*!
     *  Called when add to library is selected from the context menu.
     */
	virtual void onAddToLibraryAction();

    /*!
     *  Called when open component is selected from the context menu.
     */
	virtual void onOpenComponentAction();

    /*!
     *  Called when open HW design is selected from the context menu.
     */
	virtual void onOpenDesignAction();

protected:
    //! Handler for mouse click events.
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //! Handler for mouse move events.
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //! Handler for mouse release events.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //! Handler for mouse double click events.
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //! Called when a key has been release.
    void keyReleaseEvent(QKeyEvent *event);

    //! Ends the drawing of current connection.
    void endConnect();

    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);

    void updateDropAction(QGraphicsSceneDragDropEvent* event);

    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);

	/*!
     *  Creates the context menu for function contextMenuEvent().
     *
     *      @param [in] pos Mouse position when the menu is requested.
	 *
	 *      @return The menu with allowed actions or 0 if no menu is allowed or nothing to show.
     */
	virtual QMenu* createContextMenu(QPointF const& pos);

private:
    // Disable copying.
    HWDesignDiagram(HWDesignDiagram const& rhs);
    HWDesignDiagram& operator=(HWDesignDiagram const& rhs);

    /*!
     *  Called when an item has been selected in the diagram.
     *
     *      @param [in] newSelection The selected item.
     */
    virtual void onSelected(QGraphicsItem* newSelection);

    /*!
     *  Toggles the connection style of the given connection between normal and off-page style.
     *
     *      @param [in] conn      The connection.
     *      @param [in] parentCmd The parent undo command.
     */
    void toggleConnectionStyle(GraphicsConnection* conn, QUndoCommand* parentCmd);

    /*!
     *  Hides all visible off-page connections.
     */
    void hideOffPageConnections();

    /*!
     *  Destroys all connections in the diagram.
     */
    void destroyConnections();

    /*!
     *  Handler for connection tool clicks. Creates a connection to the given position by either 
     *  beginning a new connection or ending currently drawn connection to it.     
     *
     *      @param [in] cursorPosition      The position to connect.
     *      @param [in] setOffPageMode      If true, offpage mode is set for the new connection.
     */
    void connectAt(QPointF const& cursorPosition);

    /*!
     *  Sets the starting end point for a new connection. If offpage mode is on, the previous starting point
     *  is used, otherwise the starting point is selected based on the clicked cursor position.
     *
     *      @param [in] cursorPosition   The cursor position.     
     */
    void setConnectionStaringPoint(QPointF const& cursorPosition);

    /*!
     *  Checks if a connection is being drawn.
     *     
     *      @return True, if a connection is being drawn, otherwise false.
     */
    bool creatingConnection();

    /*!
     *  Creates the currently drawn connection by ending it to the given point.
     *
     *      @param [in] event The ending point.
     */
    void endConnectionTo(QPointF const& point);

    //! Removes highlight from possible endpoints for current connection.
    void clearPotentialEndpoints();

    /*!
     *  Checks if an endpoint can be used as the ending point for current connection.
     *
     *      @param [in] endpoint   The endpoint to check.
     *
     *      @return True, if endpoint can be connected to the current connection, otherwise false.
     */
    bool isPossibleEndpointForCurrentConnection(ConnectionEndpoint* endpoint);

    //! Deletes the currently drawn connection.
    void removeCurrentConnection();
    
   /*!
    *  Begins drawing a connection from the given point.
    *
    *      @param [in] startingPoint   The point to start the connection from.    
    */
    void beginCreateConnection(QPointF const& startingPoint);

    //! Highlights all endpoints that can be connected to the current connection.     
    void highlightConnectableEndpoints();

    /*!
     *  Handler for interface tool clicks.
     *
     *      @param [in] position   The interface position.
     */
    void addInterfaceAt(QPointF const& position);

    /*!
     *  Adds a new interface to the given diagram column.
     *
     *      @param [in] column The column where to add the interface.
     *      @param [in] pos    The interface position.
     */

    void addInterface(GraphicsColumn* column, QPointF const& pos);

    /*!
     *  Handler for draft tool clicks. Creates a draft component instance or a draft interface according to the
     *  clicked position.
     *
     *      @param [in] clickedPosition   The position to create the draft item to.     
     */
    void draftAt(QPointF const& clickedPosition);

    /*!
     *  Finds the item types for the given column. If the types are ambiguous, asks the user for types.
     *
     *      @param [in] column   The column to find the types for.
     *
     *      @return The column types.
     */
    unsigned int findColumnItemType(GraphicsColumn* column) const;

    /*!
     *  Adds a draft component instance to the design.
     *
     *      @param [in] column      The column to add the instance to.
     *      @param [in] position    The initial position to add the instance at.     
     */
    void addDraftInstance(GraphicsColumn* column, QPointF const& position);

    /*!
     *  Adds a draft interface in a draft component instance.
     *
     *      @param [in] targetComponent     The component item to add the interface to.
     *      @param [in] position            The initial position of the interface.     
     */
    void addDraftInterface(HWComponentItem* targetComponent, QPointF const& position);

    /*!
     *  Handler for offpage tool clicks. Toggles the connection offpage mode for clicked interface or connection.
     *
     *      @param [in] position   The point to toggle at.     
     */
    void toggleOffPageAt(QPointF const& position);

    /*!
     *  Begins replacing another component with a component at the given position.
     *
     *      @param [in] startpoint   The position to start replacing from.     
     */
    void beginComponentReplace(QPointF const& startpoint);

    /*!
     *  Updates the cursor according to design content at the given position while replacing.
     *
     *      @param [in] cursorPosition   The cursor position.
     */
    void updateComponentReplaceCursor(QPointF const& cursorPosition);

    /*!
     *  Ends the component replace at the given position. If another component is in the given position,
     *  it is replaced with the component being dragged.
     *
     *      @param [in] endpoint   The point to end the replacing.
     */
    void endComponentReplace(QPointF const& endpoint);

    /*!
     *  Updates the highlights and drawing of current connection.
     *
     *      @param [in] cursorPosition   The position of the cursor.     
     */
    void updateConnectionDisplay(QPointF const& cursorPosition);

    /*!
     *  Updates the highlighting of endpoint near the cursor for current connection.
     *
     *      @param [in] cursorPosition   The position of the cursor.
     */
    void updateConnectionHighlight(QPointF const& cursorPosition);

    /*!
     *  Updates the drawing of current connection.
     *
     *      @param [in] cursorPosition   The position of the cursor. Connection ends to it.
     */
    void updateCurrentConnection(QPointF const& cursorPosition);

    /*!
     *  Updates highlighting of possible endpoints for a new connection.
     *
     *      @param [in] cursorPosition   The position of the cursor. Connection begins from it.
     */
    void updateHighlightForNewConnection(QPointF const& cursorPosition);

    //! Clears the highlighting of the currently highlighted endpoint.
     void clearHighlightedEndpoint();

    /*!
     *  Creates a missing port to the given component item.
     *
     *      @param [in] portName   The name of the port to create.
     *      @param [in] component  The parent component.
     *      @param [in] design     The design containing related information.
     */
    BusPortItem* createMissingPort(QString const& portName, HWComponentItem* component, QSharedPointer<Design> design);

    /*!
     *  Initializes the context menu actions.
     */
	void setupActions();

    /*!
     *  Copies component instances in a format which can be saved to clipboard.
     *
     *      @param [in]  items       The top-level bus interface instance items to copy.
     *      @param [out] collection  The resulted collection of bus interface instance copy data.
     */
    void copyInterfaces(QList<QGraphicsItem*> const& items, BusInterfaceCollectionCopyData &collection);

    /*!
     *  Copies component instances in a format which can be saved to clipboard.
     *
     *      @param [in]  items       The component instance items to copy.
     *      @param [out] collection  The resulted collection of component instance copy data.
     */
    void copyInstances(QList<QGraphicsItem*> const& items, ComponentCollectionCopyData &collection);
 
    /*!
     *  Pastes bus port instances from a copy data collection.
     *
     *      @param [in] collection     The collection of bus port instance copy data.
     *      @param [in] component      The component where to place the ports.
     *      @param [in] cmd            The parent undo command for the paste undo commands.
     */
   void pasteInterfaces(BusInterfaceCollectionCopyData const& collection,
                        HWComponentItem* component, QUndoCommand* cmd);

   /*!
     *  Pastes top-level bus interface instances from a copy data collection.
     *
     *      @param [in] collection     The collection of bus interface instance copy data.
     *      @param [in] column         The column where to place the instances.
     *      @param [in] cmd            The parent undo command for the paste undo commands.
     *      @param [in] userCursorPos  If true, the instances are placed close to the cursor position.
     *                                 Otherwise the original positions are used.
     */
    void pasteInterfaces(BusInterfaceCollectionCopyData const& collection,
                         GraphicsColumn* column, QUndoCommand* cmd, bool useCursorPos);

    /*!
     *  Pastes component instances from a copy data collection.
     *
     *      @param [in] collection     The collection of component instance copy data.
     *      @param [in] column         The column where to place the instances.
     *      @param [in] cmd            The parent undo command for the paste undo commands.
     *      @param [in] userCursorPos  If true, the instances are placed close to the cursor position.
     *                                 Otherwise the original positions are used.
     */
    void pasteInstances(ComponentCollectionCopyData const& collection,
                        GraphicsColumn* column, QUndoCommand* cmd, bool useCursorPos);

    /*!
     *  Enables/disable context menu actions based on the current selection.
     */
    void prepareContextMenuActions();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! \brief Pointer to the parent of this scene.
	HWDesignWidget* parent_;

    //! The connection that is being drawn.
    HWConnection *tempConnection_;

    //! The starting end point of a connection that is being drawn.
    ConnectionEndpoint* tempConnEndPoint_;

    //! The potential end points that can be connected to the starting end point.
    QVector<ConnectionEndpoint*> tempPotentialEndingEndPoints_;

    //! The highlighted end point to which the connection could be snapped automatically.
    HWConnectionEndpoint* highlightedEndPoint_;

    //! The type of the item being dragged.
    ColumnItemType dragCompType_;

    //! Flag for indicating that the item being dragged is a bus.
    bool dragBus_;

    //! The component that is used to replace another component in replace mode.
    HWComponentItem* sourceComp_;

    //! Old selected items.
    QList<QGraphicsItem*> oldSelectedItems_;

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

    //! Context menu action for opening a component design.
    QAction openDesignAction_;

    //! Cursor position where the user right-presses to open the context menu.
    QPoint contextPos_;
};

#endif // HWDESIGNDIAGRAM_H
