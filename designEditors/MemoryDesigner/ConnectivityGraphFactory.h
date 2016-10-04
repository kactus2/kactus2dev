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

#ifndef COMPONENTINSTANCELOCATOR_H
#define COMPONENTINSTANCELOCATOR_H

#include <IPXACTmodels/common/VLNV.h>

class ActiveInterface;
class AddressBlock;
class BusInterface;
class LibraryInterface;
class ConfigurableVLNVReference;
class ComponentInstance;
class Component;
class Design;
class DesignConfiguration;
class Field;
class MemoryMap;
class Register;
class View;

class ConnectivityConnection;
class ConnectivityGraph;
class ConnectivityComponent;
class ConnectivityInterface;
class ExpressionParser;
class MemoryItem;
class ComponentParameterFinder;

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
     *  Creates a connectivity graph from the given design and its underlying hierarchy.
     *
     *      @param [in] design                  The selected design.
     *      @param [in] designConfiguration     The selected design configuration.
     *
     *      @return Connectivity graph for the design hierarchy.
     */
    QSharedPointer<ConnectivityGraph> createConnectivityGraph(QSharedPointer<const Design> design, 
        QSharedPointer<const DesignConfiguration> designConfiguration);

private:
    // Disable copying.
    ConnectivityGraphFactory(ConnectivityGraphFactory const& rhs);
    ConnectivityGraphFactory& operator=(ConnectivityGraphFactory const& rhs);

    /*!
     *  Creates graph elements from the given design and configuration.
     *
     *      @param [in] design                  The design to analyze and transform into graph.
     *      @param [in] designConfiguration     The design configuration to analyze and transform into graph.
     *      @param [in] topInstance             The name of the top component instance.
     *      @param [in/out] graph               The graph to add elements into.
     */
    void analyzeDesign(QSharedPointer<const Design> design, 
        QSharedPointer<const DesignConfiguration> designConfiguration, QString const& topInstance, 
        QSharedPointer<ConnectivityGraph> graph);
       
    /*!
     *  Creates instances and interfaces from the given design and adds them into the graph.
     *
     *      @param [in] design                  The design to analyze and transform into graph.
     *      @param [in] designConfiguration     The design configuration to analyze and transform into graph.
     *      @param [in/out] graph               The graph to add elements into.
     */
    void addInstancesAndInterfaces(QSharedPointer<const Design> design,
        QSharedPointer<const DesignConfiguration> designConfiguration, QSharedPointer<ConnectivityGraph> graph);
         
    /*!
     *  Creates an interface representation for a bus interface.
     *
     *      @param [in] busInterface   The bus interface to transform into the graph.
     *      @param [in] instanceNode   The instance containing the interface.
     *
     *      @return The graph representation for the interface.
     */
    QSharedPointer<ConnectivityInterface> createInterfaceData(QSharedPointer<BusInterface> busInterface,
        QSharedPointer<ConnectivityComponent> instanceNode);

    /*!
     *  Creates a component instance representation for a component instance.
     *
     *      @param [in] instance      The component instance to transform into the graph.
     *      @param [in] component     The component for the instance.
     *      @param [in] activeView    The active view for the instance.
     *
     *      @return The graph representation for the component instance.
     */
    QSharedPointer<ConnectivityComponent> createInstanceData(QSharedPointer<ComponentInstance> instance,
        QSharedPointer<const Component> component, QString const& activeView) const;
    
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
    QSharedPointer<MemoryItem> createMemoryMapData(QSharedPointer<MemoryMap> map, int addressableUnitBits, 
        QSharedPointer<ConnectivityComponent> containingInstance) const;

    /*!
     *  Adds a all memory remap representations into a component instance representation.
     *
     *      @param [in] map                     The memory map containing the memory remaps.
     *      @param [in] mapItem                 The memory map representation for the memory map.
     *      @param [in] addressableUnitBits     The number of addressable unit bits in the memory map.
     *      @param [in] containingInstance      The component instance containing the memory map and remaps.
     */
    void addMemoryRemapData(QSharedPointer<MemoryMap> map, QSharedPointer<MemoryItem> mapItem, 
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
    QSharedPointer<MemoryItem> createMemoryBlock(QSharedPointer<AddressBlock> block, QString const& mapIdentifier,
        int addressableUnitBits) const;

    /*!
     *  Adds a register representations into an address block representation.
     *
     *      @param [in] reg                     The register to add.
     *      @param [in] baseAddress             The base address for the address block.
     *      @param [in] addressableUnitBits     The number of addressable unit bits in the memory map.
     *      @param [in] blockIdentifier         Identifier for the containing address block.
     *      @param [in/out] blockItem           The address block to the add register into.
     */
    void addRegisterData(QSharedPointer<Register> reg, int baseAddress, int addressableUnitBits,
        QString const& blockIdentifier, QSharedPointer<MemoryItem> blockItem) const;

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
    QSharedPointer<MemoryItem> createField(QSharedPointer<Field> field, QString const& registerIdentifier,
        int regAddress, int addressableUnitBits) const;

    /*!
     *  Get the child instances of a hierarchical component instance.
     *
     *      @param [in] instance                The selected component instance.
     *      @param [in] instancedComponent      The instanced component.
     *      @param [in] designConfiguration     Design configuration of the design containing the component
     *                                          instance.
     */
    void addSubInstances(QSharedPointer<ConnectivityComponent> topInstance,
        QSharedPointer<const Component> topComponent,
        QSharedPointer<const DesignConfiguration> designConfiguration,
        QSharedPointer<ConnectivityGraph> graph);

    /*!
     *  Get the design configuration from the selected view.
     *
     *      @param [in] component           The component containing the selected view.
     *      @param [in] hierarchicalView    The selected view.
     *
     *      @return The design configuration referenced by the selected view.
     */
    QSharedPointer<const DesignConfiguration> getHierarchicalDesignConfiguration(
        QSharedPointer<const Component> component, QSharedPointer<View> hierarchicalView) const;

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
        QSharedPointer<View> hierarchicalView, QSharedPointer<const DesignConfiguration> designConfiguration) const;

    /*!
     *  Gets the VLNV of the design referenced by the selected view.
     *
     *      @param [in] component               Component containing the selected view.
     *      @param [in] hierarchicalView        The selected view.
     *
     *      @return The VLNV of the design referenced by the selected view.
     */
    VLNV getHierarchicalDesignVLNV(QSharedPointer<const Component> component,
        QSharedPointer<View> hierarchicalView) const;
    
    /*!
     *  Creates connections from the given design and adds them into the graph.
     *
     *      @param [in] design              The design to analyze and transform into graph.
     *      @param [in] topInstanceName     The name of the top component instance.
     *      @param [in/out] graph           The graph to add elements into.
     */
    void addConnections(QSharedPointer<const Design> design, QString const& topInstanceName,
        QSharedPointer<ConnectivityGraph> graph);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* library_;

    QSharedPointer<ComponentParameterFinder> parameterFinder_;

    //! Parser for resolving expressions.
    ExpressionParser* expressionParser_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNERDIAGRAM_H
