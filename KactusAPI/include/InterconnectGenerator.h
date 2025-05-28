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
#include <kactusAPI/KactusAPIGlobal.h>
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

struct TargetData {
    QSharedPointer<BusInterface> targetBus;
    QString start;
    QString range;
};

struct BusInterfaceInfo {
    std::string name;
    std::string mode;
    QString start;
    QString range;
};

class KACTUS2_API InterconnectGenerator
{
public:
    //! The constructor
    InterconnectGenerator(LibraryInterface* library,  MessageMediator* messager);

    //! The destructor
    ~InterconnectGenerator() = default;

    VLNV generate();

    void generate(ConfigStruct* config, const QHash<QString, QList<QSharedPointer<BusInterface>>>& initiators,
        const QHash<QString, QList<QSharedPointer<TargetData>>>& targets);

    //! No copying. No assignment.
    InterconnectGenerator(const InterconnectGenerator& other) = delete;
    InterconnectGenerator& operator=(const InterconnectGenerator& other) = delete;

private:

    void openDesign(VLNV designVLNV);

    void createInterconComponent(VLNV VLNV);

    void findUnconnectedInterfaces();

    void createBusInterface(std::string busName, std::string modeString, int index);

    std::string createBusInterface(VLNV busVLNV, std::string busName, std::string modeString, int index);

    void createRstorClkInterface(std::string busName, int index);

    void finalizeBusInterfaceCustomization(const std::vector<BusInterfaceInfo>& createdBuses);

    void processStartingPointsAndEndpoints(
        const QHash<QString, QList<QSharedPointer<BusInterface>>>& startingPoints,
        const QHash<QString, QList<QSharedPointer<TargetData>>>& endpoints);

    std::vector<BusInterfaceInfo> processEndpointSide(
        const QHash<QString, QList<QSharedPointer<TargetData>>>& endpoints, int& index);

    std::vector<BusInterfaceInfo> processStartingSide(
        const QHash<QString, QList<QSharedPointer<BusInterface>>>& startingPoints, int& index);

    BusInterfaceInfo createInterfaceForBus(
        const QString& instanceName, const QSharedPointer<BusInterface>& bus,
        bool isTop, bool isEndpoint, int& index);

    QSharedPointer<ConfigurableVLNVReference> resolveComponentVLNV(
        const QString& instanceName, bool isTop) const;

    std::string getInterfaceMode(QSharedPointer<BusInterface> bus, bool isTarget, bool useChannel, bool isTop);

    bool isTargetInterface(const QSharedPointer<BusInterface>& bus) const;

    quint64 parseIpxactHex(const QString& str, bool* ok);

    void createAddressSpace(std::string busName, QString range, QString width = "32");

    void createGlobalAddressSpaceFromTargets(const QHash<QString, QList<QSharedPointer<TargetData>>>& targets);

    std::string getLogicalPortName(QSharedPointer<Component> comp, QString busName, QString physicalName) const;

    std::pair<QString, QString> getWidthBoundsFromAbstraction(const std::string& logicalName) const;

    std::pair<QString, QString> getMirroredWidthBounds(const QString& physicalName) const;

    void createChannel();

    void createPortMaps(std::string modeString, QSharedPointer<BusInterface> busInf);

    void createPhysPorts(QSharedPointer<Component> comp, QString busName, bool isTop = false);


    //! Message handler.
    MessageMediator* messager_{ nullptr };

    LibraryInterface* library_{ nullptr };

    QSharedPointer<Component> interconComponent_{ nullptr };

    //! Currently active design.
    QSharedPointer<Design> design_{ nullptr };

    //! Component parameter finder.
    QSharedPointer<ComponentParameterFinder> parameterFinder_{ new ComponentAndInstantiationsParameterFinder(nullptr) };

    //! Parser for expressions.
    QSharedPointer<ExpressionParser> expressionParser_{ new IPXactSystemVerilogParser(parameterFinder_) };

    //! Formatter for expressions.
    QSharedPointer<ExpressionFormatter>expressionFormatter_{ new ExpressionFormatter(parameterFinder_) };

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInfInterface_{ nullptr };

    //TransparentBridgeInterface* bridgeInterface_{ nullptr };

    //! Validator for ports.
    QSharedPointer<PortValidator> portValidator_{ new PortValidator(expressionParser_,
        QSharedPointer<QList<QSharedPointer<View> > >()) };

    //! Interface for accessing the component ports.
    PortsInterface* portsInterface_{ new PortsInterface(portValidator_, expressionParser_, expressionFormatter_) };

    PortAbstractionInterface* portAbsInterface_{ new PortAbstractionInterface(expressionParser_, expressionFormatter_) };

    //! Interface for accessing interconnections.
    InterconnectionInterface* connectionInterface_{ new InterconnectionInterface() };

    //! Interface for accessing ad hoc connections.
    AdHocConnectionInterface* adhocConnectionInterface_{ new AdHocConnectionInterface() };

    //! Interface for accessing component instances.
    ComponentInstanceInterface* instanceInterface_{ new ComponentInstanceInterface(connectionInterface_,
        adhocConnectionInterface_) };

    AbstractionTypeInterface* absTypeInf_{ nullptr };

    ConfigStruct* config_;

    QList<QSharedPointer<BusInterface>> initiators_;

    QList<QSharedPointer<BusInterface>> targets_;

    QString directory_;

    std::string prefix_;

    std::string globalAddressSpaceName_;

    std::string clockName_;

    std::string rstName_;

    VLNV busDefVLNV_;

    VLNV rstVLNV_;

    VLNV clkVLNV_;

    QString clkPort_;

    QString rstPort_;

    QString startAddr_;

    QString range_;

};

#endif // INTERCONNECTGENERATOR_H
