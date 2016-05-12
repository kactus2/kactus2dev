//-----------------------------------------------------------------------------
// File: HWDesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// HWDesignDiagram is a graphical view to a design.
//-----------------------------------------------------------------------------

#ifndef HWDESIGNDIAGRAM_H
#define HWDESIGNDIAGRAM_H

#include "AdHocEnabled.h"

#include <designEditors/common/ComponentDesignDiagram.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/kactusExtensions/ColumnDesc.h>

#include <QMap>
#include <QVector>
#include <QSharedPointer>
#include <QUndoCommand>

class AbstractionDefinition;
class AdHocConnection;
class AdHocInterfaceItem;
class AdHocPortItem;
class BusPortItem;
class BusInterfaceItem;
class Component;
class ComponentInstance;
class ComponentItem;
class ConnectionEndpoint;
class ConnectionRoute;
class Design;
class DesignConfiguration;
class GenericEditProvider;
class GraphicsColumn;
class GraphicsColumnLayout;
class GraphicsConnection;
class HierConnection;
class HierInterface;
class HWComponentItem;
class HWConnection;
class HWConnectionEndpoint;
class Interconnection;
class LibraryInterface;
class PortReference;
class VLNV;
class InterfaceGraphicsData;
class Kactus2Placeholder;
class DesignDiagramResolver;
class AdHocItem;

//-----------------------------------------------------------------------------
//! HWDesignDiagram is a graphical view to a design.
//-----------------------------------------------------------------------------
class HWDesignDiagram : public ComponentDesignDiagram
{
    Q_OBJECT

public:
    // Column widths.
    enum
    {
		COMPONENT_COLUMN_WIDTH = 319,
        IO_COLUMN_WIDTH = 119
    };		

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a single bus interface instance.
    //-----------------------------------------------------------------------------
    struct BusInterfaceCopyData
    {
        QSharedPointer<Component> srcComponent;         //!< The origin component.
        QSharedPointer<BusInterface> busInterface;      //!< The bus interface.
        QPointF position;
        bool topLevelIf;                                //!< Top-level or bus port interface.

        /*!
        *  Constructor.
        */
        BusInterfaceCopyData()
            : srcComponent(),
            busInterface(),
            position(),
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
        QSharedPointer<ComponentInstance> instance;
       
        ComponentInstanceCopyData()
            : component(),
            instance()
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
        QSharedPointer<ColumnDesc> desc;            //!< Column description.
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
    HWDesignDiagram(LibraryInterface *lh, QSharedPointer<IEditProvider> editProvider, DesignWidget* parent = 0);

	//! The destructor.
	virtual ~HWDesignDiagram();

    /*!
     *  Set the IP-XACT document that is viewed in HWDesignDiagram.
     */
    void loadDesign(QSharedPointer<Design> design);

    /*!
     *  Get HWComponentItem that has the given instance name.
     */
    HWComponentItem* getComponentItem(QString const& instanceName);

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
    void addColumn(QSharedPointer<ColumnDesc> desc);

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
     *  Called when copy is selected from the context menu.
     */
	virtual void onCopyAction();

    /*!
     *  Called when paste is selected from the context menu.
     */
	virtual void onPasteAction();
    
    /*!
     *  Paste hierarchical interfaces.
     *
     *      @param [in] mimeData    The data containing the pasted items.
     */
    void pasteHierarchicalInterfaces(QMimeData const* mimeData);

    /*!
     *  Paste columns.
     */
    void pasteColumns();

    /*!
     *  Paste components to a new column.
     */
    void pasteComponentsToColumn();

    /*!
     *  Paste interfaces to a draft component.
     *
     *      @param [in] targetItem  The component item to paste interfaces into.
     */
    void pasteInterfacesToDraftComponent(HWComponentItem* targetItem);

    /*!
     *  Called when add to library is selected from the context menu.
     */
	virtual void onAddToLibraryAction();

protected:

    /*!
     *  Opens a design for a given component.
     *
     *      @param [in] component   The component whose design to open.
     *      @param [in] viewName    The name of the view to open.
     */
    void openDesignForComponent(ComponentItem* component, QString const& viewName);

    //! Handler for mouse double click events.
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    /*!
     *  Adds a new bus to library based on the given endpoint.
     *
     *      @param [in] endpoint   The endpoint to create the bus from.
     */
    void addBusToLibrary(HWConnectionEndpoint* endpoint);

