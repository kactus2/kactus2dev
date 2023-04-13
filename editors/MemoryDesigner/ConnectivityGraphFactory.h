//-----------------------------------------------------------------------------
// File: ConnectivityGraphFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.07.2016
//
// Description:
// Creates a connectivity graph from a given design and design configuration.
//-----------------------------------------------------------------------------

#ifndef CONNECTIVITYGRAPHFACTORY_H
#define CONNECTIVITYGRAPHFACTORY_H

#include <IPXACTmodels/common/VLNV.h>

class ActiveInterface;
class AddressBlock;
class BusInterface;
class Channel;
class ConfigurableVLNVReference;
class Component;
class ComponentInstance;
class Design;
class DesignConfiguration;
class DesignInstantiation;

class Interconnection;
class AddressSpace;
class MemoryMap;
class MemoryMapBase;
class Register;
class Field;
class EnumeratedValue;
class FieldReset;
class View;

class LibraryInterface;

class ConnectivityConnection;
class ConnectivityGraph;
class ConnectivityComponent;
class ConnectivityInterface;
class ExpressionParser;
class MemoryItem;
class MultipleParameterFinder;

#include <QString>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Creates a connectivity graph from a given design and design configuration.
//-----------------------------------------------------------------------------
class ConnectivityGraphFactory
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] library     The library interface.
     */    
    ConnectivityGraphFactory(LibraryInterface* library);

    //! The destructor.
    ~ConnectivityGraphFactory();
   
    /*!
     *  Creates a connectivity graph from the component and its underlying hierarchy.
     *
     *      @param [in] topComponent    The selected component.
     *      @param [in] activeView      The view to determine hierarchy.
     *
     *      @return Connectivity graph for the design hierarchy.
     */
    QSharedPointer<ConnectivityGraph> createConnectivityGraph(QSharedPointer<const Component> topComponent,
        QString const& activeView);

