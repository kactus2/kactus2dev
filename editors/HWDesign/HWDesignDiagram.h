//-----------------------------------------------------------------------------
// File: HWDesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date: 
//
// Description:
// HWDesignDiagram is a graphical view to a design.
//-----------------------------------------------------------------------------

#ifndef HWDESIGNDIAGRAM_H
#define HWDESIGNDIAGRAM_H

#include "AdHocEnabled.h"

#include <editors/common/ComponentDesignDiagram.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/kactusExtensions/ColumnDesc.h>

#include <QMap>
#include <QVector>
#include <QSharedPointer>
#include <QUndoCommand>

class AbstractionDefinition;
class AdHocConnection;
class HierarchicalPortItem;
class ActivePortItem;
class ActiveBusInterfaceItem;
class HierarchicalBusInterfaceItem;
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
class MultipleParameterFinder;
class ListParameterFinder;
class ComponentItemAutoConnector;

//-----------------------------------------------------------------------------
//! HWDesignDiagram is a graphical view to a design.
//-----------------------------------------------------------------------------
class HWDesignDiagram : public ComponentDesignDiagram
{
    Q_OBJECT

public:

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a single bus interface instance.
    //-----------------------------------------------------------------------------
    struct BusInterfaceCopyData
    {
        QSharedPointer<Component> srcComponent;         //!< The origin component.
        QSharedPointer<BusInterface> busInterface;      //!< The bus interface.
        ComponentItem* containingItem;                  //!< The containing component item.
        QPointF position;
        bool topLevelIf;                                //!< Top-level or bus port interface.

        /*!
        *  Constructor.
        */
        BusInterfaceCopyData()
            : srcComponent(),
            busInterface(),
            containingItem(),
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
     *  The constructor.
     *
     *      @param [in] lh                                  Enables access to the library.
     *      @param [in] editProvider                        The edit provider.
     *      @param [in] designAndInstancesParameterFinder   Parameter finder for design parameter reference tree.
     *      @param [in] designParameterFinder               Parameter finder for design parameters.
     *      @param [in] parent                              Owner of this diagram.
     */
    HWDesignDiagram(LibraryInterface *lh, QSharedPointer<IEditProvider> editProvider,
        QSharedPointer<MultipleParameterFinder> designAndInstancesParameterFinder,
        QSharedPointer<ListParameterFinder> designParameterFinder,
        DesignWidget* parent = 0);

	/*!
     *  The destructor.
     */
	~HWDesignDiagram() final = default;

    /*!
     *  Set the IP-XACT document that is viewed in HWDesignDiagram.
     */
    void loadDesign(QSharedPointer<Design> design);

    /*!
     *  Get HWComponentItem that has the given instance name.
     */
    HWComponentItem* getComponentItem(QString const& instanceName);

	/*!
     *  Reflects the changes in the design to the top-level component.
     */
    virtual void updateHierComponent();

    /*!
     *  Load and insert the selected column to the graphics layout.
     *
     *      @param [in] description     Column description of the selected column.
     */
    void loadColumn(QSharedPointer<ColumnDesc> description);

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
    virtual void openDesignForComponent(ComponentItem* component, QString const& viewName) override;

    //! Handler for mouse double click events.
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

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
    void setInterfaceVLNVatEndpoint(VLNV const& droppedVLNV);

    /*!
     *  Replace the component item at the selected position.
     *
     *      @param [in] position    The position of the replaced component item.
     *      @param [in] component   The component to replace the item at position.
     */
    void replaceComponentItemAtPositionWith(QPointF position, QSharedPointer<Component> component);

    /*!
     *  Create a new component item.
     *
     *      @param [in] comp            The selected component.
     *      @param [in] instanceName    The name of the instance to create.
     *      @param [in] position        Position of the new component item.
     *      @param [in] parentCommand   Parent undo command for item creation.
     */
    HWComponentItem* createComponentItem(QSharedPointer<Component> comp, QString const& instanceName,
        QPointF position, QUndoCommand* parentCommand);

    //! Updates the dropAction and highlight according to underlying element.
    virtual void updateDropAction(QGraphicsSceneDragDropEvent* event) override;

    /*!
     *  Checks if open component action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool openComponentActionEnabled() const override;

    /*!
     *  Check if the selected items are of a type used in the HW design.
     *
     *      @return True, if the items are used in HW design, false otherwise.
     */
    virtual bool selectedItemIsCorrectType() const override;

    /*!
     *  Checks if the given item is a hierarchical component.
     *
     *      @param [in] item   The item to check.
     *
     *      @return True, if item is a hierarchical component, otherwise false.
     */
    virtual bool isHierarchicalComponent(QGraphicsItem* item) const override;

    /*!
     *  Checks if copy action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool copyActionEnabled() const override;

    /*!
     *  Checks if paste action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool pasteActionEnabled() const override;

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
    virtual QStringList hierarchicalViewsOf(ComponentItem* component) const override;

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
    virtual AdHocItem* createAdhocItem(QString const& portName) override;

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
     *      @param [in] connection          The connection to create a command for.     
     *      @param [in] parentUndoCommand   Optional undocommand to set as parent for created commands.
     *
     *      @return The created add command.
     */
    virtual QUndoCommand* createAddCommandForConnection(GraphicsConnection* connection, QUndoCommand* parentCommand = nullptr);

