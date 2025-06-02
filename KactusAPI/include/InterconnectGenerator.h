#ifndef INTERCONNECTGENERATOR_H
#define INTERCONNECTGENERATOR_H

#include <IPXACTmodels/Component/validators/PortValidator.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/Component/TransparentBridge.h>
#include <IPXACTmodels/Component/AddressSpace.h>

#include <KactusAPI/include/ConfigJsonParser.h>
#include <KactusAPI/include/BusInterfaceInterfaceFactory.h>
#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/TransparentBridgeInterface.h>
#include <KactusAPI/include/LibraryInterface.h>
#include <KactusAPI/include/ComponentAndInstantiationsParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/PortsInterface.h>
#include <KactusAPI/include/PortAbstractionInterface.h>
#include <KactusAPI/include/ComponentInstanceInterface.h>
#include <KactusAPI/include/InterconnectionInterface.h>
#include <KactusAPI/include/AdHocConnectionInterface.h>
#include <KactusAPI/include/MessageMediator.h>
#include <KactusAPI/include/AbstractionTypeInterface.h>

#include <QSharedPointer>
#include <QHash>
#include <QString>

/*!
 *  Struct representing address-bound bus interface used as an endpoint.
 */
struct EndpointData {
    QSharedPointer<BusInterface> endpointBus;  //!< Endpoint bus interface.
    QString start;                             //!< Start address.
    QString range;                             //!< Address range.
};

/*!
 *  Struct representing bus interface information used for generation.
 */
struct BusInterfaceInfo {
    std::string name;  //!< Name of the bus interface.
    std::string mode;  //!< Mode of the interface.
    QString start;     //!< Optional start address.
    QString range;     //!< Optional address range.
};

/*!
 *  Generator class responsible for creating an interconnect component and related IP-XACT data.
 */
class KACTUS2_API InterconnectGenerator
{
public:
    /*!
     *  Constructor.
     *
     *  @param [in] library   Library interface to load/save IP-XACT models.
     *  @param [in] messager  Message mediator for reporting status/errors.
     */
    InterconnectGenerator(LibraryInterface* library, MessageMediator* messager);

    //! Default destructor.
    ~InterconnectGenerator() = default;

    /*!
     *  Generate an interconnect component based on configuration loaded from JSON.
     *  Starting point to generation from CLI
     *
     *  @return The VLNV of the generated component.
     */
    VLNV generate();

    /*!
     *  Generate an interconnect component from explicit configuration and connection data.
     *  Starting point to genration from UI configuration dialog
     *
     *  @param [in] config         Interconnect generation configuration.
     *  @param [in] startingPoints Map of initiator instances and interfaces.
     *  @param [in] endPoints      Map of endpoint instances and interfaces.
     */
    void generate(ConfigStruct* config,
        const QHash<QString, QList<QSharedPointer<BusInterface>>>& startingPoints,
        const QHash<QString, QList<QSharedPointer<EndpointData>>>& endPoints);

    //! No copy constructor.
    InterconnectGenerator(const InterconnectGenerator& other) = delete;

    //! No assignment.
    InterconnectGenerator& operator=(const InterconnectGenerator& other) = delete;

private:
    /*!
     *  Opens the design referenced by the provided VLNV and initializes the internal model.
     *
     *  @param [in] designVLNV  VLNV of the top-level design component.
     */
    void openDesign(VLNV designVLNV);

    /*!
     *  Creates a new interconnect component based on the given VLNV.
     *  Replaces any existing component with the same VLNV.
     *
     *  @param [in] VLNV  VLNV of the component to create.
     */
    void createInterconComponent(VLNV VLNV);

    /*!
     *  Searches the current design for unconnected interfaces matching the bus definition,
     *  and connects them to the generated interconnect.
     */
    void findUnconnectedInterfaces();

    /*!
     *  Processes starting points and endpoints and creates all related interfaces and connections.
     *
     *  @param [in] startingPoints  Map of initiators.
     *  @param [in] endpoints       Map of endpoints.
     */
    void processStartingPointsAndEndpoints(
        const QHash<QString, QList<QSharedPointer<BusInterface>>>& startingPoints,
        const QHash<QString, QList<QSharedPointer<EndpointData>>>& endpoints);

    /*!
     *  Processes all endpoint interfaces and returns their metadata.
     *
     *  @param [in] endpoints  Endpoint interface mapping.
     *  @param [in,out] index  Running index for naming.
     * 
     *  @return List of created bus interface metadata.
     */
    std::vector<BusInterfaceInfo> processEndpointSide(
        const QHash<QString, QList<QSharedPointer<EndpointData>>>& endpoints, int& index);

