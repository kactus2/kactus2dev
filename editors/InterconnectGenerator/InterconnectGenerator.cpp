#include "InterconnectGenerator.h"
#include <KactusAPI/KactusAPI.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>

#include <KactusAPI/include/BusInterfaceInterfaceFactory.h>
#include <KactusAPI/include/PortMapInterface.h>
#include <KactusAPI/include/PortAbstractionInterface.h>

InterconnectGenerator::InterconnectGenerator(LibraryInterface* library,  MessageMediator* messager)
{
    library_ = library;
    messager_ = messager;
}

VLNV InterconnectGenerator::generate(std::string sDesignVLNV, std::string sInterconVLNV)
{
    VLNV designVLNV(VLNV::DESIGN, QString::fromStdString(sDesignVLNV));
    VLNV interconVLNV(VLNV::COMPONENT, QString::fromStdString(sInterconVLNV));
    openDesign(designVLNV);
    createInterconComponent(interconVLNV);
    findUnconnectedInterface();

    return interconVLNV;
}

void InterconnectGenerator::openDesign(VLNV designVLNV)
{
    QSharedPointer<Document> designDocument = library_->getModel(designVLNV);
    design_ = designDocument.dynamicCast<Design>();

    instanceInterface_->setComponentInstances(design_);
    connectionInterface_->setInterconnections(design_);
    adhocConnectionInterface_->setConnections(design_);
    messager_->showMessage("Design opened and connected");
}

void InterconnectGenerator::createInterconComponent(VLNV VLNV)
{
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

void InterconnectGenerator::findUnconnectedInterface()
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
                messager_->showMessage(QString("Bus name %1").arg(busName));
                QSharedPointer<BusInterface> busInf = comp->getBusInterface(busName);
                VLNV busVLNV = busInf->getBusType();
                if(busVLNV == busDefVLNV_)
                {
                    if(!design_->hasInterconnection(QString::fromStdString(compName), busName))
                    {
                        messager_->showMessage("Unconnected interface found");
                        prefix_ = compName + "_";
                        messager_->showMessage(QString("Original interface mode name %1").arg(General::interfaceMode2Str(busInf->getInterfaceMode())));
                        General::InterfaceMode newMode = General::getCompatibleInterfaceMode(busInf->getInterfaceMode());
                        std::string modeString = General::interfaceMode2Str(newMode).toStdString();
                        messager_->showMessage(QString("Interface mode name %1").arg(QString::fromStdString(modeString)));

                        std::string newBusName = prefix_ + busName.toStdString();

                        createBusInterface(busName.toStdString(), modeString, index);

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

    QString directory = KactusAPI::getDefaultLibraryPath();
    QString vlnvDir = "/" + interconComponent_->getVlnv().getVendor() + "/" + interconComponent_->getVlnv().getLibrary() + "/" +
        interconComponent_->getVlnv().getName() + "/" + interconComponent_->getVlnv().getVersion();

    directory += vlnvDir;
    messager_->showMessage(QString("Writing component %1 to file").arg(interconComponent_->getVlnv().toString()));
    library_->writeModelToFile(directory,interconComponent_);
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

    std::string newBusName = prefix_ + busName;

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
    messager_->showMessage("Setting up port map inf");

    if(absTypeInf_->setupAbstractionTypeForPortMapInterface(0)) {
        PortMapInterface* portMapInf = absTypeInf_->getPortMapInterface();

        std::vector<std::string> logicalPortNames = portMapInf->getLogicalPortInterface()->getItemNamesWithModeAndGroup(modeString, "");
        messager_->showMessage(QString("Number of logical ports from inf %1").arg(logicalPortNames.size()));
        QList<QSharedPointer<PortMap> > portMaps = busInf->getPortMapsForView("");

        for(int index=0; index < portMaps.size(); index++)
        {
            messager_->showMessage(QString("Port map index %1").arg(index));

            for(std::string logicalName : logicalPortNames)
            {
                messager_->showMessage(QString("Logical port name %1").arg(QString::fromStdString(logicalName)));
                std::string portMapName = portMaps.at(index)->getLogicalPort()->name_.toStdString();
                messager_->showMessage(QString("Logical port name from port map %1").arg(QString::fromStdString(portMapName)));

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
         messager_->showMessage(QString("Parameter ID %1 found for comp").arg(portID));
    }

    for(QSharedPointer<Port> port : comp->getPortsMappedInInterface(busName))
    {
        std::string leftBound = portsInterface_->getLeftBoundValue(port->name().toStdString());

        messager_->showMessage(QString("Port left bound %1").arg(QString::fromStdString(leftBound)));

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

    createBusInterface(busName, modeString, index);

    absTypeInf_->setupAbstractionTypeForPortMapInterface(0);
    PortMapInterface* portMapInf = absTypeInf_->getPortMapInterface();
    PortsInterface* portInf = portMapInf->getPhysicalPortInterface();
    PortAbstractionInterface* portAbsInf = portMapInf->getLogicalPortInterface();

    portMapInf->setupPhysicalPorts(interconComponent_->getPorts());

    messager_->showMessage(QString("Interface mode name %1").arg(General::interfaceMode2Str(portMapInf->getInterfaceMode())));

    for (std::string portName : portAbsInf->getItemNamesWithModeAndGroup(modeString,""))
    {
        messager_->showMessage(QString("Port name %1").arg(QString::fromStdString(portName)));
        int portIndex = portAbsInf->getItemIndex(portName);

        portInf->addWirePort(portName);

        portInf->setDirection(portName, DirectionTypes::direction2Str(portAbsInf->getDirection(portName,mode,"")).toStdString());

        uint leftBound = 0;
        bool signalIntegerOk = false;
        uint signalWidth = QString::fromStdString(portAbsInf->getWidth(portIndex)).toUInt(&signalIntegerOk);

        if (signalIntegerOk == true && signalWidth > 0)
        {
            leftBound = signalWidth - 1;
        }

        portInf->setLeftBound(portName, QString::number(leftBound).toStdString());
        portInf->setRightBound(portName, "0");

        portInf->setDefaultValue(portName, portAbsInf->getDefaultValue(portIndex));
        portInf->setDescription(portName, portAbsInf->getDescription(portName));

        portMapInf->addPortMap(portIndex);
        portMapInf->setLogicalPort(portIndex, portName);

        portMapInf->connectPorts(portName, portName);

    }
}

