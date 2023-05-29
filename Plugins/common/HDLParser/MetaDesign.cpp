//-----------------------------------------------------------------------------
// File: MetaDesign.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 11.01.2017
//
// Description:
// An instantiated design with all its parameters, instances, and interconnections parsed.
//-----------------------------------------------------------------------------

#include "MetaDesign.h"

#include <KactusAPI/include/MessageMediator.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/utilities/Search.h>

#include <IPXACTmodels/utilities/ComponentSearch.h>

#include <IPXACTmodels/Component/BusInterface.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <KactusAPI/include/MultipleParameterFinder.h>
#include <KactusAPI/include/ComponentParameterFinder.h>

#include <QVector>

//-----------------------------------------------------------------------------
// Function: MetaDesign::MetaDesign()
//-----------------------------------------------------------------------------
MetaDesign::MetaDesign(LibraryInterface* library,
    MessageMediator* messages,
    QSharedPointer<Design const> design,
    QSharedPointer<DesignInstantiation const> designInstantiation,
    QSharedPointer<DesignConfiguration const> designConf,
    QSharedPointer<MetaInstance> topInstance) :
library_(library),
messages_(messages),
design_(design),
designInstantiation_(designInstantiation),
designConf_(designConf), 
topInstance_(topInstance),
parameters_(new QList<QSharedPointer<Parameter> >()),
instances_(new QMap<std::string,QSharedPointer<MetaInstance> >),
interconnections_(new QList<QSharedPointer<MetaInterconnection> >),
adHocWires_(new QList<QSharedPointer<MetaWire> >)
{

}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseHierarchy()
//-----------------------------------------------------------------------------
QList<QSharedPointer<MetaDesign> > MetaDesign::parseHierarchy(LibraryInterface* library, GenerationTuple input,
    QSharedPointer<View> topComponentView)
{
    QSharedPointer<ComponentInstance> componentInstance(nullptr);
    QSharedPointer<QList<QSharedPointer<Parameter> > > topList(new QList<QSharedPointer<Parameter> >);
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs(nullptr);

    // Instantiate the top component with the selected design.
    // Obviously, it cannot have CEVs or parameters of any other component.
    QSharedPointer<MetaInstance> topMostInstance(new MetaInstance(componentInstance,
        library, input.messages, input.component, topComponentView));

    parseParameters(topMostInstance->getParameters(), topList, cevs);
    topList->append(*topMostInstance->getParameters());

    parseParameters(topMostInstance->getModuleParameters(), topList, cevs);
    topMostInstance->parseInstance();

    // Find the design instantiation matching the design.
    QSharedPointer<DesignInstantiation> designInstantiation(nullptr);
    foreach(QSharedPointer<DesignInstantiation> di, *input.component->getDesignInstantiations())
    {
        if (input.design->getVlnv() == *di->getDesignReference())
        {
            designInstantiation = di;
            break;
        }
    }

    // Create the design associated with the top component.
    QSharedPointer<MetaDesign> topMostDesign(new MetaDesign(library, input.messages, input.design,
        designInstantiation, input.designConfiguration, topMostInstance));

    // Each module name, except the topmost instance, is associated with the count of the same name.
    QMap<QString, int> names;

    QList<QSharedPointer<MetaDesign> > parsedDesigns;
    int subDesignCount = 0;
    const int MAXIMUM_SUBDESIGNS = 1000;
    
    QVector<QSharedPointer<MetaDesign> > designsToParse({ topMostDesign });
    while (!designsToParse.isEmpty())
    {
         QSharedPointer<MetaDesign> currentDesign = designsToParse.takeFirst();
         currentDesign->findInstances();

         subDesignCount += currentDesign->subDesigns_.count();
         if (subDesignCount > MAXIMUM_SUBDESIGNS)
         {
             input.messages->showError(
                 QObject::tr("Exceeded maximum number of designs in one hierarchy: %1.").arg(MAXIMUM_SUBDESIGNS));
             return parsedDesigns;
         }

         for (QSharedPointer<MetaDesign> subDesign : currentDesign->subDesigns_)
         {
             designsToParse.append(subDesign);

             QString name = subDesign->getTopInstance()->getModuleName();
             
             int count = names.value(name, 0);
             names.insert(name, count + 1);

             subDesign->getTopInstance()->setModuleName(name + QLatin1Char('_') + QString::number(count));
         }

         currentDesign->parseDesign();
         parsedDesigns.append(currentDesign);
    }

    return parsedDesigns;
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseDesign()
//-----------------------------------------------------------------------------
void MetaDesign::parseDesign()
{
    parseDesignParamaters();
    parseInstances();
    parseInterconnections();
    parseAdHocs();

    // We know the unconnected assignments only after all of interconnections are parsed.
    removeUnconnectedInterfaceAssignments();
    removeUnconnectedAdHocAssignments();
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseDesignParamaters()
//-----------------------------------------------------------------------------
void MetaDesign::parseDesignParamaters()
{
    // Copy all design parameters.
    for(QSharedPointer<Parameter> parameterOrig : *design_->getParameters())
    {
        parameters_->append(QSharedPointer<Parameter>(new Parameter(*parameterOrig)));
    }

    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs(nullptr);
    if (designInstantiation_)
    {
        cevs = designInstantiation_->getDesignReference()->getConfigurableElementValues();
    }

    // Provide the list of parameters that are referable by CEVs.
    QSharedPointer<QList<QSharedPointer<Parameter> > > topParameters(new QList<QSharedPointer<Parameter> >());
    topParameters->append(*topInstance_->getParameters());

    // Also other parameters within the component may be referred.
    QSharedPointer<ComponentParameterFinder> componentParameters(new ComponentParameterFinder(topInstance_->getComponent()));
    topParameters->append(componentParameters->allCpuParameters());
    topParameters->append(componentParameters->allGeneratorParameters());
    topParameters->append(componentParameters->allBusInterfaceParameters());
    topParameters->append(componentParameters->allAddressSpaceParameters());
    topParameters->append(componentParameters->allRegisterParameters());

    parseParameters(parameters_, topParameters, cevs);

    // Finally, add these parameters to the top instance.
    for (QSharedPointer<Parameter> original : *parameters_)
    {
        // Try find an existing meta-parameter with a matching name.
        auto foundParameter = Search::findByName(original->name(), *topInstance_->getMetaParameters());

        if (foundParameter.isNull() == false)
        {
            foundParameter->setName(original->name());
            foundParameter->setValue(original->getValue());
            foundParameter->setValueResolve(original->getValueResolve());            
        }
        else
        {
            topInstance_->getMetaParameters()->append(original);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::findInstances()
//-----------------------------------------------------------------------------
void MetaDesign::findInstances()
{
    for (QSharedPointer<ComponentInstance> instance : *design_->getComponentInstances())
    {
        VLNV instanceVLNV = design_->getHWComponentVLNV(instance->getInstanceName());
        QSharedPointer<Component> component = library_->getModel<Component>(instanceVLNV);

        if (!component)
        {
            messages_->showError(QObject::tr("Design %1: Component of instance %2 was not found: %3")
                .arg(design_->getVlnv().toString(), 
                    QString::fromStdString(instance->getInstanceName()), instanceVLNV.toString()));
            continue;   
        }

        // The instance may have an active view in the design configuration.
        QSharedPointer<View> activeView;
        if (designConf_)
        {
            activeView = ComponentSearch::findView(component,
                QString::fromStdString(designConf_->getActiveView(instance->getInstanceName())));
        }

        // No chosen active view -> If there is only one in the component, use it.
        if (!activeView && component->getViews()->size() > 0)
        {
            if (component->getViews()->size() == 1)
            {
                activeView = component->getViews()->first();
            }
            else
            {
                messages_->showError(QObject::tr("Design %1: Instance %2 did not have specified active view, "
                    "and its component %3 has multiple possible views, so no active view was chosen.")
                    .arg(design_->getVlnv().toString(), 
                        QString::fromStdString(instance->getInstanceName()),
                        instanceVLNV.toString()));
            }
        }

        QSharedPointer<MetaInstance> mInstance(new MetaInstance(instance, library_, messages_, component,
            activeView));
        instances_->insert(instance->getInstanceName(), mInstance);
        findHierarchy(mInstance);
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseParameters()
//-----------------------------------------------------------------------------
void MetaDesign::parseParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > subList,
    QSharedPointer<QList<QSharedPointer<Parameter> > > topList,
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs)
{
    // If CEVs have been supplied, use them.
    if (cevs)
    {
        for (QSharedPointer<Parameter> parameter : *subList)
        {
            for (QSharedPointer<ConfigurableElementValue> cev : *cevs)
            {
                // If a CEV refers to the parameter, its value shall be the value of the parameter.
                if (cev->getReferenceId().compare(parameter->getValueId()) == 0)
                {
                    parameter->setValue(cev->getConfigurableValue());
                    break;
                }
            }
        }
    }

    // Initialize the parameter parsing: Find parameters from both the instance and the top component.
    QSharedPointer<ListParameterFinder> subFinder(new ListParameterFinder);
    subFinder->setParameterList(subList);

    QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
    multiFinder->addFinder(subFinder);

    // If top list has been supplied, use it.
    if (topList)
    {
        QSharedPointer<ListParameterFinder> topFinder(new ListParameterFinder);
        topFinder->setParameterList(topList);

        multiFinder->addFinder(topFinder);
    }

    IPXactSystemVerilogParser parser(multiFinder);
    for (QSharedPointer<Parameter> parameter : *subList)
    {
        parameter->setValue(parser.parseExpression(parameter->getValue()));
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseInstances()
//-----------------------------------------------------------------------------
void MetaDesign::parseInstances()
{    
    for (QSharedPointer<MetaInstance> mInstance : *instances_)
    {
        // Parse parameters of the instance, which may be overridden by
        // component instance CEVs, which may point to the design parameters.
        parseParameters(mInstance->getParameters(), parameters_,
            mInstance->getComponentInstance()->getConfigurableElementValues());

        // Module parameters may refer to the component parameters.
        QSharedPointer<QList<QSharedPointer<Parameter> > > topList(new QList<QSharedPointer<Parameter> >);
      
        topList->append(*mInstance->getParameters());
        
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs;

        // Design configuration may have CEVs pointing to module parameters.
        if (designConf_)
        {
            QSharedPointer<ViewConfiguration> viewConfig = designConf_->getViewConfiguration(
                mInstance->getComponentInstance()->getInstanceName());

            if (viewConfig)
            {
                // TODO: Use view configuration CEVs.
                //cevs = viewConfig->getViewConfigurableElements();
            }
        }

        // Parse module parameters of the instance, TODO: which may be overridden by view configuration CEVS,
        // which may point to the design configuration parameters.
        parseParameters(mInstance->getModuleParameters(), topList, cevs);

        mInstance->parseInstance();
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseInterconnections()
//-----------------------------------------------------------------------------
void MetaDesign::parseInterconnections()
{
    // Go through each non-ad-hoc interconnection in the design.
    for (QSharedPointer<Interconnection> connection : *design_->getInterconnections())
    {
        QList<QSharedPointer<ActiveInterface> > interfaces = *connection->getActiveInterfaces();
        interfaces.append(connection->getStartInterface());

        QList<QSharedPointer<MetaInterface> > foundInterInterfaces;
        QList<QSharedPointer<MetaInterface> > foundHierInterfaces;

        for (QSharedPointer<ActiveInterface> connectionInterface : interfaces)
        {
            QSharedPointer<MetaInstance> mInstance = 
                instances_->value(connectionInterface->getComponentReference());
            if (!mInstance)
            {
                messages_->showError(QObject::tr("Design %1: Instance %2 referred by interconnection %3"
                    " does not exist.").arg(design_->getVlnv().toString(),
                        QString::fromStdString(connectionInterface->getComponentReference()),
                        connection->name()));
                continue;
            }

            QSharedPointer<MetaInterface> mInterface = mInstance->getInterfaces()->value(
                QString::fromStdString(connectionInterface->getBusReference()));
            if (!mInterface)
            {
                messages_->showError(QObject::tr("Design %1: Bus interface %2 referred by interconnection %3"
                    " does not exist within component %4.")
                    .arg(design_->getVlnv().toString(), 
                        QString::fromStdString(connectionInterface->getBusReference()),
                    connection->name(), mInstance->getComponent()->getVlnv().toString()));
                continue;
            }

            // TODO: Errr if mInterface already has an interconnection!

            foundInterInterfaces.append(mInterface);
        }

        for (QSharedPointer<HierInterface> hierInterface : *connection->getHierInterfaces())
        {
            QSharedPointer<MetaInterface> mInterface = topInstance_->getInterfaces()->value(
                QString::fromStdString(hierInterface->getBusReference()));

            if (!mInterface)
            {
                messages_->showError(QObject::tr("Design %1: Bus interface %2 referred by interconnection %3"
                    " does not exist within component %4.").arg(design_->getVlnv().toString(),
                        QString::fromStdString(hierInterface->getBusReference()), connection->name(),
                    topInstance_->getComponent()->getVlnv().toString()));
                continue;
            }

            // TODO: Errr if mInterface already has an interconnection!

            foundHierInterfaces.append(mInterface);
        }

        // If not enough interfaces are in the interconnect, drop it.
        if (foundInterInterfaces.size() + foundHierInterfaces.size() < 2)
        {
            messages_->showError(
                QObject::tr("Design %1: No bus interfaces were found for interconnection %2.").arg(
                    design_->getVlnv().toString(), connection->name()));
            continue;
        }

        QSharedPointer<MetaInterconnection> mIterconnect;

        // First, try to reuse an existing interconnection:
        // If any of the found interfaces has an interconnection already, use it.
        // TODO: Have validators enforce the rule that each interface may have only one interconnection.
        // Then this step must be omitted.
        for (QSharedPointer<MetaInterface> mInterface : foundInterInterfaces)
        {
            if (mInterface->upInterconnection_)
            {
                mIterconnect = mInterface->upInterconnection_;
                break;
            }
        }

        for (QSharedPointer<MetaInterface> mInterface : foundHierInterfaces)
        {
            if (mInterface->downInterconnection_)
            {
                mIterconnect = mInterface->downInterconnection_;
                break;
            }
        }

        // If a matching interconnection did not exist, create a new one.
        if (!mIterconnect)
        {
            mIterconnect = QSharedPointer<MetaInterconnection>(new MetaInterconnection);
            mIterconnect->name_ = connection->name();

            interconnections_->append(mIterconnect);

            // The interconnection needs to know of the hierarchical interfaces connected to it.
            mIterconnect->hierIfs_ = foundHierInterfaces;
        }

        // Associate the interfaces with the interconnect.
        for (QSharedPointer<MetaInterface> mInterface : foundInterInterfaces)
        {
            mInterface->upInterconnection_ = mIterconnect;
            wireInterfacePorts(mInterface, mIterconnect, false);
        }

        for (QSharedPointer<MetaInterface> mInterface : foundHierInterfaces)
        {
            mInterface->downInterconnection_ = mIterconnect;
            wireInterfacePorts(mInterface, mIterconnect, true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::wireInterfacePorts()
//-----------------------------------------------------------------------------
void MetaDesign::wireInterfacePorts(QSharedPointer<MetaInterface> mInterface,
    QSharedPointer<MetaInterconnection> mIterconnect, bool isHierarchical)
{
    // Associate the port assignments with the wires of the interconnect.
    for (QSharedPointer<MetaPort> mPort : mInterface->ports_)
    {
        for (QSharedPointer<PortAbstraction> pAbs : *mInterface->absDef_->getLogicalPorts())
        {
            // ...get all port assignments in the interface utilizing its logical port...
            QList<QSharedPointer<MetaPortAssignment> > assignments;

            if (isHierarchical)
            {
                assignments = mPort->downAssignments_.values(pAbs->getLogicalName());
            }
            else
            {
                assignments = mPort->upAssignments_.values(pAbs->getLogicalName());
            }

            QList<QSharedPointer<MetaWire> > connectedWires;
            QList<QSharedPointer<MetaTransactional> > connectedTransactionals;

            // ...and associate them with the wire.
            for (QSharedPointer<MetaPortAssignment> assignment : assignments)
            {
                if (assignment->mappedInterface_ == mInterface)
                {
                    if (pAbs->hasWire())
                    {
                        associateWithWire(mIterconnect, pAbs, assignment, connectedWires, isHierarchical, mPort);
                    }
                    else if (pAbs->hasTransactional())
                    {
                        associateWithTransactional(
                            mIterconnect, pAbs, assignment, connectedTransactionals, isHierarchical, mPort);
                    }

                }

            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::associateWithWire()
//-----------------------------------------------------------------------------
void MetaDesign::associateWithWire(QSharedPointer<MetaInterconnection> metaInterconnect,
    QSharedPointer<PortAbstraction> portAbstraction, QSharedPointer<MetaPortAssignment> assignment,
    QList<QSharedPointer<MetaWire>>& connectedWires, bool isHierarchical, QSharedPointer<MetaPort> metaPort)
{
    QSharedPointer<MetaWire> mWire = metaInterconnect->wires_.value(portAbstraction->getLogicalName());
    if (!mWire)
    {
        mWire = QSharedPointer<MetaWire>(new MetaWire);
        mWire->name_ = metaInterconnect->name_ + QLatin1Char('_') + portAbstraction->getLogicalName();
        mWire->refCount = 0;

        metaInterconnect->wires_.insert(portAbstraction->getLogicalName(), mWire);
    }

    if (assignment->wire_.isNull() || connectedWires.contains(assignment->wire_))
    {
        ++mWire->refCount;
        assignment->wire_ = mWire;
        connectedWires.append(mWire);
    }
    else
    {
        ++assignment->wire_->refCount;
        connectedWires.append(assignment->wire_);
    }

    // Also assign larger bounds for wire, if applicable.
    assignLargerBounds(mWire, assignment->logicalBounds_);

    // Associate the wire with the hierarchical ports.
    if (isHierarchical && !mWire->hierPorts_.contains(metaPort))
    {
        mWire->hierPorts_.append(metaPort);
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::associateWithTransactional()
//-----------------------------------------------------------------------------
void MetaDesign::associateWithTransactional(QSharedPointer<MetaInterconnection> metaInterconnect,
    QSharedPointer<PortAbstraction> portAbstraction, QSharedPointer<MetaPortAssignment> assignment,
    QList<QSharedPointer<MetaTransactional>>& connectedTransactionals, bool isHierarchical,
    QSharedPointer<MetaPort> metaPort)
{
    QSharedPointer<MetaTransactional> metaTransactional =
        metaInterconnect->transactionals_.value(portAbstraction->getLogicalName());
    if (!metaTransactional)
    {
        metaTransactional = QSharedPointer<MetaTransactional>(new MetaTransactional);
        metaTransactional->name_ = metaInterconnect->name_ + QLatin1Char('_') + portAbstraction->getLogicalName();
        metaTransactional->refCount_ = 0;

        metaInterconnect->transactionals_.insert(portAbstraction->getLogicalName(), metaTransactional);
    }

    if (assignment->transactional_.isNull() || connectedTransactionals.contains(assignment->transactional_))
    {
        ++metaTransactional->refCount_;
        assignment->transactional_ = metaTransactional;
        connectedTransactionals.append(metaTransactional);
    }
    else
    {
        ++assignment->transactional_->refCount_;
        connectedTransactionals.append(assignment->transactional_);
    }

    // Associate the wire with the hierarchical ports.
    if (isHierarchical && !metaTransactional->hierPorts_.contains(metaPort))
    {
        metaTransactional->hierPorts_.append(metaPort);
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseAdHocs()
//-----------------------------------------------------------------------------
void MetaDesign::parseAdHocs()
{
    for (QSharedPointer<AdHocConnection> connection : *design_->getAdHocConnections())
    {
        QList<QSharedPointer<MetaPort> > foundPorts;
        QList<QSharedPointer<MetaPort> > foundHierPorts;

        // The part select is expected to be in the same index as its matching port reference.
        QList<QSharedPointer<PartSelect> > matchingPartSelects;

        findPortsInAdHoc(connection, foundPorts, matchingPartSelects);
        findHierarchicalPortsInAdHoc(connection, foundPorts, foundHierPorts, matchingPartSelects);

        // If not enough ports are in the connection, drop it.
        if (foundPorts.size() < 1)
        {
            messages_->showError(QObject::tr("Design %1: No ports were found for ad-hoc connection %2.").arg(
                design_->getVlnv().toString(), connection->name()));
            continue;
        }

        if (foundPorts.first()->isWire_)
        {
            QSharedPointer<MetaWire> mWire;
            QString wireName = connection->name();

            if (foundPorts.size() > 1)
            {
                mWire = QSharedPointer<MetaWire>(new MetaWire);
                mWire->name_ = wireName;
                mWire->refCount = 0;

                // Append to the pool of detected interconnections.
                adHocWires_->append(mWire);

                // The interconnection needs to know of the hierarchical interfaces connected to it.
                mWire->hierPorts_ = foundHierPorts;
            }

            for (int i = 0; i < foundPorts.size(); ++i)
            {
                QSharedPointer<MetaPort> mPort = foundPorts[i];
                bool isHierarchical = foundHierPorts.contains(mPort);

                if (mPort->isWire_)
                {
                    parseAdHocAssignmentForPort(mPort, connection, mWire, isHierarchical, wireName,
                        matchingPartSelects.at(i));
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseAdHocAssignmentForPort()
//-----------------------------------------------------------------------------
void MetaDesign::parseAdHocAssignmentForPort(QSharedPointer<MetaPort> mPort,
    QSharedPointer<AdHocConnection> connection,
    QSharedPointer<MetaWire> mWire,
    bool isHierarchical,
    QString wireName,
    QSharedPointer<PartSelect> partSelect)
{
    std::string defaultValue;
    if (connection->getTiedValue().compare("open") == 0)
    {
        //defaultValue = "";
    }
    else if (connection->getTiedValue().compare("default") == 0)
    {
        defaultValue = mPort->defaultValue_.toStdString();
    }
    else
    {
        defaultValue = connection->getTiedValue();
    }

    // No wire or default value means no assignment.
    if (!mWire)
    {
        if (defaultValue.empty() )
        {
            messages_->showError(
                QObject::tr("Design %1: Ad-hoc connection %2 needs either more ports or a tie-off.").arg(
                    design_->getVlnv().toString(), connection->name()));
            return;
        }
    }
    else
    {
        ++mWire->refCount;
    }

    // New port assignment must be created for the each port.
    QSharedPointer<MetaPortAssignment> assignment(new MetaPortAssignment);

    // Associate the port assignments with the wire.
    assignment->wire_ = mWire;
    assignment->defaultValue_ = QString::fromStdString(defaultValue);
    assignment->invert_ = false;

    // Map the port assignment to the port using the name of the wire.
    if (isHierarchical)
    {
        mPort->downAssignments_.insert(wireName, assignment);
    }
    else
    {
        mPort->upAssignments_.insert(wireName, assignment);
    }

    // Assigning bounds.
    if (partSelect && !partSelect->getLeftRange().isEmpty() && !partSelect->getRightRange().isEmpty())
    {
        // If part select exists, it shall be used.
        assignment->physicalBounds_.first = partSelect->getLeftRange();
        assignment->physicalBounds_.second = partSelect->getRightRange();
    }
    else
    {
        // Else just choose the port bounds.
        assignment->physicalBounds_ = mPort->vectorBounds_;
    }

    // Determine the part of the wire that shall be assigned to the port:
    // This is [abs(physical.left – physical.right):0]
    QPair<QString, QString> newBounds;
    newBounds.first = QString::number(assignment->physicalBounds_.first.toInt() -
        assignment->physicalBounds_.second.toInt());
    newBounds.second = QStringLiteral("0");

    assignment->logicalBounds_ = newBounds;

    // Also assign larger bounds for the wire, if applicable.
    if (mWire)
    {
        assignLargerBounds(mWire, assignment->logicalBounds_);
    }

}

//-----------------------------------------------------------------------------
// Function: MetaDesign::findHierarchicalPortsInAdHoc()
//-----------------------------------------------------------------------------
void MetaDesign::findHierarchicalPortsInAdHoc(QSharedPointer<AdHocConnection> connection,
    QList<QSharedPointer<MetaPort> > &foundPorts,
    QList<QSharedPointer<MetaPort> > &foundHierPorts,
    QList<QSharedPointer<PartSelect> > &matchingPartSelects)
{
    for (QSharedPointer<PortReference> portRef : *connection->getExternalPortReferences())
    {
        QSharedPointer<MetaPort> mPort = topInstance_->getPorts()->value(
            QString::fromStdString(portRef->getPortRef()));

        if (!mPort)
        {
            messages_->showError(QObject::tr("Design %1: Port %2 referred by ad-hoc connection %3"
                " does not exist within component %4.")
                .arg(design_->getVlnv().toString(),
                    QString::fromStdString(portRef->getPortRef()),
                connection->name(),
                topInstance_->getComponent()->getVlnv().toString()));
            continue;
        }

        foundPorts.append(mPort);
        foundHierPorts.append(mPort);
        matchingPartSelects.append(portRef->getPartSelect());
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::findPortsInAdHoc()
//-----------------------------------------------------------------------------
void MetaDesign::findPortsInAdHoc(QSharedPointer<AdHocConnection> connection,
    QList<QSharedPointer<MetaPort> > &foundPorts,
    QList<QSharedPointer<PartSelect> > &matchingPartSelects)
{
    for (QSharedPointer<PortReference> portRef : *connection->getInternalPortReferences())
    {
        QSharedPointer<MetaInstance> mInstance = instances_->value(portRef->getComponentRef());
        if (!mInstance)
        {
            messages_->showError(
                QObject::tr("Design %1: Instance %2 referred by ad-hoc connection %3 does not exist.")
                .arg(design_->getVlnv().toString(),
                    QString::fromStdString(portRef->getComponentRef()), connection->name()));
            continue;
        }

        // The port must be found within the ad-hoc ports recognized for the instance.
        QSharedPointer<MetaPort> mPort = mInstance->getPorts()->value(QString::fromStdString(portRef->getPortRef()));
        if (!mPort)
        {
            messages_->showError(QObject::tr("Design %1: Port %2 referred by ad-hoc connection %3 does"
                " not exist within component %4.")
                .arg(design_->getVlnv().toString(), 
                    QString::fromStdString(portRef->getPortRef()),
                    connection->name(),
                mInstance->getComponent()->getVlnv().toString()));
            continue;
        }

        foundPorts.append(mPort);
        matchingPartSelects.append(portRef->getPartSelect());
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::removeUnconnectedInterfaceAssignments()
//-----------------------------------------------------------------------------
void MetaDesign::removeUnconnectedInterfaceAssignments()
{
    for (QSharedPointer<MetaInstance> mInstance : *instances_)
    {
        for (QSharedPointer<MetaPort> mPort : *mInstance->getPorts())
        {
            bool connected = false;
            for (QSharedPointer<MetaPortAssignment> assignment : mPort->upAssignments_)
            {
                if ((mPort->isWire_ && ((assignment->wire_ && assignment->wire_->refCount >= 2) ||
                        assignment->defaultValue_.isEmpty() == false)) ||
                    (mPort->isTransactional_ && ((assignment->transactional_ &&
                        assignment->transactional_->refCount_ >= 2) ||
                        assignment->defaultValue_.isEmpty() == false)))
                {
                    connected = true;
                }
            }

            if (connected == false)
            {
                mPort->upAssignments_.clear();
            }
        }
    }

    for (QSharedPointer<MetaInterface> mInterface : *topInstance_->getInterfaces())
    {
        for (QSharedPointer<MetaPort> mPort : *topInstance_->getPorts())
        {
            bool connected = false;
            for (QSharedPointer<MetaPortAssignment> assignment : mPort->downAssignments_)
            {
                if ((mPort->isWire_ && assignment->wire_ && assignment->wire_->refCount >= 2) ||
                    (mPort->isTransactional_ && assignment->transactional_ &&
                        assignment->transactional_->refCount_ >= 2))
                {
                    connected = true;
                }
            }

            if (connected == false)
            {
                mPort->downAssignments_.clear();
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::removeUnconnectedAdHocAssignments()
//-----------------------------------------------------------------------------
void MetaDesign::removeUnconnectedAdHocAssignments()
{
    for (QSharedPointer<MetaInstance> mInstance : *instances_)
    {
        // Go through its ports.
        for (QSharedPointer<MetaPort> mPort : *mInstance->getPorts())
        {
            auto iter = mPort->upAssignments_.begin();
            auto end = mPort->upAssignments_.end();

            while (iter != end)
            {
                QSharedPointer<MetaPortAssignment> assignment = *iter;
                bool isAdHocWire = getAdHocWires()->contains(assignment->wire_);

                // Wire does not have at least two users -> remove.
                if (assignment->wire_ && assignment->wire_->refCount < 2)
                {
                    assignment->wire_ = QSharedPointer<MetaWire>();
                }

                // Remove port assignment that do not match the criteria. Applies only to the ad hoc wires.
                if (!assignment->wire_ && assignment->defaultValue_.isEmpty() && isAdHocWire)
                {
                    iter = mPort->upAssignments_.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }
    }

    for (QSharedPointer<MetaPort> mPort : *topInstance_->getPorts())
    {
        auto iter = mPort->downAssignments_.begin();
        auto end = mPort->downAssignments_.end();

        while (iter != end)
        {
            QSharedPointer<MetaPortAssignment> assignment = *iter;
            bool isAdHocWire = getAdHocWires()->contains(assignment->wire_);

            // Wire does not have at least two users -> remove. Applies only to the ad hoc wires.
            if (assignment->wire_ && assignment->wire_->refCount < 2)
            {
                assignment->wire_ = QSharedPointer<MetaWire>();
            }

            // Remove port assignment that do not match the criteria. Applies only to the ad hoc wires.
            if (!assignment->wire_ && assignment->defaultValue_.isEmpty() && isAdHocWire)
            {
                iter = mPort->downAssignments_.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::findHierarchy()
//-----------------------------------------------------------------------------
void MetaDesign::findHierarchy(QSharedPointer<MetaInstance> mInstance)
{
    QSharedPointer<View> activeView = mInstance->getActiveView();
    if (!activeView)
    {
        return;
    }

    QSharedPointer<Component> component = mInstance->getComponent();

    // Try to find the instantiations.
    QSharedPointer<DesignConfigurationInstantiation> configurationInstantiation = component->getModel()->
        findDesignConfigurationInstantiation(activeView->getDesignConfigurationInstantiationRef());

    QSharedPointer<DesignInstantiation> designInstantiation = component->getModel()->findDesignInstantiation(
        activeView->getDesignInstantiationRef());

    // Try to find the referred documents.
    QSharedPointer<DesignConfiguration> subDesignConfiguration =
        findDesignConfigurationFromInsantiation(configurationInstantiation);
    QSharedPointer<Design> subDesign = findDesignFromInstantiation(designInstantiation, subDesignConfiguration);

    if (subDesign)
    {
        // If a sub design exists, it must be also parsed.
        QSharedPointer<MetaDesign> subMetaDesign(new MetaDesign(library_, messages_, subDesign, 
            designInstantiation, subDesignConfiguration, mInstance));
        subDesigns_.append(subMetaDesign);
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::findDesignConfigurationFromInsantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> MetaDesign::findDesignConfigurationFromInsantiation(
    QSharedPointer<DesignConfigurationInstantiation> configurationInstantiation)
{
    QSharedPointer<DesignConfiguration> referredDesignConfiguration(nullptr);

    if (configurationInstantiation && configurationInstantiation->getDesignConfigurationReference())
    {
        // Try to find the referred design configuration.
        referredDesignConfiguration = library_->getModel<DesignConfiguration>(
            *(configurationInstantiation->getDesignConfigurationReference()));

        // If instantiation exists, the referred document must exist!
        if (!referredDesignConfiguration)
        {
            messages_->showError(QObject::tr
                ("Design %1: Design configuration referred by instantiation did not exist: %2")
                .arg(design_->getVlnv().toString(),
                    configurationInstantiation->getDesignConfigurationReference()->toString()));
        }
    }

    return referredDesignConfiguration;
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::findDesignFromInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<Design> MetaDesign::findDesignFromInstantiation(QSharedPointer<DesignInstantiation> designInstantiation,
    QSharedPointer<DesignConfiguration> configuration)
{
    QSharedPointer<Design> referredDesign(nullptr);

    if (designInstantiation && designInstantiation->getDesignReference())
    {
        referredDesign = library_->getModel<Design>(*(designInstantiation->getDesignReference()));        
        if (!referredDesign)
        {
            messages_->showError(QObject::tr("Design %1: Subdesign referred by instantiation did not exist: %2")
                .arg(design_->getVlnv().toString(), designInstantiation->getDesignReference()->toString()));
        }
    }

    if (configuration)
    {
        if (referredDesign)
        {
            // If the design is already found, check for discrepancy.
            if (configuration->getDesignRef() != referredDesign->getVlnv())
            {
                messages_->showError(
                    QObject::tr("Design %1: Design configuration %2 of sub design %3 refers to different VLNV: %4")
                    .arg(design_->getVlnv().toString(), configuration->getVlnv().toString(),
                        referredDesign->getVlnv().toString(), configuration->getDesignRef().toString()));
            }
        }
        else
        {
            referredDesign = library_->getModel<Design>(configuration->getDesignRef());
        }
    }

    return referredDesign;
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::assignLargerBounds()
//-----------------------------------------------------------------------------
void MetaDesign::assignLargerBounds(QSharedPointer<MetaWire> wire, QPair<QString,QString> const& boundCand)
{
    // Do the comparison only if there are existing bounds.
    if (!wire->bounds_.first.isEmpty() && !wire->bounds_.second.isEmpty())
    {
        QPair<int,int> newBounds;

        // Check the size of the new bounds.
        newBounds.first = boundCand.first.toInt();
        newBounds.second = boundCand.second.toInt();

        // Find the widest alignment order of the new bounds.
        int maxAlignment1 = qMax(newBounds.first, newBounds.second);
        int minAlignment1 = qMin(newBounds.first, newBounds.second);

        QPair<int,int> existingBound;

        // Check the size of the existing bounds.
        existingBound.first = wire->bounds_.first.toInt();
        existingBound.second = wire->bounds_.second.toInt();

        // Find the widest alignment order of the existing bounds.
        int maxAlignment2 = qMax(existingBound.first, existingBound.second);
        int minAlignment2 = qMin(existingBound.first, existingBound.second);

        // Finally, compare and assign.
        wire->bounds_.first = QString::number(qMax(maxAlignment1,maxAlignment2));
        wire->bounds_.second = QString::number(qMin(minAlignment1,minAlignment2));
    }
    else
    {
        // No existing bounds -> This shall be the new one.
        wire->bounds_.first = boundCand.first;
        wire->bounds_.second = boundCand.second;
    }
}
