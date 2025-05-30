#include "InterconnectGenerator.h"

#include <IPXACTmodels/Component/Channel.h>

#include <KactusAPI/KactusAPI.h>
#include <KactusAPI/include/InterconnectRTLWriter.h>
#include <KactusAPI/include/BusInterfaceInterfaceFactory.h>
#include <KactusAPI/include/PortMapInterface.h>
#include <KactusAPI/include/PortAbstractionInterface.h>

InterconnectGenerator::InterconnectGenerator(LibraryInterface* library, MessageMediator* messager)
{
    library_ = library;
    messager_ = messager;
    config_ = nullptr;
}

VLNV InterconnectGenerator::generate()
{
    ConfigJsonParser parser;
    config_ = parser.readFile();
    VLNV designVLNV(VLNV::COMPONENT, config_->DesignVLNV);
    VLNV interconVLNV(VLNV::COMPONENT, config_->InterconVLNV);

    busDefVLNV_ = VLNV(VLNV::BUSDEFINITION, config_->BusVLNV);
    rstVLNV_ = VLNV(VLNV::BUSDEFINITION, config_->RstVLNV);
    clkVLNV_ = VLNV(VLNV::BUSDEFINITION, config_->ClkVLNV);
    openDesign(designVLNV);
    createInterconComponent(interconVLNV);
    findUnconnectedInterfaces();

    directory_ = KactusAPI::getDefaultLibraryPath();
    QString vlnvDir = "/" + interconComponent_->getVlnv().getVendor() + "/" + interconComponent_->getVlnv().getLibrary() + "/" +
        interconComponent_->getVlnv().getName() + "/" + interconComponent_->getVlnv().getVersion();

    directory_ += vlnvDir;
    messager_->showMessage(QString("Writing component %1 to file").arg(interconComponent_->getVlnv().toString()));
    library_->writeModelToFile(directory_, interconComponent_);

    InterconnectRTLWriter writer(interconComponent_, library_, messager_, directory_,
        config_, clkPort_, rstPort_);
    writer.generateRTL();
    return interconVLNV;
}

void InterconnectGenerator::generate(ConfigStruct* config, const QHash<QString, QList<QSharedPointer<BusInterface>>>& startingPoints,
    const QHash<QString, QList<QSharedPointer<EndpointData>>>& endpoints)
{
    config_ = config;

    VLNV designVLNV(VLNV::COMPONENT, config_->DesignVLNV);
    VLNV interconVLNV(VLNV::COMPONENT, config_->InterconVLNV);

    busDefVLNV_ = VLNV(VLNV::BUSDEFINITION, config_->BusVLNV);
    rstVLNV_ = VLNV(VLNV::BUSDEFINITION, config_->RstVLNV);
    clkVLNV_ = VLNV(VLNV::BUSDEFINITION, config_->ClkVLNV);
    openDesign(designVLNV);
    createInterconComponent(interconVLNV);
    processStartingPointsAndEndpoints(startingPoints, endpoints);

    if (config_->isChannel)
    {
        createChannel();
    }

    directory_ = KactusAPI::getDefaultLibraryPath();
    QString vlnvDir = "/" + interconComponent_->getVlnv().getVendor() + "/" + interconComponent_->getVlnv().getLibrary() + "/" +
        interconComponent_->getVlnv().getName() + "/" + interconComponent_->getVlnv().getVersion();

    directory_ += vlnvDir;
    messager_->showMessage(QString("Writing component %1 to file").arg(interconComponent_->getVlnv().toString()));

    bool writeSucceeded = true;
    if (!library_->writeModelToFile(directory_, interconComponent_))
    {
        writeSucceeded = false;
    }

    if (!library_->writeModelToFile(design_))
    {
        writeSucceeded = false;
    }

    if (!writeSucceeded) {
        messager_->showError("Error saving design to disk.");
    }

    InterconnectRTLWriter writer(interconComponent_, library_, messager_, directory_, config_, clkPort_, rstPort_);
    writer.generateRTL();
}