    //! Handler for drag enter event. 
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);

    //! Handler for drag leave event.
    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);

    //! Handler for drop event.
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    
    /*!
     *  Set the interface VLNV at an end point.
     *
     *      @param [in] droppedVLNV     The selected VLNV.
     */
    void setInterfaceVLNVatEndpoint(VLNV &droppedVLNV);

    /*!
     *  Replace the component item at the selected position.
     *
     *      @param [in] position    The position of the replaced component item.
     *      @param [in] comp        The component item to replace the item at position.
     */
    void replaceComponentItemAtPositionWith(QPointF position, QSharedPointer<Component> comp);

    /*!
     *  Create a new component item.
     *
     *      @param [in] comp        The selected component.
     *      @param [in] position    Position of the new component item.
     */
    void createComponentItem(QSharedPointer<Component> comp, QPointF position);

    //! Updates the dropAction and highlight according to underlying element.
    virtual void updateDropAction(QGraphicsSceneDragDropEvent* event);

    /*!
     *  Checks if open component action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool openComponentActionEnabled() const;

    /*!
     *  Checks if the given item is a hierarchical component.
     *
     *      @param [in] item   The item to check.
     *
     *      @return True, if item is a hierarchical component, otherwise false.
     */
    virtual bool isHierarchicalComponent(QGraphicsItem* item) const;

    /*!
     *  Checks if copy action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool copyActionEnabled() const;

    /*!
     *  Checks if paste action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool pasteActionEnabled() const;

    /*!
     *  Opens the given component according to the active view of the component.
     *
     *      @param [in] comp   The component to open.
     */
    void openComponentByActiveView(ComponentItem * comp);

   /*!
    *  Gets the names of hierarchical views of a component.
    *
    *      @param [in] component   The component whose hierarchical views to get.
    *
    *      @return The names of the hierarchical views.
    */
    virtual QStringList hierarchicalViewsOf(ComponentItem* component) const;

    /*!
     *  Gets the graphics item type of the components in the diagram.
     *     
     *      @return The type of the components.
     */
    virtual int componentType() const;

    /*!
     *  Gets the graphics item type of the off page connectors in the diagram.
     *     
     *      @return The type of the components.
     */
    virtual int offpageConnectorType() const;

    /*!
     *  Gets the graphics item type of the connections in the diagram.
     *     
     *      @return The type of the components.
     */
    virtual int connectionType() const;

    /*!
     *  Create an ad hoc interface item with the selected name.
     *
     *      @param [in] portName    The name of the new ad hoc interface item.
     *
     *      @return The created ad hoc interface item.
     */
    virtual AdHocItem* createAdhocItem(QString const& portName);

