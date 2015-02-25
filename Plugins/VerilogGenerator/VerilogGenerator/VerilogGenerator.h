//-----------------------------------------------------------------------------
// File: VerilogGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Verilog file generator.
//-----------------------------------------------------------------------------

#ifndef VERILOGGENERATOR_H
#define VERILOGGENERATOR_H

#include "../veriloggeneratorplugin_global.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/design.h>

#include <QTextStream>


class ComponentVerilogWriter;
class ComponentInstanceVerilogWriter;
class ExpressionFormatter;
class LibraryInterface;
class PortSorter;
class VerilogHeaderWriter;
class VerilogWireWriter;
class Writer;
class WriterGroup;

//-----------------------------------------------------------------------------
// Verilog file generator.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT VerilogGenerator
{
public:
    //! The constructor.
    VerilogGenerator(LibraryInterface* library);

    //! The destructor.
    ~VerilogGenerator();

    /*!
     *  Parses a given component for generation.
     *
     *      @param [in] component           The component to parse for generation.
     *      @param [in] topComponentView    The component view to parse for generation.
     *      @param [in] design              The design to parse for generation.
     *
     *      @remark If parse() is not called before generate(), nothing is generated.
     */
    void parse(QSharedPointer<Component> component, QString const& topComponentView, 
        QSharedPointer<Design> design = QSharedPointer<Design>());
    
    /*!
     *  Generates the component Verilog to a given file.
     *
     *      @param [in] outputPath   The path to the output file.
     *
     *      @remark If parse() is not called before generate(), nothing is generated.
     */
    void generate(QString const& outputPath) const;

private:

    // Disable copying.
    VerilogGenerator(VerilogGenerator const& rhs);
    VerilogGenerator& operator=(VerilogGenerator const& rhs);

    /*!
     *  Initializes writers for parsing.
     */
    void initializeWriters(QString const& topComponentView);

    /*!
     *  Creates an expression formatter for the given component.
     *
     *      @param [in] targetComponent   The component for which to create the formatter.
     *
     *      @return Expression formatter for the component.
     */
    QSharedPointer<ExpressionFormatter> createFormatterForComponent(QSharedPointer<Component> targetComponent);

    /*!
    *  Checks if the generator should write nothing.
    *
    *      @return True, if the generator has nothing to write, otherwise false.
    */
    bool nothingToWrite() const;

    /*!
     *  Parses the interconnections in the design creating wires to connect instances.
     */
    void createWiresForInterconnections();

    /*!
     *  Creates wires for all logical signals in a given interface.
     *
     *      @param [in] interface   The interface for which to create wires.
     */
    void createWiresForInterface(Interface const& interface);

        /*!
     *  Creates wires for a given logical signal in a given interface.
     *
     *      @param [in] logicalSignal   The logical signal for which to create wires.
     *      @param [in] interface       The interface for which to create wires.
     */
    void createWireForLogicalSignalInInterface(QString const& logicalSignal, Interface const& interface);

    /*!
     *  Gets the primary side interfaces in all interconnections. A primary interface e.g. master is potentially 
     *  connected to multiple secondary sides e.g. slave.
     *
     *      @return All primary side interfaces.
     */
    QList<Interface> getPrimaryInterfacesForInterconnections();

    /*!
     *  Generates a name for a wire.
     *
     *      @param [in] primaryInterface    The primary side interface.
     *      @param [in] logicalPort         The logical name in the connected bus interface port map.
     *
     *      @return Generated name for a wire.
     */
    QString generateWireName(Interface const& primaryInterface, QString const& logicalPort);

    /*!
     *  Finds the name of the interconnection for an interface.
     *
     *      @param [in] interface   The interface whose interconnection's name to find.
     *
     *      @return The name of the interconnection.
     */
    QString findConnectionNameForInterface(Interface interface);

    /*!
     *  Finds the appropriate size for a wire.
     *
     *      @param [in] primaryInterface  The primary side interface for the wire.
     *      @param [in] logicalName       The corresponding logical signal name.
     *
     *      @return The size for the wire.
     */
    int findWireSize(Interface const& primaryInterface, QString const& logicalName);
    
    /*!
     *  Finds the highest connected logical bit index for a logical signal in a given interface.
     *
     *      @param [in] logicalName     The logical signal whose highest connected bit index to find.
     *      @param [in] interface       The interface whose logical mappings to search through.
     *
     *      @return The highest connected logical bit index in the interface.
     */
    int findHighestMappedLogicalIndexInInterface(QString const& logicalName, Interface const& interface);

    /*!
     *  Finds the logical bounds for a port map in an instance.
     *
     *      @param [in] instanceName    The name of the instance.
     *      @param [in] portMap         The port map for the which to find the logical bounds.
     *
     *      @return The port bounds for the port map in an instance.
     */
    General::PortBounds logicalBoundsInInstance(QString const& instanceName, 
        QSharedPointer<PortMap> portMap) const;

    /*!
     *  Finds the component referenced in the design by a given name.
     *
     *      @param [in] instanceName   The name of the instance referencing a component.
     *
     *      @return The component referenced by the instance.
     */
    QSharedPointer<Component> getComponentForInstance(QString const& instanceName) const;

    /*!
     *  Finds all port maps for a logical port in an interface.
     *
     *      @param [in] logicalPort     The name of the logical port.
     *      @param [in] interface       The interface whose port maps to find.
     *
     *      @return The port maps for the logcial signal in the interface.
     */
    QList<QSharedPointer<PortMap> > findPortMapsForLogicalPortInInterface(QString const& logicalPort,
        Interface const& interface) const;

    /*!
     *  Connects a wire to all instances connected to the given interface.
     *
     *      @param [in] startInterface  The interface whose connected ends to map.
     *      @param [in] logicalPort     The name of the logical signal to map.
     *      @param [in] wireName        The name of the wire to map.
     *      @param [in] wireSize        The size of the wire to map.
     */
    void connectPortsInConnectedInstancesToWire(Interface const& startInterface, QString const& logicalPort, 
        QString const& wireName, int const& wireSize);

    /*!
     *  Connects all ports mapped to a given logical signal in an interface to a given wire.
     *
     *      @param [in] interface       The interface whose ports to map.
     *      @param [in] logicalPort     The logical signal whose ports to map.
     *      @param [in] wireName        The name of the wire to map.
     *      @param [in] wireSize        The size of the wire to map.
     *
     *      @return <Description>.
     */
    void connectPortsInInterfaceToWire(Interface const& interface, QString const& logicalPort,
        QString const& wireName, int const& wireSize);

    /*!
     *  Connects a port in an component instance to given wire.
     *
     *      @param [in] instanceName    The name of the component instance.
     *      @param [in] portMap         The port map in the component for the mapping.
     *      @param [in] wireName        The name of the wire to map to.
     *      @param [in] wireSize        The size of the wire to map to.
     */
    void connectInstancePortToWire(QString const& instanceName, QSharedPointer<PortMap> portMap, 
        QString const& wireName, int const& wireSize);

    /*!
     *  Adds a writer for a wire with the given name and size.
     *
     *      @param [in] wireName   The name of the wire.
     *      @param [in] wireSize   The size of the wire.
     */
    void addWriterForWire(QString wireName, int wireSize);

    /*!
     *  Finds the interface mode in the component instance and bus interface described by an interface.
     *
     *      @param [in] interface   The interface whose interface mode to find.
     *
     *      @return The interface mode of the bus interface in the component referenced in the interface.
     */
    General::InterfaceMode interfaceModeForInterface(Interface const& interface);

    /*!
     *  Finds the bus interface referenced by an interface.
     *
     *      @param [in] interface   The interface referencing a bus interface in a component instance.
     *
     *      @return The referenced bus interface.
     */
    QSharedPointer<BusInterface> getBusinterfaceForInterface(Interface const& interface) const;

    /*!
     *  Parses all the component instances in the design.
     */
    void createWritersForComponentInstances();

    /*!
     *  Parses all the hierarchical connections for a component instance in the design and connects
     *  the instance ports to top ports.
     */
    void connectHierarchicalConnectionsToInstances();

    /*!
     *  *  Creates port maps to instance for bus interface with a hierarchical connection..
     *
     *      @param [in] topIf               The connected top-level interface.
     *      @param [in] instanceInterface   The connected interface.
     */
    void connectTopBusInterfaceToInterfaceInInstance(QSharedPointer<BusInterface> topIf,
        Interface const& instanceInterface);  
    
    /*!
     *  Checks if an instance port can be directly connected to a top port.
     *
     *      @param [in] instancePortWidth   The width of the physical port in the instance.
     *      @param [in] topPortWidth        The width of the physical port in the top component.
     *      @param [in] alignment           The calculated port alignment for the port map connecting the ports.
     *
     *      @return True, if the instance port can be directly connected to the top port, otherwise false.
     */
    bool canConnectForFullWidth(int instancePortWidth, int topPortWidth, 
        General::PortAlignment const& alignment) const;

    /*!
     *  Parses all the ad-hoc connections in the design.
     */
    void connectAndWireAdHocConnections();

    /*!
     *  Checks if a given ad-hoc connection is hierarchical.
     *
     *      @param [in] adHocConnection   The ad-hoc connection to check.
     *
     *      @return True, if the ad-hoc connection is hierarchical, otherwise false.
     */
    bool isHierarchicalAdHocConnection(AdHocConnection const &adHocConnection);

    /*!
     *  Adds port connection to all component instances connected by the given hierarchical ad-hoc connection.
     *
     *      @param [in] adHocConnection   The ad-hoc connection whose connected instances to map.
     */
    void connectInstancePortsTopPort(AdHocConnection const& adHocConnection);

    /*!
     *  Checks if a wire should be created to represent the given ad-hoc connection.
     *
     *      @param [in] adHocConnection   The ad-hoc connection to check.
     *
     *      @return True, if a wire should be created, otherwise false.
     */
    bool shouldCreateWireForAdHocConnection(AdHocConnection const& adHocConnection);

    /*!
     *  Creates a wire writer to represent the given ad-hoc connection and connects ports in 
     *  the ad-hoc connection to the wire.
     *
     *      @param [in] adHocConnection   The ad-hoc connection to create as wire and to connect.
     */
    void createWireForAdHocConnection(AdHocConnection const& adHocConnection);

    /*!
     *  Finds the required size for a wire representing a given ad-hoc connection.
     *
     *      @param [in] adHocConnection   The ad-hoc connection whose size to find.
     *
     *      @return The size of the wire required to represent the ad-hoc connection.
     */
    int findWireSizeForAdHocConnection(AdHocConnection const &adHocConnection) const;

    /*!
     *  Connects all ports referenced in an ad-hoc connection to the given wire name.
     *
     *      @param [in] adHocConnection     The ad-hoc connection whose referenced ports to connect.
     *      @param [in] wireName            The name of the wire to connect.
     */
    void connectPortsInAdHocConnectionToWire(AdHocConnection const& adHocConnection, QString const& wireName);

     /*!
      *  Adds the generated writers to the top writer in correct order.            
      */
    void addWritersToTopInDesiredOrder() const;

    /*!
     *  Creates a writer for writing a preceding header for a component instance.
     *
     *      @param [in] instanceName   The name of the component instance to create the writer for.
     *
     *      @return A writer for the header.
     */
    QSharedPointer<Writer> createHeaderWriterForInstance(QString const& instanceName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

     //! The component library.
     LibraryInterface* library_;

     //! Writer for generating file header.
     QSharedPointer<VerilogHeaderWriter> headerWriter_;

     //! Writer for top level component.
     QSharedPointer<ComponentVerilogWriter> topWriter_;

     //! The top level component.
     QSharedPointer<Component> topComponent_;

     //! The design to parse.
     QSharedPointer<Design> design_;

     //! Writers for Verilog wires.
     QSharedPointer<WriterGroup> wireWriters_;

     //! Writers for Verilog instances.
     QMap<QString, QSharedPointer<ComponentInstanceVerilogWriter> > instanceWriters_;

     //! Sorter for component ports.
     QSharedPointer<PortSorter> sorter_;

};

#endif // VERILOGGENERATOR_H