void InterconnectGenerator::openDesign(VLNV designVLNV)
{
    QSharedPointer<Document> designCompDocument = library_->getModel(designVLNV);

    QSharedPointer<Component> designComp = designCompDocument.dynamicCast<Component>();
    QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > list = designComp->getDesignConfigurationInstantiations();
    QSharedPointer<DesignConfigurationInstantiation> inst = list->at(0);
    QSharedPointer<ConfigurableVLNVReference> vlnv = inst->getDesignConfigurationReference();

    QSharedPointer<Document> designConfDocument = library_->getModel(*vlnv);
    QSharedPointer<DesignConfiguration> designConfig = designConfDocument.dynamicCast<DesignConfiguration>();

    QSharedPointer<Document> designDocument = library_->getModel(designConfig->getDesignRef());
    design_ = designDocument.dynamicCast<Design>();

    instanceInterface_->setComponentInstances(design_);
    connectionInterface_->setInterconnections(design_);
    adhocConnectionInterface_->setConnections(design_);
}

void InterconnectGenerator::createInterconComponent(VLNV VLNV)
{
    if (library_->contains(VLNV)) {
        library_->removeObject(VLNV);
    }
    messager_->showMessage("Creating new component");
    QSharedPointer<Component> component = QSharedPointer<Component>(new Component(VLNV, design_->getRevision()));

    component->setHierarchy(KactusAttribute::FLAT);
    component->setFirmness(KactusAttribute::MUTABLE);
    component->setImplementation(KactusAttribute::HW);
    component->setVersion(VLNV.getVersion());
    interconComponent_ = component;

    parameterFinder_->setComponent(interconComponent_);

    interconComponent_->getParameters()->append(config_->interconnectParams);

    busInfInterface_ = BusInterfaceInterfaceFactory::createBusInterface(parameterFinder_,
        expressionFormatter_, expressionParser_, interconComponent_, library_);

    busInfInterface_->setBusInterfaces(interconComponent_);
    absTypeInf_ = busInfInterface_->getAbstractionTypeInterface();

    instanceInterface_->addComponentInstance(VLNV.getName().toStdString());
    instanceInterface_->setComponentReference(VLNV.getName().toStdString(), VLNV.getVendor().toStdString(), VLNV.getLibrary().toStdString(),
        VLNV.getName().toStdString(), VLNV.getVersion().toStdString());

    messager_->showMessage("Component created and linked");
}

void InterconnectGenerator::findUnconnectedInterfaces()
{
    messager_->showMessage("Seaching for interfaces..");
    int index = 0;
    std::vector<std::string> compNames = instanceInterface_->getItemNames();

    for (std::string compName : compNames)
    {
        messager_->showMessage(QString("Comp name %1").arg(QString::fromStdString(compName)));
        if (compName != "interconnect") {
            auto compVLNV = instanceInterface_->getComponentReference(compName);
            QSharedPointer<Document> compDocument = library_->getModel(*compVLNV.dynamicCast<VLNV>());
            QSharedPointer<Component> comp = compDocument.dynamicCast<Component>();
            QStringList busNames = comp->getBusInterfaceNames();
            for (QString busName : busNames)
            {
                QSharedPointer<BusInterface> busInf = comp->getBusInterface(busName);
                VLNV busVLNV = busInf->getBusType();
                if (busVLNV == busDefVLNV_)
                {
                    if (!design_->hasInterconnection(QString::fromStdString(compName), busName))
                    {
                        messager_->showMessage("Unconnected interface found");
                        prefix_ = compName + "_";
                        General::InterfaceMode newMode = General::getCompatibleInterfaceMode(busInf->getInterfaceMode());
                        std::string modeString = General::interfaceMode2Str(newMode).toStdString();

                        createBusInterface(busName.toUpper().toStdString(), modeString, index);

                        createPortMaps(modeString, busInf);

                        createPhysPorts(comp, busName);

                        //connectionInterface_->addInterconnection(compName, busName.toStdString(), interconComponent_->getVlnv().getName().toStdString(), newBusName);

                        index++;
                        break;
                    }
                }
            }
        }
    }
    createRstorClkInterface("rst", index);
    createRstorClkInterface("clk", index + 1);
}