private:
    // Disable copying.
    ConnectivityGraphFactory(ConnectivityGraphFactory const& rhs);
    ConnectivityGraphFactory& operator=(ConnectivityGraphFactory const& rhs);

    /*!
     *  Creates graph elements from the given design and configuration.
     *
     *      @param [in] designInstantiation     The design instantiation used in the top component.
     *      @param [in] design                  The design to analyze and transform into graph.
     *      @param [in] designConfiguration     The design configuration to analyze and transform into graph.
     *      @param [in] topInterfaces           The top-level component interfaces available for connections.
     *      @param [in/out] graph               The graph to add elements into.
     */
    void analyzeDesign(QSharedPointer<DesignInstantiation> designInstantiation,
        QSharedPointer<const Design> design,
        QSharedPointer<const DesignConfiguration> designConfiguration,
        QVector<QSharedPointer<ConnectivityInterface> > const& topInterfaces,
        QSharedPointer<ConnectivityGraph> graph) const;

    /*!
     *  Creates a component instance representation for a component instance.
     *
     *      @param [in] instance      The component instance to transform into the graph.
     *      @param [in] component     The component for the instance.
     *      @param [in] activeView    The active view for the instance.
     *      @param [in/out] graph     The graph to add instance node into.
     *
     *      @return The graph representation for the component instance.
     */
    QSharedPointer<ConnectivityComponent> createInstanceData(QSharedPointer<ComponentInstance> instance,
        QSharedPointer<const Component> component, QString const& activeView, 
        QSharedPointer<ConnectivityGraph> graph) const;
    
    /*!
     *  Adds all address spaces to a component instance in the graph.
     *
     *      @param [in/out] instance    The component instance to add the address spaces into.
     *      @param [in] component       The component for the instance.
     */
    void addAddressSpaceMemories(QSharedPointer<ConnectivityComponent> instance,
        QSharedPointer<const Component> component) const;

    /*!
     *  Adds all memory maps to a component instance in the graph.
     *
     *      @param [in] instanceData    The component instance representation to add memory maps into.
     *      @param [in] component       The component for the instance.     
     */
    void addMemoryMapMemories(QSharedPointer<ConnectivityComponent> instanceData,
        QSharedPointer<const Component> component) const;

    /*!
     *  Creates a representation for a memory map in a component instance.
     *
     *      @param [in] map                     The memory map to transform into the graph.
     *      @param [in] addressableUnitBits     The number of addressable unit bits in the memory map.
     *      @param [in] containingInstance      The component instance containing the memory map.
     *
     *      @return Representation for the memory map.
     */
    QSharedPointer<MemoryItem> createMemoryMapData(QSharedPointer<const MemoryMapBase> map,
        int addressableUnitBits, QSharedPointer<ConnectivityComponent> containingInstance) const;

    /*!
     *  Adds a all memory remap representations into a component instance representation.
     *
     *      @param [in] map                     The memory map containing the memory remaps.
     *      @param [in] mapItem                 The memory map representation for the memory map.
     *      @param [in] addressableUnitBits     The number of addressable unit bits in the memory map.
     *      @param [in] containingInstance      The component instance containing the memory map and remaps.
     */
    void addMemoryRemapData(QSharedPointer<const MemoryMap> map, QSharedPointer<MemoryItem> mapItem, 
        int addressableUnitBits, QSharedPointer<ConnectivityComponent> containingInstance) const;

    /*!
     *  Creates a representation for an address block.
     *
     *      @param [in] block                   The address block to transform into the graph.
     *      @param [in] mapIdentifier           Identifier for the containing memory map.
     *      @param [in] addressableUnitBits     The number of addressable unit bits in the memory map.
     *
     *      @return Representation for the address block.
     */
    QSharedPointer<MemoryItem> createMemoryBlock(QSharedPointer<const AddressBlock> block,
        QString const& mapIdentifier, int addressableUnitBits) const;

    /*!
     *  Adds a register representations into an address block representation.
     *
     *      @param [in] reg                     The register to add.
     *      @param [in] baseAddress             The base address for the address block.
     *      @param [in] addressableUnitBits     The number of addressable unit bits in the memory map.
     *      @param [in] blockIdentifier         Identifier for the containing address block.
     *      @param [in/out] blockItem           The address block to the add register into.
     */
    void addRegisterData(QSharedPointer<const Register> reg, int baseAddress, int addressableUnitBits,
        QString const& blockIdentifier, QSharedPointer<MemoryItem> blockItem) const;

    /*!
     *  Get field items of the selected register item in order of offset.
     *
     *      @param [in] registerItem    The selected register item.
     *      @param [in] fieldItems      Field items of the register item.
     *
     *      @return Field items in order of offset.
     */
    QMap<quint64, QSharedPointer<MemoryItem> > getOrderedFieldItems(QSharedPointer<MemoryItem> registerItem,
        QVector<QSharedPointer<MemoryItem> > fieldItems) const;

    /*!
     *  Creates a representation for a field within a register.
     *
     *      @param [in] field                   The field to transform into the graph.
     *      @param [in] registerIdentifier      Identifier for the containing register.
     *      @param [in] regAddress              The base address for the containing register.
     *      @param [in] addressableUnitBits     The number of addressable unit bits in the memory map.
     *
     *      @return Representation for the field.
     */
    QSharedPointer<MemoryItem> createField(QSharedPointer<const Field> field, QString const& registerIdentifier,
        int regAddress, int addressableUnitBits) const;

    /*!
     *  Creates a representation for an enumerated value within a field.
     *
     *      @param [in] enumeratedValue     The enumerated value to transform into the graph.
     *      @param [in] fieldIdentifier     Identifier for the containing field.
     *      @param [in] addressUnitBits     The number of address unit bits in the memory map.
     *
     *      @return Representation of the enumerated value.
     */
    QSharedPointer<MemoryItem> createEnumeratedValueItem(QSharedPointer<const EnumeratedValue> enumeratedValue,
        QString const& fieldIdentifier, int const& addressUnitBits) const;

    /*!
     *  Create a representation of a reset within a field.
     *
     *      @param [in] fieldReset          The selected field reset.
     *      @param [in] fieldIdentifier     Identifier of the containing field.
     *      @param [in] addressUnitBits     The number of address unit bits in the memory map.
     *      @param [in] fieldWidth          The bit width of the field.
     *
     *      @return Representation of the reset.
     */
    QSharedPointer<MemoryItem> createFieldResetItem(QSharedPointer<FieldReset> fieldReset,
        QString const& fieldIdentifier, int const& addressUnitBits, QString const& fieldWidth) const;

    /*!
     *  Create a representation of a reset within a register.
     *
     *      @param [in] fieldItems          Field items within the selected register.
     *      @param [in] registerItem        The selected register item.
     *      @param [in] registerIdentifier  Identifier of the containing register.
     *      @param [in] addressUnitBits     The number of address unit bits in the memory map.
     *
     *      @return Representation of the reset.
     */
    QSharedPointer<MemoryItem> createRegisterResetItem(QMap<quint64, QSharedPointer<MemoryItem> > fieldItems,
        QSharedPointer<MemoryItem> registerItem, QString const& registerIdentifier, int const& addressUnitBits)
        const;

    /*!
     *  Get the representation of HARD reset of the selected item.
     *
     *      @param [in] containingItem  The selected item.
     *
     *      @return Representation of a HARD reset.
     */
    QSharedPointer<MemoryItem> getHardResetItem(QSharedPointer<MemoryItem> containingItem) const;

    /*!
     *  Creates graph vertices from the bus interfaces of the given component.
     *
     *      @param [in] instancedComponent      The component whose bus interfaces to transform into graph.
     *      @param [in] instanceNode            The graph element representing the instance.
     *      @param [in/out] graph               The graph to add elements into.
     *
     *      @return The vertices created from the component bus interfaces.
     */
    QVector<QSharedPointer<ConnectivityInterface> > createInterfacesForInstance(
        QSharedPointer<Component const> instancedComponent, QSharedPointer<ConnectivityComponent> instanceNode,
        QSharedPointer<ConnectivityGraph> graph) const;
    
    /*!
     *  Creates an interface representation for a bus interface.
     *
     *      @param [in] busInterface    The bus interface to transform into the graph.
     *      @param [in] instanceNode    The instance containing the interface.
     *      @param [in/out] graph       The graph to add elements into.
     *
     *      @return The graph representation for the interface.
     */
    QSharedPointer<ConnectivityInterface> createInterfaceData(QSharedPointer<const BusInterface> busInterface,
        QSharedPointer<ConnectivityComponent> instanceNode, QSharedPointer<ConnectivityGraph> graph) const;

    /*!
     *  Creates an interfaces representation of a local memory map of an address space.
     *
     *      @param [in] addressSpace        The selected address space.
     *      @param [in] interfacedMemory    Memory item created for the address space.
     *      @param [in] instanceNode        Component instance containing the interface.
     *      @param [in/out] graph           The graph containing the interface.
     *
     *      @return The graph representation for the local memory map.
     */
    QSharedPointer<ConnectivityInterface> createLocalInterfaceData(QSharedPointer<AddressSpace> addressSpace,
        QSharedPointer<MemoryItem> interfacedMemory, QSharedPointer<ConnectivityComponent> instanceNode,
        QSharedPointer<ConnectivityGraph> graph) const;

    /*!
     *  Creates graph elements for component instance internal connections and a possible sub design.
     *
     *      @param [in] instancedComponent      The instanced component to transform.
     *      @param [in] instanceNode            The node containing the component instance.
     *      @param [in] instanceName            The name of the instance.
     *      @param [in] activeView              The active view of the component instance.
     *      @param [in] instanceInterfaces      The graph interfaces for the instanced component.
     *      @param [in/out] graph               The graph to add elements into.
     */
    void createInteralConnectionsAndDesigns(QSharedPointer<const Component> instancedComponent,
        QSharedPointer<ConnectivityComponent> instanceNode, QString const& instanceName, QString const& activeView,
        QVector<QSharedPointer<ConnectivityInterface> > instanceInterfaces,
        QSharedPointer<ConnectivityGraph> graph) const;

    /*!
     *  Create a connection between an address space and its local memory map.
     *
     *      @param [in] instancedComponent  Instanced component containing the address space.
     *      @param [in] instanceNode        The node containing the component instance.
     *      @param [in] instanceName        Name of the instance.
     *      @param [in] addressSpace        The selected address space.
     *      @param [in] instanceInterfaces  Interfaces within the instanced component.
     *      @param [in/out] graph               The graph to add elements into.
     */
    void createInternalSpaceMapConnection(QSharedPointer<const Component> instancedComponent,
        QSharedPointer<ConnectivityComponent> instanceNode, QString const& instanceName,
        QSharedPointer<AddressSpace> addressSpace,
        QVector<QSharedPointer<ConnectivityInterface> > instanceInterfaces,
        QSharedPointer<ConnectivityGraph> graph) const;

    /*!
     *  Get the bus interface that references the selected address space.
     *
     *      @param [in] instancedComponent  Instanced component containing the address space.
     *      @param [in] spaceName           Name of the selected address space.
     *
     *      @return The bus interface referencing the selected address space.
     */
    QSharedPointer<BusInterface> getBusInterfaceReferencingAddressSpace(
        QSharedPointer<const Component> instancedComponent, QString const& spaceName) const;

    /*!
     *  Get the memory item node containing the selected memory item.
     *
     *      @param [in] memoryReference     Name of the selected memory item.
     *      @param [in] instanceNode        The node containing the component instance.
     *
     *      @return The node containing the selected memory item.
     */
    QSharedPointer<MemoryItem> getMemoryItemNode(QString const& memoryReference,
        QSharedPointer<ConnectivityComponent> instanceNode) const;

    /*!
     *   Creates graph elements for component instance sub designs.
     *
     *      @param [in] instancedComponent      The instanced component whose sub design to transform.
     *      @param [in] activeView              The active view of the component instance.
     *      @param [in] instanceInterfaces      The graph interfaces for the instances component.
     *      @param [in/out] graph               The graph to add elements into.
     */
    void createConnectionsForDesign(QSharedPointer<const Component> instancedComponent, QString const& activeView,
        QVector<QSharedPointer<ConnectivityInterface> > instanceInterfaces, QSharedPointer<ConnectivityGraph> graph) const;

    /*!
     *  Get the design instantiation for the selected view from the selected component.
     *
     *      @param [in] instancedComponent  The selected component.
     *      @param [in] activeView          The selected view.
     *
     *      @return The design instantiation for the selected view from the selected component.
     */
    QSharedPointer<DesignInstantiation> getDesignInstantiation(
        QSharedPointer<const Component> instancedComponent, QSharedPointer<View> activeView) const;

    /*!
     *  Creates graph edges for the given design interconnection.
     *
     *      @param [in] interconnection     The interconnection to transform into edge(s).
     *      @param [in] designInterfaces    The interfaces in the component instances available for connections.
     *      @param [in] topInterfaces       The top-level interfaces in the design available for connections.
     *      @param [in/out] graph           The graph to add elements into.
     */
    void createConnectionsForInterconnection(QSharedPointer<const Interconnection> interconnection,
        QVector<QSharedPointer<ConnectivityInterface> > designInterfaces,
        QVector<QSharedPointer<ConnectivityInterface> > const& topInterfaces,
        QSharedPointer<ConnectivityGraph> graph) const;

    /*!
     *  Finds the interface vertex for the given interface name and instance.
     *
     *      @param [in] interfaceName           The name of the interface to find.
     *      @param [in] instanceName            The name of the component instance containing the interface.
     *      @param [in] instanceInterfaces      The interfaces available to search from.
     *
     *      @return The found interface vertex.
     */
    QSharedPointer<ConnectivityInterface> getInterface(QString const& interfaceName, QString const& instanceName,
        QVector<QSharedPointer<ConnectivityInterface> > const& instanceInterfaces) const;
    
    /*!
     *  Finds the top-level interface vertex for the given interface.
     *
     *      @param [in] interfaceName       The name of the interface to find.
     *      @param [in] topInterfaces       The interfaces available to search from.
     *
     *      @return The found interface vertex.
     */
    QSharedPointer<ConnectivityInterface> getTopInterface(QString const& interfaceName,
        QVector<QSharedPointer<ConnectivityInterface> > const& topInterfaces) const;

    /*!
     *  Finds the given view in a component.
     *
     *      @param [in] component       The component to search the view for.
     *      @param [in] viewName        The name of the view.
     *
     *      @return The view with the given name.
     */
    QSharedPointer<View> findView(QSharedPointer<const Component> component, QString const& viewName) const;

    /*!
     *  Creates the graph edges for the given channel.
     *
     *      @param [in] channel                 The channel to transform into edge(s).
     *      @param [in] instanceName            The name of the containing component instance.
     *      @param [in] instanceInterfaces      The interface vertices of the instance.
     *      @param [in/out] graph               The graph to add elements into.
     */
    void createInternalConnectionsForChannel(QSharedPointer<const Channel> channel, QString const& instanceName, 
        QVector<QSharedPointer<ConnectivityInterface> > instanceInterfaces, 
        QSharedPointer<ConnectivityGraph> graph) const;

    /*!
     *  Creates the graph edges for the given bridged bus interface.
     *
     *      @param [in] busInterface            The interface whose bridges to transform into edge(s).
     *      @param [in] instanceName            The name of the containing component instance.
     *      @param [in] instanceInterfaces      The interface vertices of the instance.
     *      @param [in/out] graph               The graph to add elements into.
     */
    void createInternalConnectionsForBridge(QSharedPointer<const BusInterface> busInterface, 
        QString const& instanceName, QVector<QSharedPointer<ConnectivityInterface> > const& instanceInterfaces, 
        QSharedPointer<ConnectivityGraph> graph) const;

    /*!
     *  Get the design configuration from the selected view.
     *
     *      @param [in] component           The component containing the selected view.
     *      @param [in] hierarchicalView    The selected view.
     *
     *      @return The design configuration referenced by the selected view.
     */
    QSharedPointer<const DesignConfiguration> getHierarchicalDesignConfiguration(
        QSharedPointer<const Component> component, QSharedPointer<const View> hierarchicalView) const;

    /*!
     *  Get the design from the selected view.
     *
     *      @param [in] component               Component containing the selected view.
     *      @param [in] hierarchicalView        The selected view.
     *      @param [in] designConfiguration     Design configuration referenced by the selected view.
     *
     *      @return The design referenced by the selected view.
     */
    QSharedPointer<const Design> getHierarchicalDesign(QSharedPointer<const Component> component,
        QSharedPointer<const View> hierarchicalView,
        QSharedPointer<const DesignConfiguration> designConfiguration) const;

    /*!
     *  Gets the VLNV of the design referenced by the selected view.
     *
     *      @param [in] component               Component containing the selected view.
     *      @param [in] hierarchicalView        The selected view.
     *
     *      @return The VLNV of the design referenced by the selected view.
     */
    VLNV getHierarchicalDesignVLNV(QSharedPointer<const Component> component,
        QSharedPointer<const View> hierarchicalView) const;

    /*!
     *  Creates an interconnection representation between bus interfaces.
     *
     *      @param [in] connectionName  Name of the interconnection.
     *      @param [in] startPoint      The starting interface of the connection.
     *      @param [in] endPoint        The ending interface of the connection.
     *      @param [in/out] graph       The graph to add elements into.
     *
     *      @return The graph representation for the interface.
     */
    void createConnectionData(QString const& connectionName, QSharedPointer<ConnectivityInterface> startPoint,
        QSharedPointer<ConnectivityInterface> endPoint, QSharedPointer<ConnectivityGraph> graph) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* library_;

    //! The parameter finder to use for instantiated components.
    QSharedPointer<MultipleParameterFinder> parameterFinder_;

    //! Parser for resolving expressions.
    ExpressionParser* expressionParser_;
};

//-----------------------------------------------------------------------------

#endif // CONNECTIVITYGRAPHFACTORY_H
