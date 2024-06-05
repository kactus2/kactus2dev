#include "InterconnectGenerator.h"
#include "InterconnectRTLWriter.h"
#include <KactusAPI/KactusAPI.h>

#include <KactusAPI/include/BusInterfaceInterfaceFactory.h>
#include <KactusAPI/include/PortMapInterface.h>
#include <KactusAPI/include/PortAbstractionInterface.h>

InterconnectGenerator::InterconnectGenerator(LibraryInterface* library,  MessageMediator* messager)
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
    library_->writeModelToFile(directory_,interconComponent_);

    InterconnectRTLWriter writer(interconComponent_, library_, messager_, directory_,
                                 config_, clkPort_, rstPort_);
    writer.generateRTL();
    return interconVLNV;
}

VLNV InterconnectGenerator::generate(ConfigStruct* config, const QHash<QString, QList<QSharedPointer<BusInterface>>>& initiators,
    const QHash<QString, QList<QSharedPointer<BusInterface>>>& targets)
{
    config_ = config;

    VLNV designVLNV(VLNV::COMPONENT, config_->DesignVLNV);
    VLNV interconVLNV(VLNV::COMPONENT, config_->InterconVLNV);

    busDefVLNV_ = VLNV(VLNV::BUSDEFINITION, config_->BusVLNV);
    rstVLNV_ = VLNV(VLNV::BUSDEFINITION, config_->RstVLNV);
    clkVLNV_ = VLNV(VLNV::BUSDEFINITION, config_->ClkVLNV);
    openDesign(designVLNV);
    createInterconComponent(interconVLNV);
    processInitiatorsAndTargets(initiators, targets);

    directory_ = KactusAPI::getDefaultLibraryPath();
    QString vlnvDir = "/" + interconComponent_->getVlnv().getVendor() + "/" + interconComponent_->getVlnv().getLibrary() + "/" +
        interconComponent_->getVlnv().getName() + "/" + interconComponent_->getVlnv().getVersion();

    directory_ += vlnvDir;
    messager_->showMessage(QString("Writing component %1 to file").arg(interconComponent_->getVlnv().toString()));
    library_->writeModelToFile(directory_, interconComponent_);

    InterconnectRTLWriter writer(interconComponent_, library_, messager_, directory_, config_);
    writer.generateRTL();
    return interconVLNV;
}

void InterconnectGenerator::openDesign(VLNV designVLNV)
{
    QSharedPointer<Document> designCompDocument = library_->getModel(designVLNV);
    
    QSharedPointer<Component> designComp = designCompDocument.dynamicCast<Component>();
    messager_->showMessage(QString("checkpoint"));
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
    if(library_->contains(VLNV)){
        library_->removeObject(VLNV);
    }
    messager_->showMessage("Creating new component");
    QSharedPointer<Component> component = QSharedPointer<Component>(new Component(VLNV, design_->getRevision()));

    component->setHierarchy(KactusAttribute::FLAT);
    component->setFirmness(KactusAttribute::MUTABLE);
    component->setImplementation(KactusAttribute::HW);
    component->setVersion("1.0");
    interconComponent_ = component;

    parameterFinder_->setComponent(interconComponent_);

    busInfInterface_ = BusInterfaceInterfaceFactory::createBusInterface(parameterFinder_,
            expressionFormatter_, expressionParser_, interconComponent_, library_);

    busInfInterface_->setBusInterfaces(interconComponent_);
    absTypeInf_ = busInfInterface_->getAbstractionTypeInterface();

    //instanceInterface_->addComponentInstance("interconnect");
    //instanceInterface_->setComponentReference("interconnect", VLNV.getVendor().toStdString(), VLNV.getLibrary().toStdString(),
                                              //VLNV.getName().toStdString(), VLNV.getVersion().toStdString());

    messager_->showMessage("Component created and linked");
}

