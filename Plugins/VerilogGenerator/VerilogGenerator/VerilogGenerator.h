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

class LibraryInterface;
class ComponentVerilogWriter;
class ComponentInstanceVerilogWriter;
class VerilogHeaderWriter;
class VerilogWireWriter;
class WriterGroup;
class PortSorter;
class Writer;

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
     *  Generates the component Verilog to a given file.
     *
     *      @param [in] outputPath   The path to the output file.
     *
     *      @remark If parse() is not called before generate(), nothing is generated.
     */
    void generate(QString const& outputPath) const;

    /*!
     *  Parses a given component for generation.
     *
     *      @param [in] component   The component to parse for generation.
     *
     *      @remark If parse() is not called before generate(), nothing is generated.
     */
    void parse(QSharedPointer<Component> component, 
        QSharedPointer<Design> design = QSharedPointer<Design>());

private:

    // Disable copying.
    VerilogGenerator(VerilogGenerator const& rhs);
    VerilogGenerator& operator=(VerilogGenerator const& rhs);

    /*!
    *  Checks if the generator should write nothing.
    *
    *      @return True, if the generator has nothing to write, otherwise false.
    */
    bool nothingToWrite() const;

    /*!
     *  Parses the interconnections in the design creating port maps to instances and wire writers for
     *  connected ports.
     */
    void parseInterconnections();

    /*!
     *  Gets the master side interfaces in all interconnections. 
     *
     *      @return All master side interfaces.
     */
    QList<Interface> getStartInterfacesForInterconnections();

    /*!
     *  Generates a name for a wire.
     *
     *      @param [in] startInterface      The master side interface.
     *      @param [in] logicalPort         The logical name in the connected bus interface port map.
     *
     *      @return Generated name for a wire.
     */
    QString generateWireName(Interface const& startInterface, QString const& logicalPort);

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
     *      @param [in] startInterface  The master side interface for the wire.
     *      @param [in] startPortMap    The port map for the wire on the master side.
     *
     *      @return The size for the wire.
     */
    int findWireSize(Interface const& startInterface, QString const& logicalName);

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
    QSharedPointer<Component> getComponentForInstanceName(QString const& instanceName) const;

    /*!
     *  Finds all port maps for a logical port in an interface.
     *
     *      @param [in] logicalPort     The name of the logical port.
     *      @param [in] interface       The interface whose port maps to find.
     *
     *      @return The port maps for the logcial signal in the interface.
     */
    QList<QSharedPointer<PortMap> > findPortMaps(QString const& logicalPort,
        Interface const& interface) const;

    /*!
     *  Maps a wire to all instances connected to the given interface.
     *
     *      @param [in] startInterface  The interface whose connected ends to map.
     *      @param [in] logicalPort     The name of the logical signal to map.
     *      @param [in] wireName        The name of the wire to map.
     *      @param [in] wireSize        The size of the wire to map.
     */
    void mapConnectedInstances(Interface const& startInterface, QString const& logicalPort, 
        QString const& wireName, int const& wireSize);

    /*!
     *  Maps all ports mapped to a given logical signal in an interface to a given wire.
     *
     *      @param [in] interface       The interface whose ports to map.
     *      @param [in] logicalPort     The logical signal whose ports to map.
     *      @param [in] wireName        The name of the wire to map.
     *      @param [in] wireSize        The size of the wire to map.
     *
     *      @return <Description>.
     */
    void mapPortsInInterface(Interface const& interface, QString const& logicalPort,
        QString const& wireName, int const& wireSize);

    /*!
     *  Maps a port in an component instance to given wire.
     *
     *      @param [in] instanceName    The name of the component instance.
     *      @param [in] portMap         The port map in the component for the mapping.
     *      @param [in] wireName        The name of the wire to map to.
     *      @param [in] wireSize        The size of the wire to map to.
     */
    void mapInstancePortToWire(QString const& instanceName, QSharedPointer<PortMap> portMap, 
        QString const& wireName, int const& wireSize);

    /*!
     *  Adds a writer for a wire with the given name and size.
     *
     *      @param [in] wireName   The name of the wire.
     *      @param [in] wireSize   The size of the wire.
     */
    void addWireWriter(QString wireName, int wireSize);

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
    void parseComponentInstances();

    /*!
     *  Parses all the hierarchical connections for a component instance in the design.
     */
    void parseHierarchicalConnections();

    /*!
     *  *  Creates port maps to instance for bus interface with a hierarchical connection..
     *
     *      @param [in] topIf               The connected top-level interface.
     *      @param [in] instanceInterface   The connected interface.
     *      @param [in] instanceComponent   The instantiated component.     
     */
    void mapTopBusInterfaceToInterfaceInInstance(QSharedPointer<BusInterface> topIf, 
        Interface const& instanceInterface, QSharedPointer<Component> instanceComponent);  

    /*!
     *  Parses all the ad-hoc connections in the design.
     */
    void parseAdHocConnections();

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
    void createPortMapsForHierarchicalAdHocConnection(AdHocConnection const& adHocConnection);

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
    int wireSizeForAdHocConnection(AdHocConnection const &adHocConnection) const;

    /*!
     *  Connects all ports referenced in an ad-hoc connection to the given wire name.
     *
     *      @param [in] adHocConnection     The ad-hoc connection whose referenced ports to connect.
     *      @param [in] wireName            The name of the wire to connect.
     */
    void mapPortsInAdHocConnectionToWire(AdHocConnection const &adHocConnection, QString wireName);

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
