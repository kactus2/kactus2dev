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

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/Component/PortMap.h>

#include <QTextStream>

class ComponentVerilogWriter;
class ComponentInstanceVerilogWriter;
class VerilogTiedValueWriter;
class ExpressionFormatter;
class LibraryInterface;
class PortSorter;
class VerilogHeaderWriter;
class VerilogWireWriter;
class Writer;
class WriterGroup;

class ExpressionParser;
class PortAlignment;

//-----------------------------------------------------------------------------
// Verilog file generator.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT VerilogGenerator : public QObject
{
	Q_OBJECT

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
	 *      @param [in] implementation	The implementation within the module, which will be written back to the file.
     *
     *      @remark If parse() is not called before generate(), nothing is generated.
     */
    void parse(QSharedPointer<Component> component, QString topComponentView, 
        QString const& outputPath = QString(""), QSharedPointer<Design> design = QSharedPointer<Design>() );
    
    /*!
     *  Generates the component Verilog to a given file.
     *
     *      @param [in] outputPath   The path to the output file.
     *
     *      @remark If parse() is not called before generate(), nothing is generated.
     */
	void generate(QString const& outputPath) const;
	
    /*!
     *  Parses the module implementation out of verilog file given as output, if it already exists.
     *
	 *      @param [in] outputPath		The path to the output file.
	 *      @param [in] implementation	The module implementation.
	 *      @param [in] postModule		Anything that exists after the module declaration.
	 *
	 *      @return False, if the file exists, but could not be opened or parsed properly.
     */
	bool selectImplementation(QString const& outputPath, QString& implementation,
		QString& postModule) const;

signals:
	
    /*!
     *  Emitted when reportable error occurs.
     */
	void reportError(const QString& errorMessage) const;

private:

    // Disable copying.
    VerilogGenerator(VerilogGenerator const& rhs);
    VerilogGenerator& operator=(VerilogGenerator const& rhs);

    /*!
     *  Initializes writers for parsing.
     */
	void initializeWriters();

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
    void createWiresForInterface(QSharedPointer<ActiveInterface> interface);

        /*!
     *  Creates wires for a given logical signal in a given interface.
     *
     *      @param [in] logicalSignal   The logical signal for which to create wires.
     *      @param [in] interface       The interface for which to create wires.
     */
    void createWireForLogicalSignalInInterface(QString const& logicalSignal, QSharedPointer<ActiveInterface> interface);

    /*!
     *  Gets the primary side interfaces in all interconnections. A primary interface e.g. master is potentially 
     *  connected to multiple secondary sides e.g. slave.
     *
     *      @return All primary side interfaces.
     */
    QList<QSharedPointer<ActiveInterface> > getPrimaryInterfacesForInterconnections();

    /*!
     *  Check if the selected primary interface is unique.
     *
     *      @param [in] connectedInterfaces     The list of currently connected interfaces.
     *      @param [in] primaryInterface        The selected primary interface.
     */
    bool primaryInterfaceIsUnique(QList<QSharedPointer<ActiveInterface> > const& connectedInterfaces,
        QSharedPointer<ActiveInterface> primaryInterface) const;

    /*!
     *  Generates a name for a wire.
     *
     *      @param [in] primaryInterface    The primary side interface.
     *      @param [in] logicalPort         The logical name in the connected bus interface port map.
     *
     *      @return Generated name for a wire.
     */
    QString generateWireName(QSharedPointer<ActiveInterface> primaryInterface, QString const& logicalPort);

    /*!
     *  Finds the name of the interconnection for an interface.
     *
     *      @param [in] interface   The interface whose interconnection's name to find.
     *
     *      @return The name of the interconnection.
     */
    QString findConnectionNameForInterface(QSharedPointer<ActiveInterface> interface);

    /*!
     *  Finds the appropriate size for a wire.
     *
     *      @param [in] primaryInterface  The primary side interface for the wire.
     *      @param [in] logicalName       The corresponding logical signal name.
     *
     *      @return The size for the wire.
     */
    int findWireSize(QSharedPointer<ActiveInterface> primaryInterface, QString const& logicalName);
    
    /*!
     *  Find the list of interfaces connected to the selected primary interface.
     *
     *      @param [in] primaryInterface    The selected primary interface.
     *
     *      @return A list of the interfaces connected to the primary interface.
     */
    QList<QSharedPointer<ActiveInterface> > getConnectedInterfaces(
        QSharedPointer<ActiveInterface> primaryInterface) const;

    /*!
     *  Check if the interfaces share component and bus references.
     *
     *      @param [in] comparisonInterface     The first interface being compared.
     *      @param [in] comparedInterface       The second interface being compared.
     */
    bool interfacesShareReferences(QSharedPointer<ActiveInterface> comparisonInterface,
        QSharedPointer<ActiveInterface> comparedInterface) const;

    /*!
     *  Finds the highest connected logical bit index for a logical signal in a given interface.
     *
     *      @param [in] logicalName     The logical signal whose highest connected bit index to find.
     *      @param [in] interface       The interface whose logical mappings to search through.
     *
     *      @return The highest connected logical bit index in the interface.
     */
    int findHighestMappedLogicalIndexInInterface(QString const& logicalName, QSharedPointer<ActiveInterface> interface);

    /*!
     *  Finds the logical bounds for a port map in an instance.
     *
     *      @param [in] instanceName    The name of the instance.
     *      @param [in] portMap         The port map for the which to find the logical bounds.
     *
     *      @return The port bounds for the port map in an instance.
     */
    QPair<int, int> logicalBoundsInInstance(QString const& instanceName, QSharedPointer<PortMap> portMap) const;

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
        QSharedPointer<ActiveInterface> interface) const;

    /*!
     *  Connects a wire to all instances connected to the given interface.
     *
     *      @param [in] startInterface  The interface whose connected ends to map.
     *      @param [in] logicalPort     The name of the logical signal to map.
     *      @param [in] wireName        The name of the wire to map.
     *      @param [in] wireSize        The size of the wire to map.
     */
    void connectPortsInConnectedInstancesToWire(QSharedPointer<ActiveInterface> startInterface, QString const& logicalPort, 
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
    void connectPortsInInterfaceToWire(QSharedPointer<ActiveInterface> interface, QString const& logicalPort,
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
    General::InterfaceMode interfaceModeForInterface(QSharedPointer<ActiveInterface> interface);

    /*!
     *  Finds the bus interface referenced by an interface.
     *
     *      @param [in] interface   The interface referencing a bus interface in a component instance.
     *
     *      @return The referenced bus interface.
     */
    QSharedPointer<BusInterface> getBusinterfaceForInterface(QSharedPointer<ActiveInterface> interface) const;

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
     *  Creates port maps to instance for bus interface with a hierarchical connection..
     *
     *      @param [in] topIf               The connected top-level interface.
     *      @param [in] portParser          The expression parser for the top component.
     *      @param [in] instanceInterface   The connected interface.
     */
    void connectTopBusInterfaceToInterfaceInInstance(QSharedPointer<BusInterface> topIf,
        QSharedPointer<ExpressionParser> topParser, QSharedPointer<ActiveInterface> instanceInterface);

    /*!
     *  Calculate the mapped port bounds.
     *
     *      @param [in] containingMap       The port map containing the selected port.
     *      @param [in] portLeft            The left bound of the selected port.
     *      @param [in] portRight           The right bound of the selected port.
     *      @param [in] parser              The used expression parser.
     *      @param [in] logicalPortWidth    The width of the logical port.
     *
     *      @return The aligned port.
     */
    PortAlignment calculateMappedPortBounds(QSharedPointer<PortMap> containingMap, int portLeft,
        int portRight, QSharedPointer<ExpressionParser> parser, QString logicalPortWidth) const;

    /*!
     *  Checks if an instance port can be directly connected to a top port.
     *
     *      @param [in] topAlignment        The alignment of the mapped top port.
     *      @param [in] instanceAlignment   The alignment of the mapped instance port.
     *      @param [in] topWidth            The width of the top port.
     *      @param [in] instanceWidth       The width of the instance port.
     *
     *      @return True, if the ports can be directly connected,
     */
    bool fullWidthConnectionIsValid(PortAlignment const& topAlignment,
        PortAlignment const&instanceAlignment, int topWidth, int instanceWidth) const;

    /*!
     *  Parses all the ad-hoc connections in the design.
     */
    void connectAndWireAdHocConnections();

    /*!
     *  Parse an ad hoc connection with a tied value.
     *
     *      @param [in] adHocConnection     The selected ad hoc connection.
     */
    void connectTieOffConnection(QSharedPointer<AdHocConnection> adHocConnection);

    /*!
     *  Get the tied value for an ad hoc connection.
     *
     *      @param [in] adHocConnection         The selected ad hoc connection.
     *      @param [in] containingComponent     Component containing the ad hoc port.
     *      @param [in] portReference           The connection port reference.
     *      @param [in] requiredDirection       The direction that is required for a tied value.
     *
     *      @return The tie off value.
     */
    QString getConnectionTieOffValue(QSharedPointer<AdHocConnection> adHocConnection,
        QSharedPointer<Component> containingComponent, QSharedPointer<PortReference> portReference,
        DirectionTypes::Direction requiredDirection) const;

    /*!
     *  Checks if a given ad-hoc connection is hierarchical.
     *
     *      @param [in] adHocConnection   The ad-hoc connection to check.
     *
     *      @return True, if the ad-hoc connection is hierarchical, otherwise false.
     */
    bool isHierarchicalAdHocConnection(QSharedPointer<AdHocConnection> adHocConnection) const;

    /*!
     *  Adds port connection to all component instances connected by the given hierarchical ad-hoc connection.
     *
     *      @param [in] adHocConnection   The ad-hoc connection whose connected instances to map.
     */
    void connectInstancePortsToTopPort(QSharedPointer<AdHocConnection> adHocConnection);

    /*!
     *  Gets the primary ports in all ad hoc connections. A primary port (direction out) is potentially 
     *  connected to multiple secondary sides (in ports).
     *
     *      @return The primary ports in ad hoc connections.
     */
    QVector<QSharedPointer<PortReference> > findPrimaryPortsInAdHocConnections() const;

    /*!
     *  Finds the physical port the referenced component.
     *
     *      @param [in] portReference   The reference for the port to find.
     *
     *      @return The found physical port.
     */
    QSharedPointer<Port> findPhysicalPort(QSharedPointer<PortReference> portReference) const;

    /*!
     *  Checks if the given port reference is unique within the given references.
     *
     *      @param [in] reference           The reference to check.
     *      @param [in] otherReferences     The set of references to find the given reference in.
     *
     *      @return True, if no item in otherReferences refers the same port, otherwise false.
     */
    bool isUniquePortReference(QSharedPointer<PortReference> reference, 
        QVector<QSharedPointer<PortReference> > const& otherReferences) const;

    /*!
     *  Checks if the given port references refer the same port.
     *
     *      @param [in] firstPort   The first reference.
     *      @param [in] secondPort  The second reference.
     *
     *      @return True, if the references are for the same port in the same component instance, otherwise false.
     */
    bool referencesTheSamePort(QSharedPointer<PortReference> firstPort, 
        QSharedPointer<PortReference> secondPort) const;

    /*!
     *  Creates a wire for the given primary ad-hoc port and wires the connected ports to it.
     *
     *      @param [in] primaryPort   The primary (output) port to create the wire for.
     */
    void wireConnectedPorts(QSharedPointer<PortReference> primaryPort);

    /*!
     *  Finds the ad-hoc connection for the given port reference.
     *
     *      @param [in] primaryPort   The port reference whose ad-hoc connection to find.
     *
     *      @return The ad-hoc connection for the given port reference.
     */
    QSharedPointer<AdHocConnection> findAdHocConnectionForPort(QSharedPointer<PortReference> primaryPort) const;

    /*!
     *  Finds the ports connected to the given primary (output) port.
     *
     *      @param [in] primaryPort   The port whose opposing ports to find.
     *
     *      @return The ports connected to the given port.
     */
    QVector<QSharedPointer<PortReference> > findConnectedPorts(QSharedPointer<PortReference> primaryPort) const;

     /*!
      *  Connects the given port to the given wire.
      *
      *      @param [in] port       The port to connect to the wire.
      *      @param [in] wireName   The name of the wire to connect.
      *      @param [in] wireSize   The size of the wire to connect.
      *
      *      @return <Description>.
      */
     void connectPortToWire(QSharedPointer<PortReference> port, QString const& wireName, int wireSize);

    /*!
     *  Checks if a wire should be created to represent the given ad-hoc connection.
     *
     *      @param [in] adHocConnection   The ad-hoc connection to check.
     *
     *      @return True, if a wire should be created, otherwise false.
     */
    bool shouldCreateWireForAdHocConnection(QSharedPointer<AdHocConnection> adHocConnection) const;

    /*!
     *  Finds the required size for a wire representing a given ad-hoc connection.
     *
     *      @param [in] adHocConnection   The ad-hoc connection whose size to find.
     *
     *      @return The size of the wire required to represent the ad-hoc connection.
     */
    int findWireSizeForAdHocConnection(QSharedPointer<AdHocConnection> adHocConnection) const;
   
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

     //! The active view for top component.
     QString topComponentView_;

     //! Writers for Verilog wires.
     QSharedPointer<WriterGroup> wireWriters_;

     //! Writer for ad hoc tied values.
     QSharedPointer<VerilogTiedValueWriter> tiedValueWriter_;

     //! Writers for Verilog instances.
     QMap<QString, QSharedPointer<ComponentInstanceVerilogWriter> > instanceWriters_;

     //! Sorter for component ports.
     QSharedPointer<PortSorter> sorter_;

};

#endif // VERILOGGENERATOR_H