    /*!
     *  Processes all initiator interfaces and returns their metadata.
     *
     *  @param [in] startingPoints  Initiator interface mapping.
     *  @param [in,out] index       Running index for naming.
     * 
     *  @return List of created bus interface metadata.
     */
    std::vector<BusInterfaceInfo> processStartingSide(
        const QHash<QString, QList<QSharedPointer<BusInterface>>>& startingPoints, int& index);

    /*!
     *  Creates a new interface for a bus and returns its metadata.
     *
     *  @param [in] instanceName  Instance name the bus belongs to.
     *  @param [in] bus           Bus interface to replicate.
     *  @param [in] isTop         True if bus belongs to top-level component.
     *  @param [in] isEndpoint    True if the interface is a target.
     *  @param [in,out] index     Index used for interface naming.
     * 
     *  @return Metadata of created interface.
     */
    BusInterfaceInfo createInterfaceForBus(
        const QString& instanceName, const QSharedPointer<BusInterface>& bus,
        bool isTop, bool isEndpoint, int& index);

    /*!
     *  Creates a named bus interface with mode and type, used for auto-generated naming.
     *
     *  @param [in] busName     Logical name for the interface.
     *  @param [in] modeString  Interface mode string.
     *  @param [in] index       Index to insert at.
     */
    void createBusInterface(std::string busName, std::string modeString, int index);

    /*!
     *  Creates a fully defined bus interface and returns its name.
     *
     *  @param [in] busVLNV     Bus type definition.
     *  @param [in] busName     Base name for the interface.
     *  @param [in] modeString  Interface mode string.
     *  @param [in] index       Insertion index.
     * 
     *  @return Name of the created interface.
     */
    std::string createBusInterface(VLNV busVLNV, std::string busName, std::string modeString, int index);

    /*!
     *  Creates a clock or reset interface and maps ports appropriately.
     *
     *  @param [in] busName  Must be either "clk" or "rst".
     *  @param [in] index    Index for interface insertion.
     */
    void createRstorClkInterface(std::string busName, int index);

    /*!
     *  Creates port maps for a given bus and interface mode.
     *
     *  @param [in] modeString  Interface mode (e.g. "target").
     *  @param [in] busInf      Source bus interface.
     */
    void createPortMaps(std::string modeString, QSharedPointer<BusInterface> busInf);

    /*!
     *  Clones and adds physical ports from a component to the interconnect.
     *
     *  @param [in] comp     Source component.
     *  @param [in] busName  Bus interface name.
     *  @param [in] isTop    True if the port belongs to top-level component.
     */
    void createPhysPorts(QSharedPointer<Component> comp, QString busName, bool isTop = false);

    /*!
     *  Applies address space, subinterfaces, and bridges to the bus interfaces.
     *
     *  @param [in] createdBuses  List of bus interface metadata.
     */
    void finalizeBusInterfaceCustomization(const std::vector<BusInterfaceInfo>& createdBuses);

    /*!
     *  Creates an address space with provided name, range, and width.
     *
     *  @param [in] spaceName  Name of the address space.
     *  @param [in] range      Address range in IP-XACT format.
     *  @param [in] width      Address width (default = 32).
     */
    void createAddressSpace(std::string spaceName, QString range, QString width = "32");

    /*!
     *  Analyzes endpoint ranges and creates a shared global address space.
     *
     *  @param [in] endpoints  Endpoint mapping including start/range data.
     */
    void createGlobalAddressSpaceFromEndpoints(
        const QHash<QString, QList<QSharedPointer<EndpointData>>>& endpoints);

    /*!
     *  Creates a channel and assigns all appropriate interfaces to it.
     */
    void createChannel();

    /*!
     *  Resolves the VLNV of a component instance from the design.
     *
     *  @param [in] instanceName  Instance name.
     *  @param [in] isTop         Whether the instance is the top-level component.
     * 
     *  @return VLNV reference if found.
     */
    QSharedPointer<ConfigurableVLNVReference> resolveComponentVLNV(
        const QString& instanceName, bool isTop) const;

    /*!
     *  Determines if a bus is a target based on its interface mode.
     *
     *  @param [in] bus  Bus interface to evaluate.
     * 
     *  @return True if it's a target interface.
     */
    bool isTargetInterface(const QSharedPointer<BusInterface>& bus) const;

