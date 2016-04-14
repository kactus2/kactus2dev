//-----------------------------------------------------------------------------
// File: BusInterfaceWizardBusDefinitionPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Bus interface wizard page for editing bus and abstraction definitions.
//-----------------------------------------------------------------------------

#include "BusInterfaceWizardBusDefinitionPage.h"

#include "BusInterfaceWizard.h"

#include <editors/BusEditor/absdefgroup.h>
#include <editors/BusEditor/busdefgroup.h>
#include <editors/ComponentEditor/ports/portsdelegate.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/common/Vector.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::BusInterfaceWizardBusDefinitionPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardBusEditorPage::BusInterfaceWizardBusEditorPage(QSharedPointer<Component> component,
    QSharedPointer<BusInterface> busIf, LibraryInterface* lh, QStringList physicalPorts,
    BusInterfaceWizard* parent, VLNV& absDefVLNV, QSharedPointer<ExpressionParser> expressionParser,
    SignalNamingPolicy namingPolicy /* = NAME */):
QWizardPage(parent),
    handler_(lh),
    component_(component),
    busIf_(busIf),
    absDefVLNV_(absDefVLNV),
    physicalPorts_(physicalPorts),
    editor_(this, handler_, QSharedPointer<BusDefinition>(0), QSharedPointer<AbstractionDefinition>(0), true),
    portNamesPolicy_(namingPolicy),
    portMappings_(),
    hasChanged_(false),
    mappingMode_(NO_GENERATION),
    expressionParser_(expressionParser)
{       
    setTitle(tr("Bus Definition"));
    setSubTitle(tr("Verify the logical signals in the bus definition."));
    setFinalPage(false);

    connect(&editor_, SIGNAL(portRenamed(QString const&, QString const&)), 
        this, SLOT(portRenamed(QString const&, QString const&)), Qt::UniqueConnection);
    connect(&editor_, SIGNAL(portRemoved(QString const&, const General::InterfaceMode)), 
        this, SLOT(portRemoved(QString const&, const General::InterfaceMode)), Qt::UniqueConnection);
    
    setupLayout();    
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::~BusInterfaceWizardBusDefinitionPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardBusEditorPage::~BusInterfaceWizardBusEditorPage()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::nextId()
//-----------------------------------------------------------------------------
int BusInterfaceWizardBusEditorPage::nextId() const
{
    return BusInterfaceWizard::PAGE_PORTMAPS;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::initializePage()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::initializePage()
{   
    QSharedPointer<Document> abstractionDocument =
        handler_->getModel(*busIf_->getAbstractionTypes()->first()->getAbstractionRef());
    QSharedPointer<AbstractionDefinition> absDef = abstractionDocument.dynamicCast<AbstractionDefinition>();
    portMappings_.clear();

    bool newAbsDef = (absDefVLNV_.isValid() && 
        *busIf_->getAbstractionTypes()->first()->getAbstractionRef() == absDefVLNV_);
    
    if (newAbsDef && absDef)
    {
        mappingMode_ = GENERATE_ALL;
        createLogicalPortsAndMappings(physicalPorts_, absDef);
    }
    else if (absDef->getPortNames(busIf_->getInterfaceMode()).size() == 1)
    {
        mappingMode_ = GENERATE_SINGLE;
        QString logicalSignal = absDef->getPortNames(busIf_->getInterfaceMode()).first();
        createLogicalMappings(physicalPorts_, logicalSignal, 
            absDef->getPortDirection(logicalSignal, busIf_->getInterfaceMode()));
    }
    else
    {
        mappingMode_ = NO_GENERATION;
    }

    editor_.setAbsDef(absDef);   
    editor_.setBusDef(handler_->getModel(busIf_->getBusType()).dynamicCast<BusDefinition>());
    editor_.setProtection(!newAbsDef);   
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::validatePage()
//-----------------------------------------------------------------------------
bool BusInterfaceWizardBusEditorPage::validatePage()
{    
    QVector<QString> errors;
    bool valid = editor_.validate(errors);

    if (!valid)
    {
        QMessageBox warningDialog(QMessageBox::Warning, tr("Warning"),
            tr("Bus definition has the following error(s):\n") + QStringList(errors.toList()).join("\n"),
            QMessageBox::Ok, this);        
        warningDialog.exec();
        return false;
    }
    
    bool newAbsDef = absDefVLNV_.isValid() && 
        *busIf_->getAbstractionTypes()->first()->getAbstractionRef() == absDefVLNV_;

    if (newAbsDef)
    {
        editor_.save();
    }
    
    createPortMaps();

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusEditorPage::portRenamed()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::portRenamed(QString const& oldName, QString const& newName)
{
    for (QMap<QString, QString>::iterator i = portMappings_.begin(); i != portMappings_.end(); i++)
    {
        if (QString::compare(i.value(), oldName) == 0)
        {
            portMappings_.insert(i.key(), newName);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusEditorPage::portRemoved()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::portRemoved(QString const& portName, const General::InterfaceMode mode)
{
    if (mode == busIf_->getInterfaceMode() || mode == General::INTERFACE_MODE_COUNT)
    {
        QStringList removeable;
        for (QMap<QString, QString>::iterator i = portMappings_.begin(); i != portMappings_.end(); i++)
        {
            if (QString::compare(i.value(), portName) == 0 && !removeable.contains(i.key()))
            {
                removeable.append(i.key());
            }
        }

        foreach(QString port, removeable)
        {
            portMappings_.remove(port);
        }
    }       
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::setupLayout()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(&editor_);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::createLogicalPortsAndMappings()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::createLogicalPortsAndMappings(QStringList const& physPorts, 
    QSharedPointer<AbstractionDefinition> absDef)
{
    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > logicalPorts = absDef->getLogicalPorts(); 
    logicalPorts->clear();

    foreach(QString const& port, physPorts)
    {
        QSharedPointer<Port> physPort = component_->getPort(port);
        DirectionTypes::Direction portDirection = physPort->getDirection();
        int portWidth = getPortSize(physPort);

        QSharedPointer<PortAbstraction> absPort(0);

        QRegExp indexExp("\\[(\\d+)\\]");
        QString absPortName = physPort->name().toUpper();        
        if (portNamesPolicy_ == DESCRIPTION)
        {
            absPortName = physPort->description().toUpper();
            indexExp.indexIn(absPortName);
            absPortName.remove(indexExp);
            absPort = findPortByName(absPortName, logicalPorts);
        }

        if (!absPort.isNull())
        {
            int index = indexExp.cap(1).toInt(); //!< On failed capture, index = 0.
            int masterPortWidth =
                expressionParser_->parseExpression(absPort->getWire()->getMasterPort()->getWidth()).toInt();
            int physicalPortSize = index + getPortSize(physPort);
            int newSize = qMax(masterPortWidth, physicalPortSize);

            absPort->getWire()->getMasterPort()->setWidth(QString::number(newSize));
            absPort->getWire()->getSlavePort()->setWidth(QString::number(newSize));
        }
        else
        {
            absPort = createAbsPort(absPortName, portDirection, portWidth);
            logicalPorts->append(absPort);
        }               
        portMappings_.insert(port, absPortName);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusEditorPage::createLogicalMappings()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::createLogicalMappings(QStringList const& physPorts, 
    QString const& logicalPort, DirectionTypes::Direction logicalDirection)
{
    foreach(QString const& physPort, physPorts)
    {
        DirectionTypes::Direction physDirection = component_->getPort(physPort)->getDirection();
        if (logicalDirection == DirectionTypes::INOUT || physDirection == DirectionTypes::INOUT ||
            logicalDirection == physDirection )
        {
            portMappings_.insert(physPort, logicalPort);
        }        
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusEditorPage::createPortMaps()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::createPortMaps()
{
    QList<QSharedPointer<PortMap> > portMaps;            
    if (mappingMode_ != NO_GENERATION)
    {
        QSharedPointer<Document> libComp =  
            handler_->getModel(*busIf_->getAbstractionTypes()->first()->getAbstractionRef());
        QSharedPointer<AbstractionDefinition> absDef = libComp.dynamicCast<AbstractionDefinition>();
        if (absDef)
        {
            int logicalMax = 0;
            foreach(QString const& port, physicalPorts_)
            {
                QString logicalName = portMappings_.value(port);

                // Map only to logical signals that are defined.
                if (absDef->hasPort(logicalName, busIf_->getInterfaceMode()))
                {
                    QSharedPointer<Port> physPort = component_->getPort(port);
                    int portWidth = getPortSize(physPort);

                    if (portWidth < 1)
                    {
                        portWidth = 1;
                    }

                    // By default, map whole port aligned to logical bit 0.                                        
                    int lowerLogical = 0;
                    int higherLogical = portWidth - 1;

                    // Use bit index in description to map to logical signal, if logical signals
                    // are generated from the physical ports and index is found in description.
                    QRegExp indexExp("\\[(\\d+)\\]");                    
                    if ( (mappingMode_ == GENERATE_SINGLE || portNamesPolicy_ == DESCRIPTION) && 
                        physPort->description().contains(indexExp))
                    {
                        lowerLogical = indexExp.cap(1).toInt();
                        higherLogical += lowerLogical;                       
                    }
                    // If all ports are mapped to a single logical port, map them in ascending bit order.
                    else if (mappingMode_ == GENERATE_SINGLE)
                    {
                        lowerLogical = logicalMax;
                        higherLogical = lowerLogical + portWidth - 1;
                        logicalMax = lowerLogical + portWidth;
                    }

                    QString absWidth = absDef->getPort(logicalName)->getWire()->getWidth(busIf_->getInterfaceMode());
                    if (absWidth.isEmpty() || higherLogical < absWidth.toInt())
                    {
                        QSharedPointer<PortMap> portMap(new PortMap());

                        QSharedPointer<PortMap::LogicalPort> logicalPort(new PortMap::LogicalPort());
                        logicalPort->name_ = logicalName;
                        logicalPort->range_ = QSharedPointer<Range>(new Range(QString::number(higherLogical), 
                            QString::number(lowerLogical)));
                        portMap->setLogicalPort(logicalPort);

                        QSharedPointer<PortMap::PhysicalPort> physicalPort(new PortMap::PhysicalPort(port));
 
                        physicalPort->partSelect_ = 
                            QSharedPointer<PartSelect>(new PartSelect(QString::number(portWidth - 1),
                            QStringLiteral("0")));
                        portMap->setPhysicalPort(physicalPort);
                        portMaps.append(portMap);
                    }
                }
            }            
        }
    }
    QSharedPointer<QList<QSharedPointer<PortMap> > > busIfPortMaps = 
        busIf_->getAbstractionTypes()->first()->getPortMaps();
    busIfPortMaps->clear();
    busIfPortMaps->append(portMaps);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusEditorPage::createAbsPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> BusInterfaceWizardBusEditorPage::createAbsPort(QString const& portName, 
    DirectionTypes::Direction portDirection, int portWidth)
{
    QSharedPointer<PortAbstraction> absPort(new PortAbstraction());
    absPort->setLogicalName(portName);
    absPort->setWire(QSharedPointer<WireAbstraction>(new WireAbstraction()));

    if (busIf_->getInterfaceMode() == General::MASTER || busIf_->getInterfaceMode() == General::MIRROREDMASTER)
    {
        QSharedPointer<WirePort> masterPort(new WirePort());
        masterPort->setDirection(portDirection);

        QSharedPointer<WirePort> slavePort(new WirePort());
        slavePort->setDirection(DirectionTypes::convert2Mirrored(portDirection));

        absPort->getWire()->setMasterPort(masterPort);
        absPort->getWire()->setSlavePort(slavePort);

        if (portWidth > 0)
        {    
            masterPort->setWidth(QString::number(portWidth));
            slavePort->setWidth(QString::number(portWidth));
        }
    } 
    else if (busIf_->getInterfaceMode() == General::SLAVE || busIf_->getInterfaceMode() == General::MIRROREDSLAVE)
    {
        QSharedPointer<WirePort> slavePort(new WirePort());
        slavePort->setDirection(portDirection);

        QSharedPointer<WirePort> masterPort(new WirePort());
        masterPort->setDirection(DirectionTypes::convert2Mirrored(portDirection));

        absPort->getWire()->setSlavePort(slavePort);
        absPort->getWire()->setMasterPort(masterPort);

        if (portWidth > 0)
        {
            masterPort->setWidth(QString::number(portWidth));
            slavePort->setWidth(QString::number(portWidth));
        }
    }
    else //if(busIf_->getInterfaceMode() == General::SYSTEM || 
         //busIf_->getInterfaceMode() == General::MIRROREDSYSTEM)
    {
        QSharedPointer<WirePort> systemPort(new WirePort());
        systemPort->setDirection(portDirection);

        absPort->getWire()->addSystemPort(systemPort);

        if (portWidth > 0)
        {
            systemPort->setWidth(QString::number(portWidth));
        }
    }
	
    return absPort;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusEditorPage::findPortByName()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> BusInterfaceWizardBusEditorPage::findPortByName(QString const& portName, 
    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports)
{
    foreach (QSharedPointer<PortAbstraction> port, *ports)
    {
        if (QString::compare(portName, port->getLogicalName()) == 0)
        {
            return port;
        }
    }
    return QSharedPointer<PortAbstraction>(0);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::getPortSize()
//-----------------------------------------------------------------------------
int BusInterfaceWizardBusEditorPage::getPortSize(QSharedPointer<Port> targetPort) const
{
    int portLeftBound = expressionParser_->parseExpression(targetPort->getLeftBound()).toInt();
    int portRightBound = expressionParser_->parseExpression(targetPort->getRightBound()).toInt();

    return abs(portLeftBound - portRightBound) + 1;
}