    /*!
     *  Copies the interface definition to hierarchical interface from a defined interface.
     *
     *      @param [in] definedPoint        The interface item to copy definitions from.
     *      @param [in] undefinedPoint      The interface item to copy definitions into.
     *      @param [in] definingConnection  The connection between the points initiating the copy.
     *      @param [in] parentCommand       The parent undo command.
     *
     *      @return True, if the copy was accepted by the user, otherwise false.
     */
    bool copyDefinitions(ConnectionEndpoint* definedPoint, ConnectionEndpoint* undefinedPoint,
        HWConnection* definingConnection, QUndoCommand* parentCommand);

    /*!
     *  Checks if the given bus interface item has a default name.
     *
     *      @param [in] connectionPoint   The bus interface item to check.
     *
     *      @return True, if the name is default pattern, otherwise false.
     */
    bool hasDefaultName(ConnectionEndpoint const* undefinedPoint) const;
    
    /*!
     *  Copies the interface name to hierarchical interface from a defined interface.
     *
     *      @param [in] sourceInterface     The interface item to copy name from.
     *      @param [in] targetInterface     The interface item to copy the name into.
     *      @param [in] definingConnection  The connection between the points initiating the copy.
     *      @param [in] parentCommand       The parent undo command.
     */
    void copyInterfaceName(ConnectionEndpoint* sourceInterface, ConnectionEndpoint* targetInterface,
        HWConnection* definingConnection, QUndoCommand* parentCommand);

    /*!
     *  Finds the hierInterfaces corresponding to the given interface name.
     *
     *      @param [in] previousName    The name to search for.
     *      @param [in] connection      The interconnection to search in.
     *
     *      @return The hierInterfaces with the given name.
     */
    QList<QSharedPointer<HierInterface> > findInterfacesByName(QString const& previousName,
        QSharedPointer<Interconnection> connection) const;
    
    /*!
     *  Copies the interface mode to hierarchical interface from a defined interface.
     *
     *      @param [in] sourceInterface     The interface item to copy mode from.
     *      @param [in] targetInterface     The interface item to copy mode into.
     *      @param [in] parentCommand       The parent undo command.
     */
    void copyMode(ConnectionEndpoint* sourceInterface, ConnectionEndpoint* targetInterface,
        QUndoCommand* parentCommand);

    /*!
     *  Copies the interface type to hierarchical interface from a defined interface.
     *
     *      @param [in] definedPoint        The interface item to copy type from.
     *      @param [in] draftPoint          The interface item to copy type into.
     *      @param [in] parentCommand       The parent undo command.
     */
    void copyType(ConnectionEndpoint* definedPoint, ConnectionEndpoint* draftPoint, QUndoCommand* parentCommand);

    /*!
     *  Copies the port maps and ports into a hierarchical bus interface.
     *
     *      @param [in] sourcePoint         The interface item to copy from.
     *      @param [in] targetPoint         The interface item to copy into.
     *      @param [in] parentCommand       The parent undo command.
     *
     *      @return True, if the copy was accepted by the user, otherwise false.
     */
    bool copyPortsAndMapsForHierarchicalPoint(ConnectionEndpoint* sourcePoint, ConnectionEndpoint* targetPoint,
        QUndoCommand* parentCommand);

    /*!
     *  Copies port and port maps from the given bus interface into a bus interface item.
     *
     *      @param [in] sourceComponent     The component containing the bus interface to copy from.
     *      @param [in] sourceInterface     The bus interface to copy from.
     *      @param [in] sourceInstanceItem  The item containing the copied end point.
     *      @param [in] target              The bus interface item to copy into.
     *      @param [in] parentCommand       The parent undo command.
     */
    void copyPortMapsAndPhysicalPorts(QSharedPointer<Component> sourceComponent, 
        QSharedPointer<BusInterface> sourceInterface, ComponentItem* sourceInstanceItem,
        ConnectionEndpoint* target, QUndoCommand* parentCommand);

    /*!
     *  Creates a connecting port for opposing the given port.
     *
     *      @param [in] sourcePort      The port whose opposing port to create.
     *      @param [in] portDirection   The port direction for the created port.
     *      @param [in] reservedNames   The names of existing ports in the target component.
     *
     *      @return The newly created port matching the source port in size and type.
     */
    QSharedPointer<Port> createConnectingPhysicalPort(QSharedPointer<Port> sourcePort, 
        DirectionTypes::Direction portDirection, QStringList const& reservedNames);

    /*!
     *  Creates the port maps for hierarchical interface by user selection.
     *
     *      @param [in] sourcePoint     The bus interface whose mode and definitions to use as a starting point.
     *      @param [in] targetPoint     The interface whose port maps to create.
     *      @param [in] parentCommand   The parent undo command.
     *
     *      @return True, if the selection was accepted by the user, otherwise false.
     */
    bool createPortMapsManually(ConnectionEndpoint* sourcePoint, ConnectionEndpoint* targetPoint,
        QUndoCommand* parentCommand);

