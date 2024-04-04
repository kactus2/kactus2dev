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
    busInfInterface_ = BusInterfaceInterfaceFactory::createBusInterface(parameterFinder_,
            expressionFormatter_, expressionParser_,
            QSharedPointer<Component>(new Component(VLNV(), Document::Revision::Unknown)), library_);
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
    QSharedPointer<Component> component = QSharedPointer<Component>(new Component(VLNV, Document::Revision::Std14));

    component->setHierarchy(KactusAttribute::FLAT);
    component->setFirmness(KactusAttribute::MUTABLE);
    component->setImplementation(KactusAttribute::HW);
    component->setVersion("1.0");


    portValidator_->componentChange(component->getViews());
    portsInterface_->setPorts(component->getPorts());
    busInfInterface_->setBusInterfaces(component);
    absTypeInf_ = busInfInterface_->getAbstractionTypeInterface();
    interconComponent_ = component;

    instanceInterface_->addComponentInstance("interconnect");
    instanceInterface_->setComponentReference("interconnect", VLNV.getVendor().toStdString(), VLNV.getLibrary().toStdString(),
                                              VLNV.getName().toStdString(), VLNV.getVersion().toStdString());

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
                        prefix_ = "mirror_" + compName + "_";
                        General::InterfaceMode newMode = General::getCompatibleInterfaceMode(busInf->getInterfaceMode());
                        std::string modeString = General::interfaceMode2Str(newMode).toStdString();
                        std::string newBusName = prefix_ + busName.toStdString();

                        createBusInterface(newBusName, modeString, index);

                        createPortMaps(modeString, busInf);

                        createPhysPorts(comp, busName);

                        connectionInterface_->addInterconnection(compName, busName.toStdString(), interconComponent_->getVlnv().getName().toStdString(), newBusName);

                        index++;
                        break;
                    }
                }
            }
        }
    }
    QString directory = KactusAPI::getDefaultLibraryPath();
    QString vlnvDir = "/" + interconComponent_->getVlnv().getVendor() + "/" + interconComponent_->getVlnv().getLibrary() + "/" +
        interconComponent_->getVlnv().getName() + "/" + interconComponent_->getVlnv().getVersion();

    directory += vlnvDir;
    messager_->showMessage(QString("Writing component %1 to file").arg(interconComponent_->getVlnv().toString()));
    library_->writeModelToFile(directory,interconComponent_);
}

void InterconnectGenerator::createBusInterface(std::string busName, std::string modeString, int index)
{
    messager_->showMessage("Creating new interface");
    busInfInterface_->addBusInterface(index, busName);
    busInfInterface_->setMode(busName, modeString);

    busInfInterface_->setBustype(busName,busDefVLNV_.getVendor().toStdString(), busDefVLNV_.getLibrary().toStdString(),
                                 busDefVLNV_.getName().toStdString(), busDefVLNV_.getVersion().toStdString());

    busInfInterface_->addAbstractionType(busName, busDefVLNV_.getVendor().toStdString(), busDefVLNV_.getLibrary().toStdString(),
                                         busDefVLNV_.getName().toStdString()+".absDef", busDefVLNV_.getVersion().toStdString());
    messager_->showMessage("Interface created");
}

void InterconnectGenerator::createPortMaps(std::string modeString, QSharedPointer<BusInterface> busInf)
{
    messager_->showMessage(QString("Interface mode name %1").arg(QString::fromStdString(modeString)));
    messager_->showMessage("Creating port maps");
    messager_->showMessage("Setting up port map inf");
    if(absTypeInf_->setupAbstractionTypeForPortMapInterface(0)) {
        PortMapInterface* portMapInf = absTypeInf_->getPortMapInterface();

        std::vector<std::string> logicalPortNames = portMapInf->getLogicalPortInterface()->getItemNames();
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

    for(QSharedPointer<Port> port : comp->getPortsMappedInInterface(busName))
    {
        QSharedPointer<Port> newPort( new Port(*port));

        QString newName = QString::fromStdString(prefix_) + newPort->name();
        newPort->setName(newName);

        DirectionTypes::Direction newDir = DirectionTypes::convert2Mirrored(newPort->getDirection());
        newPort->setDirection(newDir);

        interconComponent_->getPorts()->append(newPort);
    }
}