void InterconnectGenerator::processStartingPointsAndEndpoints(
    const QHash<QString, QList<QSharedPointer<BusInterface>>>& startingPoints,
    const QHash<QString, QList<QSharedPointer<EndpointData>>>& endpoints)
{
    messager_->showMessage("Processing starting points and endpoints...");
    int index = 0;

    if (!config_->isChannel) {
        createGlobalAddressSpaceFromEndpoints(endpoints);
    }

    std::vector<BusInterfaceInfo> createdBuses;

    auto busesFromEndpoints = processEndpointSide(endpoints, index);
    auto busesFromStarting = processStartingSide(startingPoints, index);

    createdBuses.insert(createdBuses.end(), busesFromEndpoints.begin(), busesFromEndpoints.end());
    createdBuses.insert(createdBuses.end(), busesFromStarting.begin(), busesFromStarting.end());

    finalizeBusInterfaceCustomization(createdBuses);

    if (!config_->RstVLNV.isEmpty()) createRstorClkInterface("rst", index++);
    if (!config_->ClkVLNV.isEmpty()) createRstorClkInterface("clk", index++);
}

std::vector<BusInterfaceInfo> InterconnectGenerator::processEndpointSide(
    const QHash<QString, QList<QSharedPointer<EndpointData>>>& endpoints, int& index)
{
    std::vector<BusInterfaceInfo> results;
    VLNV designVLNV(VLNV::COMPONENT, config_->DesignVLNV);

    for (auto it = endpoints.constBegin(); it != endpoints.constEnd(); ++it) {
        QString instanceName = it.key();
        bool isTop = (instanceName == designVLNV.getName());

        auto compVLNV = resolveComponentVLNV(instanceName, isTop);
        if (!compVLNV) continue;

        QSharedPointer<Document> compDocument = library_->getModel(*compVLNV.dynamicCast<VLNV>());
        QSharedPointer<Component> comp = compDocument.dynamicCast<Component>();

        for (const QSharedPointer<EndpointData>& data : it.value()) {
            QSharedPointer<BusInterface> bus = data->endpointBus;

            BusInterfaceInfo info = createInterfaceForBus(instanceName, bus, isTop, true, index);
            info.start = data->start;
            info.range = data->range;
            results.push_back(info);

            createPortMaps(info.mode, bus);
            createPhysPorts(comp, bus->name(), isTop);

            index++;
        }
    }

    return results;
}

std::vector<BusInterfaceInfo> InterconnectGenerator::processStartingSide(
    const QHash<QString, QList<QSharedPointer<BusInterface>>>& startingPoints, int& index)
{
    std::vector<BusInterfaceInfo> results;
    VLNV designVLNV(VLNV::COMPONENT, config_->DesignVLNV);

    for (auto it = startingPoints.constBegin(); it != startingPoints.constEnd(); ++it) {
        QString instanceName = it.key();
        bool isTop = (instanceName == designVLNV.getName());

        auto compVLNV = resolveComponentVLNV(instanceName, isTop);
        if (!compVLNV) continue;

        QSharedPointer<Document> compDocument = library_->getModel(*compVLNV.dynamicCast<VLNV>());
        QSharedPointer<Component> comp = compDocument.dynamicCast<Component>();

        for (const QSharedPointer<BusInterface>& bus : it.value()) {
            BusInterfaceInfo info = createInterfaceForBus(instanceName, bus, isTop, false, index);
            results.push_back(info);
            
            createPortMaps(info.mode, bus);
            createPhysPorts(comp, bus->name(), isTop);

            index++;
        }
    }
    return results;
}