    /*!
     *  Adds a new interface to the given diagram column.
     *
     *      @param [in] column The column where to add the interface.
     *      @param [in] pos    The interface position.
     */
    virtual void addTopLevelInterface(GraphicsColumn* column, QPointF const& pos);

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
    ActiveBusInterfaceItem* createMissingBusInterface(QString const& interfaceName, HWComponentItem* containingComponent,
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
    HierarchicalPortItem* createMissingHierarchicalAdHocPort(QString const& portName,
        QSharedPointer<Kactus2Placeholder> adHocExtension, QGraphicsItem* parentItem = (QGraphicsItem*)0);

    /*!
     *  Creates a graphics item for an ad-hoc interconnection and adds it to the diagram.
     *
     *      @param [in] adHocConn   The ad-hoc connection to create the item for.
     */
    void createAdHocConnection(QSharedPointer<AdHocConnection> adHocConnection);

    /*!
     *  Finds the graphical item for the given port reference.
     *
     *      @param [in] portReference   The port reference whose graphical item to find.
     *
     *      @return The port item or 0 if port cannot be found.
     */
    ActivePortItem* findAdhocPort(QSharedPointer<PortReference> portReference);

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
     *      @param [in] secondaryPort       The connected internal/hierarchical port.
     *      @param [in] primaryPortItem     The port item for the primary port.
     */
    void createConnectionForAdHocPorts(QSharedPointer<AdHocConnection> adHocConnection,
        QSharedPointer<PortReference> secondaryPort, 
        ConnectionEndpoint* primaryPortItem);
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

    /*!
     *  Generates a unique name.
     *
     *      @param [in] name            The base for the name.
     *      @param [in] reservedNames   The existing names that should not be generated.
     *
     *      @return A unique name.
     */
    QString generateUniqueName(QString const& name, QStringList const& reservedNames) const;

    /*!
     *  Finds the name of hierarchical interfaces in the design.
     *
     *      @return The names of the hierarchical interfaces.
     */
    QStringList getTopLevelInterfaceNames() const;

    /*!
     *  Check if the design parameter reference finder contains a component finder for the selected component.
     *
     *      @param [in] component   The selected component.
     *
     *      @return True, if the design parameter reference finder contains the selected component, otherwise
     *              false.
     */
    bool referenceFinderContainsComponent(QSharedPointer<Component> component);

    /*!
     *  Create auto connection dialog.
     *
     *      @param [in] firstItem   The first item for the auto connector.
     *
     *      @return The auto connector dialog.
     */
    virtual ComponentItemAutoConnector* createAutoConnector(ComponentItem* firstItem) const;

    /*!
     *  Get the connection end point for the selected auto connector item.
     *
     *      @param [in] connectorItem       The selected auto connector item.
     *      @param [in] parentUndoCommand   Optional undocommand to set as parent for created child commands.
     *
     *      @return The end point item for the selected auto connector item.
     */
    virtual ConnectionEndpoint* getEndPointForItem(AutoConnectorItem* connectorItem, QUndoCommand* parentUndoCommand = nullptr);

    /*!
     *  Get the component item connection end point for the selected auto connector item.
     *
     *      @param [in] connectorItem       The selected auto connector item.
     *      @param [in] parentUndoCommand   Optional undocommand to set as parent for created child commands.
     *
     *      @return The selected component item connection end point.
     */
    ConnectionEndpoint* getEndPointFromComponentItem(AutoConnectorItem* connectorItem, QUndoCommand* parentUndoCommand = nullptr);

    /*!
     *  Get the top component connection end point for the selected auto connector item.
     *
     *      @param [in] connectorItem       The selected auto connector item.
     *      @param [in] parentUndoCommand   Optional undocommand to set as parent for created child commands.
     * 
     *      @return The selected top component connection end point.
     */
    ConnectionEndpoint* getEndPointForTopComponentItem(AutoConnectorItem* connectorItem, QUndoCommand* parentUndoCommand = nullptr);

    /*!
     *  Create connection between the selected end points.
     *
     *      @param [in] startPoint  Start point for the connection.
     *      @param [in] endPoint    End point for the connection.
     */
    virtual void createConnectionBetweenEndPoints(ConnectionEndpoint* startPoint, ConnectionEndpoint* endPoint);

    /*!
     *  Check if the auto connector should be added to the context menu.
     *
     *      @return True, if the auto connected should be added, false otherwise.
     */
    virtual bool addAutoConnectorActionToContextMenu() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The type of the item being dragged.
    ColumnTypes::ColumnItemType dragCompType_ = ColumnTypes::NONE;

    //! Flag for indicating that the item being dragged is a bus.
    bool dragBus_ = false;

    //! The possible end point under cursor while performing drag.
    HWConnectionEndpoint* dragEndPoint_ = nullptr;

    //! Handler for design diagram calculation.
    QSharedPointer<DesignDiagramResolver> diagramResolver_;

    //! Parameter finder for design parameter reference tree
    QSharedPointer<MultipleParameterFinder> designAndInstancesParameterFinder_;
};

#endif // HWDESIGNDIAGRAM_H
