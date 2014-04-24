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

#include <models/component.h>
#include <models/abstractiondefinition.h>
#include <models/busdefinition.h>
#include <editors/BusEditor/absdefgroup.h>
#include <editors/BusEditor/busdefgroup.h>
#include <editors/ComponentEditor/ports/portsdelegate.h>
#include <library/LibraryManager/libraryinterface.h>
#include <library/LibraryManager/vlnv.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::BusInterfaceWizardBusDefinitionPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardBusEditorPage::BusInterfaceWizardBusEditorPage(QSharedPointer<Component> component,
    QSharedPointer<BusInterface> busIf,
    LibraryInterface* lh, QStringList physicalPorts, 
    BusInterfaceWizard* parent, 
    VLNV& absDefVLNV, 
    SignalNamingPolicy namingPolicy )
    : QWizardPage(parent),
    component_(component),
    busIf_(busIf),
    physicalPorts_(physicalPorts),
    handler_(lh),
    absDefVLNV_(absDefVLNV),
    editor_(this, handler_, QSharedPointer<BusDefinition>(0), QSharedPointer<AbstractionDefinition>(0), true),
    portNamesPolicy_(namingPolicy),
    portMappings_(),
    mappingMode_(NO_GENERATION)
{       
    setTitle(tr("Bus Definition"));
    setSubTitle(tr("Verify the logical signals in the bus definition."));
    setFinalPage(false);

    connect(&editor_, SIGNAL(portRenamed(const QString&, const QString&)), 
        this, SLOT(portRenamed(const QString&, const QString&)), Qt::UniqueConnection);
    connect(&editor_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), 
        this, SLOT(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);
    
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
    QSharedPointer<LibraryComponent> libComp = handler_->getModel(busIf_->getAbstractionType());
    QSharedPointer<AbstractionDefinition> absDef = libComp.dynamicCast<AbstractionDefinition>();
    portMappings_.clear();

    bool newAbsDef = (absDefVLNV_.isValid() && busIf_->getAbstractionType() == absDefVLNV_);
    
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
    QStringList errors;
    bool valid = editor_.validate(errors);

    if (!valid)
    {
        QMessageBox warningDialog(QMessageBox::Warning,
            tr("Warning"),
            tr("Bus definition has the following error(s):\n") + errors.join("\n"),
            QMessageBox::Ok,
            this);        
        warningDialog.exec();
        return false;
    }

    bool newAbsDef = absDefVLNV_.isValid() && busIf_->getAbstractionType() == absDefVLNV_;
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
    QList<QSharedPointer<PortAbstraction> >  logicalPorts;    

    foreach(QString port, physPorts)
    {
        QSharedPointer<Port> physPort = component_->getPort(port);
        General::Direction portDirection = physPort->getDirection();
        int portWidth = component_->getPortWidth(port);

        QSharedPointer<PortAbstraction> absPort(0);

        QRegExp indexExp("\\[(\\d+)\\]");
        QString absPortName = physPort->getName().toUpper();        
        if (portNamesPolicy_ == DESCRIPTION)
        {
            absPortName = physPort->getDescription().toUpper();
            indexExp.indexIn(absPortName);
            absPortName.remove(indexExp);
            absPort = findPortByName(absPortName, logicalPorts);
        }

        if (!absPort.isNull())
        {
            int index = indexExp.cap(1).toInt(); //!< On failed capture, index = 0.
            int newSize = qMax(absPort->getWidth(General::MASTER), index + physPort->getPortSize());            
            absPort->setMasterWidth(newSize);
            absPort->setSlaveWidth(newSize);
        }
        else
        {
            absPort = createAbsPort(absPortName, portDirection, portWidth);
            logicalPorts.append(absPort);
        }               
        portMappings_.insert(port, absPortName);
    }

    absDef->setPorts(logicalPorts);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusEditorPage::createLogicalMappings()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::createLogicalMappings(QStringList const& physPorts, 
    QString const& logicalPort, General::Direction logicalDirection)
{
    foreach(QString physPort, physPorts)
    {
        General::Direction physDirection = component_->getPortDirection(physPort);
        if (logicalDirection == General::INOUT || physDirection == General::INOUT ||
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
    QList<QSharedPointer<General::PortMap> > portMaps;            
    if (mappingMode_ != NO_GENERATION)
    {
        QSharedPointer<LibraryComponent> libComp =  handler_->getModel(busIf_->getAbstractionType());
        QSharedPointer<AbstractionDefinition> absDef = libComp.dynamicCast<AbstractionDefinition>();
        if (absDef)
        {
            QStringList absPortNames = absDef->getPortNames(busIf_->getInterfaceMode());

            int logicalMax = 0;
            foreach(QString port, physicalPorts_)
            {
                QString logicalName = portMappings_.value(port);

                // Map only to logical signals that are defined.
                if (absPortNames.contains(logicalName))
                {

                    QSharedPointer<Port> physPort = component_->getPort(port);
                    int portWidth = physPort->getPortSize();

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
                        physPort->getDescription().contains(indexExp))
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

                    if (absDef->getPortSize(logicalName, busIf_->getInterfaceMode()) == -1 ||
                        higherLogical < absDef->getPortSize(logicalName, busIf_->getInterfaceMode()))
                    {
                        QSharedPointer<General::PortMap> portMap(new General::PortMap());
                        portMap->logicalPort_ = logicalName;
                        portMap->logicalVector_ = QSharedPointer<Vector>(new Vector(higherLogical, lowerLogical));
                        portMap->physicalPort_ = port;
                        portMap->physicalVector_ = QSharedPointer<Vector>(new Vector(portWidth - 1, 0));
                        portMaps.append(portMap);
                    }
                }
            }            
        }
    }    
    busIf_->setPortMaps(portMaps);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusEditorPage::createAbsPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> BusInterfaceWizardBusEditorPage::createAbsPort(QString const& portName, 
    General::Direction portDirection, int portWidth)
{
    QSharedPointer<PortAbstraction> absPort(new PortAbstraction());
    absPort->setLogicalName(portName);

    if (busIf_->getInterfaceMode() == General::MASTER || 
        busIf_->getInterfaceMode() == General::MIRROREDMASTER)
    {
        absPort->setMasterDirection(portDirection);
        absPort->setSlaveDirection(General::convert2Mirrored(portDirection));            

        if (portWidth > 0)
        {    
            absPort->setMasterWidth(portWidth);
            absPort->setSlaveWidth(portWidth);
        }
    } 
    else if (busIf_->getInterfaceMode() == General::SLAVE ||
        busIf_->getInterfaceMode() == General::MIRROREDSLAVE)
    {
        absPort->setSlaveDirection(portDirection);
        absPort->setMasterDirection(General::convert2Mirrored(portDirection));

        if (portWidth > 0)
        {
            absPort->setMasterWidth(portWidth);
            absPort->setSlaveWidth(portWidth);
        }
    }
    else //if(busIf_->getInterfaceMode() == General::SYSTEM ||
        //busIf_->getInterfaceMode() == General::MIRROREDSYSTEM)
    {
        absPort->setSystemDirection(portDirection);
        if (portWidth > 0)
        {
            absPort->setSystemWidth(portWidth);
        }
    }
	
    return absPort;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusEditorPage::findPortByName()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> BusInterfaceWizardBusEditorPage::findPortByName(QString const& portName, 
    const QList<QSharedPointer<PortAbstraction> >& ports)
{
    foreach (QSharedPointer<PortAbstraction> port, ports)
    {
        if (QString::compare(portName, port->getLogicalName()) == 0)
        {
            return port;
        }
    }
    return QSharedPointer<PortAbstraction>(0);
}