void InterconnectGenerator::findUnconnectedInterfaces()
{
    messager_->showMessage("Seaching for interfaces..");
    int index = 0;
    std::vector<std::string> compNames = instanceInterface_->getItemNames();

    for(std::string compName : compNames)
    {
        messager_->showMessage(QString("Comp name %1").arg(QString::fromStdString(compName)));
        if(compName != "interconnect") {
            auto compVLNV = instanceInterface_->getComponentReference(compName);                                                                                                                                                               
            QSharedPointer<Document> compDocument = library_->getModel(*compVLNV.dynamicCast<VLNV>());
            QSharedPointer<Component> comp = compDocument.dynamicCast<Component>();
            QStringList busNames = comp->getBusInterfaceNames();
            for(QString busName : busNames)
            {
                QSharedPointer<BusInterface> busInf = comp->getBusInterface(busName);
                VLNV busVLNV = busInf->getBusType();
                if(busVLNV == busDefVLNV_)
                {
                    if(!design_->hasInterconnection(QString::fromStdString(compName), busName))
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
    createRstorClkInterface("clk", index+1);
}

void InterconnectGenerator::processInitiatorsAndTargets(
    const QHash<QString, QList<QSharedPointer<BusInterface>>>& initiators,
    const QHash<QString, QList<QSharedPointer<BusInterface>>>& targets)
{
    messager_->showMessage("Processing initiators and targets...");
    int index = 0;

    // Process Initiators
    for (auto it = initiators.constBegin(); it != initiators.constEnd(); ++it) {
        QString instanceName = it.key();
        messager_->showMessage(QString("Instance name: %1").arg(instanceName));
        auto compVLNV = instanceInterface_->getComponentReference(instanceName.toStdString());
        QSharedPointer<Document> compDocument = library_->getModel(*compVLNV.dynamicCast<VLNV>());
        QSharedPointer<Component> comp = compDocument.dynamicCast<Component>();
        const QList<QSharedPointer<BusInterface>>& busInterfaces = it.value();
        
        for (const QSharedPointer<BusInterface>& busInterface : busInterfaces) {
            QString busName = busInterface->name();
            VLNV busVLNV = busInterface->getBusType();

            messager_->showMessage(
                QString("Initiator Component: %1, Interface: %2").arg(instanceName, busName));

            messager_->showMessage("Unconnected initiator found");
            prefix_ = instanceName.toStdString() + "_";
            General::InterfaceMode newMode = General::getCompatibleInterfaceMode(busInterface->getInterfaceMode());
            std::string modeString = General::interfaceMode2Str(newMode).toStdString();

            createBusInterface(busName.toUpper().toStdString(), modeString, index);
            createPortMaps(modeString, busInterface);
            createPhysPorts(comp, busName);

            index++;
        }
    }
    /*
    // Process Targets
    for (auto it = targets.constBegin(); it != targets.constEnd(); ++it) {
        QString instanceName = it.key();
        messager_->showMessage(QString("Target name: %1").arg(instanceName));
        auto compVLNV = instanceInterface_->getComponentReference(instanceName.toStdString());                                                                                                                                                                 
        QSharedPointer<Document> compDocument = library_->getModel(*compVLNV.dynamicCast<VLNV>());
        QSharedPointer<Component> comp = compDocument.dynamicCast<Component>();

        const QList<QSharedPointer<BusInterface>>& busInterfaces = it.value();

        for (const QSharedPointer<BusInterface>& busInterface : busInterfaces) {
            QString busName = busInterface->name();
            VLNV busVLNV = busInterface->getBusType();

            messager_->showMessage(
                QString("Target Component: %1, Interface: %2").arg(instanceName, busName));

            if (busVLNV == busDefVLNV_ && !design_->hasInterconnection(instanceName, busName)) {
                messager_->showMessage("Unconnected target found");

                // Create necessary interfaces, ports, and interconnections
                prefix_ = instanceName.toStdString() + "_";
                General::InterfaceMode newMode = General::getCompatibleInterfaceMode(busInterface->getInterfaceMode());
                std::string modeString = General::interfaceMode2Str(newMode).toStdString();

                createBusInterface(busName.toUpper().toStdString(), modeString, index);
                createPortMaps(modeString, busInterface);
                createPhysPorts(comp, busName);

                index++;
            }
        }
    }
    */
    // Finalize with reset and clock interfaces
    createRstorClkInterface("rst", index);
    createRstorClkInterface("clk", index + 1);
}

void InterconnectGenerator::createBusInterface(std::string busName, std::string modeString, int index)
{
    VLNV busDef = busDefVLNV_;

    if(busName == "rst")
    {
        busDef = rstVLNV_;
    }
    else if(busName == "clk")
    {
        busDef = clkVLNV_;
    }

    std::string newBusName = prefix_ + config_->BusType.toStdString();

    messager_->showMessage(QString("Creating %1 interface").arg(QString::fromStdString(newBusName)));
    busInfInterface_->addBusInterface(index, newBusName);
    busInfInterface_->setMode(newBusName, modeString);

    busInfInterface_->setBustype(newBusName,busDef.getVendor().toStdString(), busDef.getLibrary().toStdString(),
                                 busDef.getName().toStdString(), busDef.getVersion().toStdString());

    busInfInterface_->addAbstractionType(newBusName, busDef.getVendor().toStdString(), busDef.getLibrary().toStdString(),
                                         busDef.getName().toStdString()+".absDef", busDef.getVersion().toStdString());

    messager_->showMessage(QString("%1 interface created").arg(QString::fromStdString(newBusName)));
}

void InterconnectGenerator::createPortMaps(std::string modeString, QSharedPointer<BusInterface> busInf)
{
    messager_->showMessage("Creating port maps");

    if(absTypeInf_->setupAbstractionTypeForPortMapInterface(0)) {
        PortMapInterface* portMapInf = absTypeInf_->getPortMapInterface();

        std::vector<std::string> logicalPortNames = portMapInf->getLogicalPortInterface()->getItemNamesWithModeAndGroup(modeString, "");
        QList<QSharedPointer<PortMap> > portMaps = busInf->getPortMapsForView("");

        for(int index=0; index < portMaps.size(); index++)
        {

            for(std::string logicalName : logicalPortNames)
            {
                std::string portMapName = portMaps.at(index)->getLogicalPort()->name_.toStdString();

                if(logicalName == portMapName)
                {
                    QSharedPointer<PortMap> portMap = portMaps.at(index);
                    std::string newName = prefix_ + portMap->getPhysicalPort()->name_.toStdString();

                    QSharedPointer<PartSelect> partSelect = portMap->getPhysicalPort()->partSelect_;
                    QSharedPointer<Range> range = portMap->getLogicalPort()->range_;

                    portMapInf->addPortMap(index);

                    portMapInf->setPhysicalPort(index, newName);

                    if(partSelect != nullptr)
                    {
                        portMapInf->setPhysicalLeftBound(index, partSelect->getLeftRange().toStdString());
                        portMapInf->setPhysicalRightBound(index, partSelect->getRightRange().toStdString());
                    }

                    portMapInf->setLogicalPort(index, logicalName);

                    if(range != nullptr)
                    {
                        portMapInf->setLogicalLeftBound(index, range->getLeft().toStdString());
                        portMapInf->setLogicalRightBound(index, range->getRight().toStdString());
                    }
                    break;
                }
            }
        }
    }
    messager_->showMessage("All port maps created");

}

void InterconnectGenerator::createPhysPorts(QSharedPointer<Component> comp, QString busName)
{
    messager_->showMessage("Populating model with ports");

    parameterFinder_->setComponent(comp);
    portValidator_->componentChange(comp->getViews());
    portsInterface_->setPorts(comp->getPorts());

    for(QString portID : parameterFinder_->getAllParameterIds()){
    }

    for(QSharedPointer<Port> port : comp->getPortsMappedInInterface(busName))
    {
        std::string leftBound = portsInterface_->getLeftBoundValue(port->name().toStdString());

        QSharedPointer<Port> newPort( new Port(*port));

        QString newName = QString::fromStdString(prefix_) + newPort->name();
        newPort->setName(newName);

        DirectionTypes::Direction newDir = DirectionTypes::convert2Mirrored(newPort->getDirection());
        newPort->setDirection(newDir);
        if(leftBound != ""){
        newPort->setLeftBound(QString::fromStdString(leftBound));
        }
        interconComponent_->getPorts()->append(newPort);
    }
    parameterFinder_->setComponent(interconComponent_);
}

void InterconnectGenerator::createRstorClkInterface(std::string busName, int index)
{
    General::InterfaceMode mode = General::TARGET;

    if(interconComponent_->getRevision() == Document::Revision::Std14)
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

    for (std::string portName : portAbsInf->getItemNamesWithModeAndGroup(modeString,""))
    {
        if(busName == "clk") {
            clkPort_ = QString::fromStdString(portName);
        } else if(busName == "rst") {
            rstPort_ = QString::fromStdString(portName);
        }
        int portIndex = portAbsInf->getItemIndex(portName);

        portInf->addWirePort(portName);

        portInf->setDirection(portName, DirectionTypes::direction2Str(portAbsInf->getDirection(portName,mode,"")).toStdString());

        uint leftBound = 0;
        bool signalIntegerOk = false;
        uint signalWidth = QString::fromStdString(portAbsInf->getWidthFormattedExpression(portIndex)).toUInt(&signalIntegerOk);

        if (signalIntegerOk == true && signalWidth > 0)
        {
            leftBound = signalWidth - 1;
        }

        portInf->setLeftBound(portName, QString::number(leftBound).toStdString());
        portInf->setRightBound(portName, "0");

        portInf->setDefaultValue(portName, portAbsInf->getDefaultValueExpression(portIndex));
        portInf->setDescription(portName, portAbsInf->getDescription(portName));

        portMapInf->addPortMap(portIndex);
        portMapInf->setLogicalPort(portIndex, portName);

        portMapInf->connectPorts(portName, portName);

    }
}