    /*!
     *  Computes correct interface mode string based on revision and context.
     *
     *  @param [in] bus         Bus to evaluate.
     *  @param [in] isTarget    True if it's a target.
     *  @param [in] isChannel   True if interconnect is channel-based.
     *  @param [in] isTop       True if it's a top-level interface.
     * 
     *  @return Corresponding interface mode string.
     */
    std::string getInterfaceMode(QSharedPointer<BusInterface> bus, bool isTarget, bool isChannel, bool isTop);

    /*!
     *  Extracts the logical port name from a physical port.
     *
     *  @param [in] comp          Component owning the port.
     *  @param [in] busName       Name of the bus interface.
     *  @param [in] physicalName  Name of the physical port.
     * 
     *  @return Matching logical port name if found.
     */
    std::string getLogicalPortName(QSharedPointer<Component> comp, QString busName, QString physicalName) const;

    /*!
     *  Gets the width bounds from the abstraction layer for a logical port.
     *
     *  @param [in] logicalName  Logical port name.
     * 
     *  @return Pair of left and right bounds as strings.
     */
    std::pair<QString, QString> getWidthBoundsFromAbstraction(const std::string& logicalName) const;

    /*!
     *  Retrieves mirrored bounds (left/right) for a given physical port.
     *
     *  @param [in] physicalName  Name of the physical port.
     * 
     *  @return Pair of bounds as strings.
     */
    std::pair<QString, QString> getMirroredWidthBounds(const QString& physicalName) const;

    /*!
     *  Parses a hexadecimal string from IP-XACT format into an integer.
     *
     *  @param [in] str  Input string.
     *  @param [out] ok  Will be set true if conversion succeeds.
     * 
     *  @return Parsed 64-bit value.
     */
    quint64 parseIpxactHex(const QString& str, bool* ok);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Message handler.
    MessageMediator* messager_{ nullptr };

    //! Library interface for model IO.
    LibraryInterface* library_{ nullptr };

    //! Interconnect component under construction.
    QSharedPointer<Component> interconComponent_{ nullptr };

    //! Currently loaded design.
    QSharedPointer<Design> design_{ nullptr };

    //! Parameter finder for expression parsing.
    QSharedPointer<ComponentParameterFinder> parameterFinder_{
        new ComponentAndInstantiationsParameterFinder(nullptr) };

    //! Expression parser.
    QSharedPointer<ExpressionParser> expressionParser_{
        new IPXactSystemVerilogParser(parameterFinder_) };

    //! Expression formatter.
    QSharedPointer<ExpressionFormatter> expressionFormatter_{
        new ExpressionFormatter(parameterFinder_) };

    //! Interface for managing bus interfaces.
    BusInterfaceInterface* busInfInterface_{ nullptr };

    //! Validator for ports.
    QSharedPointer<PortValidator> portValidator_{
        new PortValidator(expressionParser_,
            QSharedPointer<QList<QSharedPointer<View>> >()) };

    //! Interface for managing physical ports.
    PortsInterface* portsInterface_{
        new PortsInterface(portValidator_, expressionParser_, expressionFormatter_) };

    //! Interface for logical/physical abstraction mappings.
    PortAbstractionInterface* portAbsInterface_{
        new PortAbstractionInterface(expressionParser_, expressionFormatter_) };

    //! Interface for managing component instances.
    ComponentInstanceInterface* instanceInterface_{
        new ComponentInstanceInterface(connectionInterface_, adhocConnectionInterface_) };

    //! Interface for interconnections.
    InterconnectionInterface* connectionInterface_{ new InterconnectionInterface() };

    //! Interface for ad hoc connections.
    AdHocConnectionInterface* adhocConnectionInterface_{ new AdHocConnectionInterface() };

    //! Interface for abstraction type resolution.
    AbstractionTypeInterface* absTypeInf_{ nullptr };

    //! Configuration data for generation.
    ConfigStruct* config_;

    //! List of initiator interfaces.
    QList<QSharedPointer<BusInterface>> initiators_;

    //! List of target interfaces.
    QList<QSharedPointer<BusInterface>> targets_;

    //! Path to component save directory.
    QString directory_;

    //! Naming prefix for generated ports/interfaces.
    std::string prefix_;

    //! Name of the global address space.
    std::string globalAddressSpaceName_;

    //! Generated clock port/interface name.
    std::string clockName_;

    //! Generated reset port/interface name.
    std::string rstName_;

    //! VLNV of the main bus definition.
    VLNV busDefVLNV_;

    //! VLNV of the reset signal.
    VLNV rstVLNV_;

    //! VLNV of the clock signal.
    VLNV clkVLNV_;

    //! Logical port name for clock.
    QString clkPort_;

    //! Logical port name for reset.
    QString rstPort_;
};

#endif // INTERCONNECTGENERATOR_H