BusInterfaceInfo InterconnectGenerator::createInterfaceForBus(
    const QString& instanceName, const QSharedPointer<BusInterface>& bus,
    bool isTop, bool isEndpoint, int& index)
{
    QString busName = bus->name();
    VLNV busVLNV = bus->getBusType();
    prefix_ = instanceName.toStdString() + "_" + busName.toStdString() + "_";

    bool isTarget = isTargetInterface(bus);
    std::string mode = getInterfaceMode(bus, isTarget, config_->isChannel, isTop);
    std::string newName = createBusInterface(busVLNV, busName.toUpper().toStdString(), mode, index);
    
    if (!globalAddressSpaceName_.empty() && isEndpoint && isTarget) 
    {
        busInfInterface_->setAddressSpaceReference(newName, globalAddressSpaceName_);
    }

    if (isTop) 
    {
        connectionInterface_->addHierarchicalInterconnection(
            interconComponent_->getVlnv().getName().toStdString(), newName, busName.toStdString());
    }
    else 
    {
        connectionInterface_->addInterconnection(
            instanceName.toStdString(), busName.toStdString(),
            interconComponent_->getVlnv().getName().toStdString(), newName);
    }
    
    return { newName, mode };
}

QSharedPointer<ConfigurableVLNVReference> InterconnectGenerator::resolveComponentVLNV(
    const QString& instanceName, bool isTop) const
{
    VLNV designVLNV(VLNV::COMPONENT, config_->DesignVLNV);

    auto compVLNV = instanceInterface_->getComponentReference(instanceName.toStdString());

    if (!compVLNV && isTop) {
        return QSharedPointer<ConfigurableVLNVReference>::create(designVLNV);
    }

    return compVLNV;
}

bool InterconnectGenerator::isTargetInterface(const QSharedPointer<BusInterface>& bus) const
{
    auto mode = bus->getInterfaceMode();

    return mode == General::TARGET ||
        mode == General::MIRRORED_INITIATOR ||
        mode == General::SLAVE ||
        mode == General::MIRRORED_MASTER;
}

std::string InterconnectGenerator::getInterfaceMode(QSharedPointer<BusInterface> bus, bool isTarget, bool isChannel, bool isTop)
{
    if (isTop) 
    {
        return General::interfaceMode2Str(bus->getInterfaceMode()).toStdString();
    }
    Document::Revision rev = design_->getRevision();

    if (rev == Document::Revision::Std14)
    {
        if (!isTarget)
            return isChannel ? "mirroredMaster" : "slave";
        else
            return isChannel ? "mirroredSlave" : "master";
    }
    else if (rev == Document::Revision::Std22)
    {
        if (!isTarget)
            return isChannel ? "mirroredInitiator" : "target";
        else
            return isChannel ? "mirroredTarget" : "initiator";
    }
    return !isTarget ? "target" : "initiator";
}

void InterconnectGenerator::createBusInterface(std::string busName, std::string modeString, int index)
{
    VLNV busDef = busDefVLNV_;
    std::string newBusName = prefix_ + config_->BusType.toStdString();

    if (busName == "rst")
    {
        busDef = rstVLNV_;
        QString name = rstVLNV_.getName().split(".abs")[0];
        busDef.setName(name);
        newBusName = prefix_ + name.toStdString();
        rstName_ = newBusName;
    }
    else if (busName == "clk")
    {
        busDef = clkVLNV_;
        QString name = clkVLNV_.getName().split(".abs")[0];
        busDef.setName(name);
        newBusName = prefix_ + name.toStdString();
        clockName_ = newBusName;
    }

    messager_->showMessage(QString("Creating %1 interface").arg(QString::fromStdString(newBusName)));
    busInfInterface_->addBusInterface(index, newBusName);
    busInfInterface_->setMode(newBusName, modeString);

    busInfInterface_->setBustype(newBusName, busDef.getVendor().toStdString(), busDef.getLibrary().toStdString(),
        busDef.getName().toStdString(), busDef.getVersion().toStdString());

    busInfInterface_->addAbstractionType(newBusName, busDef.getVendor().toStdString(), busDef.getLibrary().toStdString(),
        busDef.getName().toStdString() + ".absDef", busDef.getVersion().toStdString());

    messager_->showMessage(QString("%1 interface created").arg(QString::fromStdString(newBusName)));
}