private:
    // Disable copying.
    HWDesignDiagram(HWDesignDiagram const& rhs);
    HWDesignDiagram& operator=(HWDesignDiagram const& rhs);

    /*!
     *  Finds the design extension for a hierarchical bus interface item. If the extension does not exist,
     *  it is created.
     *
     *      @param [in] design              The design to search extensions in.
     *      @param [in] busInterfaceName    The name of the bus interface to search for.
     *
     *      @return The extension for hierarchical bus interface data.
     */
    QSharedPointer<InterfaceGraphicsData> findOrCreateInterfaceExtensionGroup(QSharedPointer<Design> design,
        QString const& busInterfaceName);

    /*!
     *  Called when an item has been selected in the diagram.
     *
     *      @param [in] newSelection The selected item.
     */
    virtual void onSelected(QGraphicsItem* newSelection);

    /*!
     *  Creates a connection between the given two endpoints.
     *
     *      @param [in] startPoint  The starting connection end point.
     *      @param [in] endPoint    The ending connection end point.
     *
     *      @return The created connection.
     */
    virtual GraphicsConnection* createConnection(ConnectionEndpoint* startPoint, ConnectionEndpoint* endPoint);

    /*!
     *  Creates a connection between the given endpoint and a coordinate point.
     *
     *      @param [in] startPoint  The starting connection end point.
     *      @param [in] endPoint    The ending coordinate point.
     *
     *      @return The created connection.
     */
    virtual GraphicsConnection* createConnection(ConnectionEndpoint* startPoint, QPointF const& endPoint);

    /*!
     *  Creates an add command for a given connection.
     *
     *      @param [in] connection  The connection to create a command for.     
     *
     *      @return The created add command.
     */
    virtual QSharedPointer<QUndoCommand> createAddCommandForConnection(GraphicsConnection* connection);
   
    /*!
     *  Adds a new interface to the given diagram column.
     *
     *      @param [in] column The column where to add the interface.
     *      @param [in] pos    The interface position.
     */
    virtual void addTopLevelInterface(GraphicsColumn* column, QPointF const& pos);

    /*!
     *  Create a new name for a hierarchical draft interface.
     *
     *      @param [in] defaultName     The default name of the interface.
     *
     *      @return The new name for the hierarchical draft interface.
     */
    QString createNameForHierarchicalDraftInterface(QString const& defaultName) const;

    /*!
     *  Handler for draft tool clicks. Creates a draft component instance or a draft interface according to the
     *  clicked position.
     *
     *      @param [in] clickedPosition   The position to create the draft item to.     
     */
    virtual void draftAt(QPointF const& clickedPosition);

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
    void addDraftComponentInstance(GraphicsColumn* column, QPointF const& position);

    /*!
     *  Adds a draft interface in a draft component instance.
     *
     *      @param [in] targetComponent     The component item to add the interface to.
     *      @param [in] position            The initial position of the interface.     
     */
    void addDraftComponentInterface(HWComponentItem* targetComponent, QPointF const& position);

    /*!
     *  Performs the replacing of destination component with source component.
     *
     *      @param [in] destComp        The component to replace.
     *      @param [in] sourceComp      The replacing component.     
     */
    virtual void replace(ComponentItem* destComp, ComponentItem* sourceComp);

    /*!
     *  Creates a graphics item for component instance and adds it to the diagram.
     *
     *      @param [in] instance    The instance to create item for.
     *      @param [in] design      The design containing the instance.
     */
    void createComponentItem(QSharedPointer<ComponentInstance> instance, QSharedPointer<Design> design);

    /*!
     *  Finds a port item on a component item or creates one if not found.
     *
     *      @param [in] componentItem   The component graphic item containing the port.
     *      @param [in] componentRef    The reference name for the component.
     *      @param [in] busRef          The reference name for the bus interface.
     *      @param [in] design          The design containing the component instance.
     *
     *      @return The port item on the component.
     */
    ConnectionEndpoint* findOrCreateMissingInterface(HWComponentItem* componentItem, QString const& componentRef, 
        QString const& busRef, QSharedPointer<Design> design);
    
    /*!
     *  Creates a missing port to the given component item.
     *
     *      @param [in] portName   The name of the port to create.
     *      @param [in] component  The parent component.
     *      @param [in] design     The design containing related information.
     *
     *      @return A port item for a missing port.
     */
    BusPortItem* createMissingBusInterface(QString const& interfaceName, HWComponentItem* containingComponent, 
        QSharedPointer<Design> design);

    /*!
     *  Creates a graphics item for interconnection and adds it to the diagram.
     *
     *      @param [in] interconnection     The interconnection to create item for.
     *      @param [in] design              The design containing the interconnection.
     */
    void createInterconnection(QSharedPointer<Interconnection> interconnection, QSharedPointer<Design> design);

    /*!
     *  Create an interconnection between components.
     *
     *      @param [in] interconnection     The selected interconnection.
     *      @param [in] design              The design in which to create the interconnection.
     */
    void createInterconnectionBetweenComponents(QSharedPointer<Interconnection> interconnection, QSharedPointer<Design> design);

    /*!
     *  Find or create a route for the interconnection.
     *
     *      @param [in] interconnectionName     The name of the interconnection.
     */
    QSharedPointer<ConnectionRoute> findOrCreateRouteForInterconnection(QString const& interconnectionName);

    /*!
     *  Creates a graphics item for hierarchical interconnection and adds it to the diagram.
     *
     *      @param [in] hierConn    The hierarchical interconnection to create item for.
     *      @param [in] design      The design containing the interconnection.
     */
    void createHierarchicalConnection(QSharedPointer<Interconnection> connection,
          QSharedPointer<Design> design);
    
    /*!
     *  Find or create a hierarchical interface.
     *
     *      @param [in] busRef  The name of the interface.
     */
    ConnectionEndpoint* findOrCreateHierarchicalInterface(QString const& busRef);

    /*!
     *  Creates the hierarchical ad-hoc port items in the diagram.
     *
     *      @param [in] design   The design containing the ad-hoc ports.
     */
    void createHierachicalAdHocPorts(QSharedPointer<Design> design);
    
    /*!
     *  Create a missing hierarchical ad hoc port.
     *
     *      @param [in] portName        The name of the missing port.
     *      @param [in] adHocExtension  The extension containing information of the port.
     *      @param [in] parentItem      The parent item of the missing ad hoc port.
     *
     *      @return The interface item for the missing interface.
     */
    AdHocInterfaceItem* createMissingHierarchicalAdHocPort(QString const& portName,
        QSharedPointer<Kactus2Placeholder> adHocExtension, QGraphicsItem* parentItem = (QGraphicsItem*)0);

    /*!
     *  Creates a graphics item for an ad-hoc interconnection and adds it to the diagram.
     *
     *      @param [in] adHocConn   The ad-hoc connection to create the item for.
     */
    void createAdHocConnection(QSharedPointer<AdHocConnection> adHocConnection);

    /*!
     *  Create a symbol for an ad hoc tie off value.
     *
     *      @param [in] connection  The selected ad hoc connection.
     */
    void createAdHocTieOffConnection(QSharedPointer<AdHocConnection> connection);

    /*!
     *  Creates a graphics item for an ad-hoc interconnection between two ports and adds it to the diagram.
     *
     *      @param [in] adHocConnection     The ad-hoc connection containing the ports.
     *      @param [in] internalPort        The connected internal port on a component instance.
     *      @param [in] primaryPort         The connected hierarchical port or component instance port.
     *      @param [in] primaryPortItem     The port item for the primary port.
     */
    void createConnectionForAdHocPorts(QSharedPointer<AdHocConnection> adHocConnection, QSharedPointer<PortReference> internalPort, 
        QSharedPointer<PortReference> primaryPort, ConnectionEndpoint* primaryPortItem);
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
   void pasteInterfaces(BusInterfaceCollectionCopyData const& collection, HWComponentItem* component, 
       QUndoCommand* cmd);

   /*!
     *  Pastes top-level bus interface instances from a copy data collection.
     *
     *      @param [in] collection     The collection of bus interface instance copy data.
     *      @param [in] column         The column where to place the instances.
     *      @param [in] cmd            The parent undo command for the paste undo commands.
     *      @param [in] userCursorPos  If true, the instances are placed close to the cursor position.
     *                                 Otherwise the original positions are used.
     */
    void pasteTopLevelInterfaces(BusInterfaceCollectionCopyData const& collection, GraphicsColumn* column,
        QUndoCommand* cmd, bool useCursorPos);

    /*!
     *  Creates paste command for component instances from a copy data collection.
     *
     *      @param [in] collection     The collection of component instance copy data.
     *      @param [in] column         The column where to place the instances.
     *      @param [in] cmd            The parent undo command for the paste undo commands.
     *      @param [in] userCursorPos  If true, the instances are placed close to the cursor position.
     *                                 Otherwise the original positions are used.
     */
    void createComponentPasteCommand(ComponentCollectionCopyData const& collection, GraphicsColumn* column, 
        QUndoCommand* parentCommand, bool useCursorPos);

    /*!
     *  Show the selected hoc port item.
     *
     *      @param [in] portItem    The selected ad hoc port item.
     */
    virtual void showAdhocPort(AdHocItem* portItem);

    /*!
     *  Hide the selected hoc port item.
     *
     *      @param [in] portItem    The selected ad hoc port item.
     */
    virtual void hideAdhocPort(AdHocItem* portItem);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The type of the item being dragged.
    ColumnTypes::ColumnItemType dragCompType_;

    //! Flag for indicating that the item being dragged is a bus.
    bool dragBus_;

    //! The possible end point under cursor while performing drag.
    HWConnectionEndpoint* dragEndPoint_;

    //! Handler for design diagram calculation.
    QSharedPointer<DesignDiagramResolver> diagramResolver_;
};

#endif // HWDESIGNDIAGRAM_H
