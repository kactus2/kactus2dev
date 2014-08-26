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

#include <QTextStream>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/design.h>

#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>

class LibraryInterface;
class ComponentInstanceVerilogWriter;
class VerilogHeaderWriter;
class VerilogWireWriter;


//-----------------------------------------------------------------------------
// Verilog file generator.
//-----------------------------------------------------------------------------
class VerilogGenerator
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
    int findWireSize(Interface const& startInterface, QSharedPointer<General::PortMap> startPortMap);

    /*!
     *  Finds the logical bounds for a port map in an instance.
     *
     *      @param [in] instanceName    The name of the instance.
     *      @param [in] portMap         The port map for the which to find the logical bounds.
     *
     *      @return The port bounds for the port map in an instance.
     */
    General::PortBounds logicalBoundsInInstance(QString const& instanceName, 
        QSharedPointer<General::PortMap> portMap) const;

    /*!
     *  Finds all port maps for a logical port in an interface.
     *
     *      @param [in] logicalPort     The name of the logical port.
     *      @param [in] interface       The interface whose port maps to find.
     *
     *      @return The port maps for the logcial signal in the interface.
     */
    QList<QSharedPointer<General::PortMap> > findPortMaps(QString const& logicalPort,
        Interface const& interface) const;

    /*!
     *  Maps a port in an component instance to given wire.
     *
     *      @param [in] instanceName    The name of the component instance.
     *      @param [in] portMap         The port map in the component for the mapping.
     *      @param [in] wireName        The name of the wire to map to.
     *      @param [in] wireSize        The size of the wire to map to.
     */
    void mapInstancePortToWire(QString const& instanceName, QSharedPointer<General::PortMap> portMap, 
        QString const& wireName, int const& wireSize);

    /*!
     *  Maps a wire to all instances connected to the given interface.
     *
     *      @param [in] startInterface  The interface whose connected ends to map.
     *      @param [in] logicalPort     The name of the logical signal to map.
     *      @param [in] wireName        The name of the wire to map.
     *      @param [in] wireSize        The size of the wire to map.
     */
    void mapToConnectedInstances(Interface const& startInterface, QString const& logicalPort, 
        QString const& wireName, int const& wireSize);

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
    QSharedPointer<BusInterface> businterfaceForInterface(Interface const& interface) const;

    /*!
     *  Parses all the component instances in the design.
     *
     *      @param [in] design   The design to parse.
     */
    void parseComponentInstances();

    /*!
     *  Parses all the hierarchical connections for a component instance in the design.
     *
     *      @param [in] instanceComponent   The component for the instance.
     *      @param [in] instanceWriter      The the writer for the instance.
     *      @param [in] design              The design to parse.
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
      *  Adds the generated writers to the top writer in correct order.            
      */
    void addWritersToTopInDesiredOrder() const;

     //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

     //! The component library.
     LibraryInterface* library_;

     //! Writer for generating file header.
     VerilogHeaderWriter* headerWriter_;

     //! Writer for top level component.
     ComponentVerilogWriter* topWriter_;

     //! The top level component.
     QSharedPointer<Component> topComponent_;

     //! The design to parse.
     QSharedPointer<Design> design_;

     //! Writers for Verilog wires.
     QList<VerilogWireWriter*> wireWriters_;

     //! Writers for Verilog instances.
     QMap<QString, ComponentInstanceVerilogWriter*> instanceWriters_;

     //! Sorter for component ports.
     QSharedPointer<PortSorter> sorter_;

};

#endif // VERILOGGENERATOR_H