std::string InterconnectGenerator::createBusInterface(VLNV busVLNV, std::string busName,
    std::string modeString, int index
)
{
    VLNV busDef = busDefVLNV_;

    std::string newBusName = prefix_;
    newBusName.pop_back();

    busInfInterface_->addBusInterface(index, newBusName);
    busInfInterface_->setMode(newBusName, modeString);

    busInfInterface_->setBustype(newBusName, busVLNV.getVendor().toStdString(), busVLNV.getLibrary().toStdString(),
        busVLNV.getName().toStdString(), busVLNV.getVersion().toStdString());

    busInfInterface_->addAbstractionType(newBusName, busDef.getVendor().toStdString(), busDef.getLibrary().toStdString(),
        busDef.getName().toStdString(), busDef.getVersion().toStdString());

    messager_->showMessage(QString("%1 interface created").arg(QString::fromStdString(newBusName)));

    return newBusName;
}

void InterconnectGenerator::finalizeBusInterfaceCustomization(const std::vector<BusInterfaceInfo>& createdBuses)
{
    for (const auto& busInfo : createdBuses) {
        const std::string& name = busInfo.name;
        const std::string& mode = busInfo.mode;

        if (mode == "slave" || mode == "target") {
            busInfInterface_->setupSubInterfaces(name);
            TransparentBridgeInterface* bridgeInterface = busInfInterface_->getBridgeInterface();
            if (!bridgeInterface) {
                continue;
            }

            int index = bridgeInterface->itemCount();

            for (const std::string& candidate : busInfInterface_->getItemNames()) {
                std::string candidateMode = busInfInterface_->getModeString(candidate);

                if (candidateMode != "master" && candidateMode != "initiator") {
                    continue;
                }
                bridgeInterface->addBridge(index, candidate);
                index++;
            }
        }
        else if (mode == "master" || mode == "initiator") {
            if (!busInfo.start.isEmpty()) {
                busInfInterface_->setBaseAddress(name, busInfo.start.toStdString());
            }
        }
        else if (mode == "mirroredSlave" || mode == "mirroredTarget") {
            if (!busInfo.start.isEmpty()) {
                busInfInterface_->setRemapAddress(name, busInfo.start.toStdString());
            }
            if (!busInfo.range.isEmpty()) {
                busInfInterface_->setRange(name, busInfo.range.toStdString());
            }
        }
    }
}

void InterconnectGenerator::createPortMaps(std::string modeString, QSharedPointer<BusInterface> busInf)
{
    messager_->showMessage("Creating port maps");

    if (absTypeInf_->setupAbstractionTypeForPortMapInterface(0)) {
        PortMapInterface* portMapInf = absTypeInf_->getPortMapInterface();
        std::vector<std::string> logicalPortNames = portMapInf->
            getLogicalPortInterface()->getItemNamesWithModeAndGroup(modeString, "");
        QList<QSharedPointer<PortMap> > portMaps = busInf->getPortMapsForView("");
        for (int i = 0; i < portMaps.size(); i++)
        {
            QSharedPointer<PortMap> portMap = portMaps.at(i);
            if (!portMap || !portMap->getLogicalPort() || !portMap->getPhysicalPort())
            {
                continue;
            }

            for (std::string logicalName : logicalPortNames)
            {
                std::string portMapName = portMap->getLogicalPort()->name_.toStdString();
                    
                if (logicalName == portMapName)
                {
                    std::string newName = prefix_ + portMap->getPhysicalPort()->name_.toStdString();

                    QSharedPointer<PartSelect> partSelect = portMap->getPhysicalPort()->partSelect_;
                    QSharedPointer<Range> range = portMap->getLogicalPort()->range_;
                    int portMapIndex = portMapInf->itemCount();
                        
                    portMapInf->addPortMap(portMapIndex);

                    portMapInf->setPhysicalPort(portMapIndex, newName);

                    if (partSelect != nullptr)
                    {
                        portMapInf->setPhysicalLeftBound(portMapIndex, partSelect->getLeftRange().toStdString());
                        portMapInf->setPhysicalRightBound(portMapIndex, partSelect->getRightRange().toStdString());
                    }

                    portMapInf->setLogicalPort(portMapIndex, logicalName);

                    if (range != nullptr)
                    {
                        portMapInf->setLogicalLeftBound(portMapIndex, range->getLeft().toStdString());
                        portMapInf->setLogicalRightBound(portMapIndex, range->getRight().toStdString());
                    }
                        
                    break;
                }
                }
        }
    }
    messager_->showMessage("All port maps created");
}

void InterconnectGenerator::createPhysPorts(QSharedPointer<Component> comp, QString busName, bool isTop)
{
    messager_->showMessage("Populating model with ports");

    parameterFinder_->setComponent(comp);
    portValidator_->componentChange(comp->getViews());
    portsInterface_->setPorts(comp->getPorts());

    QSharedPointer<Document> libComp = library_->getModel(busDefVLNV_);
    QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();
    portAbsInterface_->setAbsDef(absDef);

    for (QSharedPointer<Port> port : comp->getPortsMappedInInterface(busName))
    {
        QString physicalName = port->name();
        std::string logicalName = getLogicalPortName(comp, busName, physicalName);

        QString leftBound, rightBound;

        if (!logicalName.empty())
        {
            std::tie(leftBound, rightBound) = getWidthBoundsFromAbstraction(logicalName);
        }

        if (leftBound.isEmpty() || rightBound.isEmpty())
        {
            std::tie(leftBound, rightBound) = getMirroredWidthBounds(physicalName);
        }

        QSharedPointer<Port> newPort(new Port(*port));
        newPort->setName(QString::fromStdString(prefix_) + newPort->name());

        DirectionTypes::Direction newDir;
        if (isTop) 
        {
            newDir = newPort->getDirection();
        }
        else 
        {
            newDir = DirectionTypes::convert2Mirrored(newPort->getDirection());
        }
        newPort->setDirection(newDir);
        newPort->setLeftBound(leftBound);
        newPort->setRightBound(rightBound);

        interconComponent_->getPorts()->append(newPort);
    }

    parameterFinder_->setComponent(interconComponent_);
}

std::string InterconnectGenerator::getLogicalPortName(
    QSharedPointer<Component> comp, QString busName, QString physicalName) const
{
    auto portMaps = comp->getBusInterface(busName)->getPortMapsForView("");
    for (const auto& map : portMaps)
    {
        if (map->getPhysicalPort() && map->getPhysicalPort()->name_ == physicalName)
        {
            return map->getLogicalPort()->name_.toStdString();
        }
    }
    return "";
}

std::pair<QString, QString> InterconnectGenerator::getWidthBoundsFromAbstraction(
    const std::string& logicalName) const
{
    int index = portAbsInterface_->getItemIndex(logicalName);
    if (index == -1) {
        return { "", "" };
    }

    std::string absWidth = portAbsInterface_->getWidthExpression(index);
    if (absWidth.empty()) {
        return { "", "" };
    }

    QString widthExpr = QString::fromStdString(absWidth).trimmed();
    QString left = widthExpr == "1" ? "0" : QString("%1 - 1").arg(widthExpr);
    QString right = "0";

    return { left, right };
}

std::pair<QString, QString> InterconnectGenerator::getMirroredWidthBounds(const QString& physicalName) const
{
    std::string left = portsInterface_->getLeftBoundValue(physicalName.toStdString());
    std::string right = portsInterface_->getRightBoundValue(physicalName.toStdString());

    QString leftBound = left.empty() ? "0" : QString::fromStdString(left);
    QString rightBound = right.empty() ? "0" : QString::fromStdString(right);

    return { leftBound, rightBound };
}


void InterconnectGenerator::createRstorClkInterface(std::string busName, int index)
{
    General::InterfaceMode mode = General::TARGET;

    if (interconComponent_->getRevision() == Document::Revision::Std14)
    {
        mode = General::SLAVE;
    }

    std::string modeString = General::interfaceMode2Str(mode).toStdString();

    prefix_ = busName + "_";

    createBusInterface(busName, modeString, index);

    absTypeInf_->setupAbstractionTypeForPortMapInterface(0);

    PortMapInterface* portMapInf = absTypeInf_->getPortMapInterface();
    PortsInterface* portInf = portMapInf->getPhysicalPortInterface();
    PortAbstractionInterface* portAbsInf = portMapInf->getLogicalPortInterface();

    portMapInf->setupPhysicalPorts(interconComponent_->getPorts());

    for (std::string portName : portAbsInf->getItemNamesWithModeAndGroup(modeString, ""))
    {
        if (busName == "clk") {
            clkPort_ = QString::fromStdString(portName);
        }
        else if (busName == "rst") {
            rstPort_ = QString::fromStdString(portName);
        }
        int portIndex = portAbsInf->getItemIndex(portName);

        if (!portInf->getPort(portName)) {
            portInf->addWirePort(portName);
        }

        portInf->setDirection(portName, DirectionTypes::direction2Str(
            portAbsInf->getDirection(portName, mode, "")).toStdString());

        uint leftBound = 0;
        bool signalIntegerOk = false;
        uint signalWidth = QString::fromStdString(
            portAbsInf->getWidthFormattedExpression(portIndex)).toUInt(&signalIntegerOk);

        if (signalIntegerOk == true && signalWidth > 0)
        {
            leftBound = signalWidth - 1;
        }

        portInf->setLeftBound(portName, QString::number(leftBound).toStdString());
        portInf->setRightBound(portName, "0");

        portInf->setDefaultValue(portName, portAbsInf->getDefaultValueExpression(portIndex));
        portInf->setDescription(portName, portAbsInf->getDescription(portName));

        int mapIndex = portMapInf->itemCount();

        portMapInf->addPortMap(mapIndex);
        portMapInf->setLogicalPort(mapIndex, portName);

        portMapInf->connectPorts(portName, portName);
    }
}

void InterconnectGenerator::createChannel()
{
    std::vector<std::string> busNames = busInfInterface_->getItemNames();
    QList<QString> interfaceList;

    for (const std::string& name : busNames) {
        if (name == clockName_ || name == rstName_)
        {
            continue;
        }
        interfaceList.append(QString::fromStdString(name));
    }

    QSharedPointer<Channel> channel(new Channel());
    channel->setInterfaces(interfaceList);
    channel->setName("channel");

    QSharedPointer<QList<QSharedPointer<Channel>>> channelList = QSharedPointer<
        QList<QSharedPointer<Channel>>>::create();
    channelList->append(channel);

    interconComponent_->setChannels(channelList);
}

void InterconnectGenerator::createGlobalAddressSpaceFromEndpoints(
    const QHash<QString, QList<QSharedPointer<EndpointData>>>& endpoints)
{
    quint64 lowestStart = UINT64_MAX;
    quint64 highestEnd = 0;

    for (const auto& endpointList : endpoints) {
        for (const QSharedPointer<EndpointData>& endpointData : endpointList) {
            bool okStart = false, okRange = false;

            quint64 start = parseIpxactHex(endpointData->start, &okStart);
            quint64 range = parseIpxactHex(endpointData->range, &okRange);

            if (okStart && okRange) {
                lowestStart = std::min(lowestStart, start);
                highestEnd = std::max(highestEnd, start + range);
            }
        }
    }

    if (lowestStart == UINT64_MAX || highestEnd == 0) {
        return;
    }

    quint64 addrSpaceSize = (highestEnd > lowestStart) ? (highestEnd - lowestStart) : 0;

    QString hexRange = QString::number(addrSpaceSize, 16).rightJustified(4, '0').toUpper();
    QString ipxactRange = "'h" + hexRange;

    QString addrWidth = QString::number(config_->AddressWidth);
    globalAddressSpaceName_ = config_->BusType.toStdString() + "_global_space";
    createAddressSpace(globalAddressSpaceName_, ipxactRange, addrWidth);
}

quint64 InterconnectGenerator::parseIpxactHex(const QString& str, bool* ok)
{
    QString clean = str.trimmed().toUpper();

    if (clean.startsWith("'H")) {
        clean.remove(0, 2);
    }
    else if (clean.startsWith("0X")) {
        clean.remove(0, 2);
    }

    return clean.toULongLong(ok, 16);
}

void InterconnectGenerator::createAddressSpace(std::string spaceName, QString range, QString width)
{
    QSharedPointer<AddressSpace> addrSpace = QSharedPointer<AddressSpace>(
        new AddressSpace(QString::fromStdString(spaceName), range, width));

    interconComponent_->getAddressSpaces()->append(addrSpace);
}
