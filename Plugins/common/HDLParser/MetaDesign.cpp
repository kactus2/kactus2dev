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

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <QQueue>

//-----------------------------------------------------------------------------
// Function: MetaDesign::MetaDesign()
//-----------------------------------------------------------------------------
MetaDesign::MetaDesign(LibraryInterface* library,
    MessagePasser* messages,
    QSharedPointer<Design const> design,
    QSharedPointer<DesignConfiguration const> designConf,
    QSharedPointer<MetaInstance> topInstance) :
library_(library),
messages_(messages),
design_(design),
designConf_(designConf),
topInstance_(topInstance),
topFinder_(new ListParameterFinder),
instances_(new QMap<QString,QSharedPointer<MetaInstance> >),
interconnections_(new QList<QSharedPointer<MetaInterconnection> >),
adHocWires_(new QList<QSharedPointer<MetaWire> >)
{
    // Create the finder for the parameters coming from the top.
    // TODO: The parameters must come through the proper chain, rather than directly from the top component!
    QSharedPointer<QList<QSharedPointer<Parameter> > > toplist =
        QSharedPointer<QList<QSharedPointer<Parameter> > >(topInstance->getParameters());
    topFinder_->setParameterList(toplist);
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::~MetaDesign()
//-----------------------------------------------------------------------------
MetaDesign::~MetaDesign()
{
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseHierarchy()
//-----------------------------------------------------------------------------
QList<QSharedPointer<MetaDesign> > MetaDesign::parseHierarchy(LibraryInterface* library, GenerationTuple input,
    QSharedPointer<View> topComponentView)
{
    // Creating null parameters for function call.
    QSharedPointer<ComponentInstance> componentInstance;
    QSharedPointer<ListParameterFinder> topFinder;
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs;

    // Instantiate the top component with the selected design.
    // Obviously it cannot have CEVs or parameters of any other component.
    QSharedPointer<MetaInstance> topMostInstance(new MetaInstance
        (library, input.messages, input.component, topComponentView));
    topMostInstance->parseInstance(componentInstance, topFinder, cevs);

    // Create the design associated with the top component.
    QSharedPointer<MetaDesign> topMostDesign(new MetaDesign
        (library, input.messages, input.design, input.designConfiguration, topMostInstance));

    // Add it to the queue of designs that are to be parsed.
    QQueue<QSharedPointer<MetaDesign> > designs;
    designs.enqueue(topMostDesign);

    // How many sub designs are encountered while parsing.
    int countOfSubDesigns = 0;
    // How many sub design are allowed in a hierarchy in maximum.
    const int MAXIMUM_SUBDESIGNS = 1000;

    // Each module name, except the topmost instance, is associated with the count of the same name.
    QMap<QString,int> names;

    // The list of parsed designs.
    QList<QSharedPointer<MetaDesign> > retval;

    // Parse designs until no more are encountered.
    while (!designs.isEmpty())
    {
         // In each iteration the next from the queue and parse it.
         QSharedPointer<MetaDesign> currentDesign = designs.dequeue();
         currentDesign->parseDesign();

         // Append to the list of returns.
         retval.append(currentDesign);

         // Now take the encountered sub designs into consideration.
         countOfSubDesigns += currentDesign->subDesigns_.count();

         // Must not have too many.
         if (countOfSubDesigns >= MAXIMUM_SUBDESIGNS)
         {
             input.messages->errorMessage(QObject::tr("Hit the limit: %1 DESIGNS IN ONE HIERARCHY!!!")
                 .arg(MAXIMUM_SUBDESIGNS));
             return retval;
         }

         foreach (QSharedPointer<MetaDesign> subDesign, currentDesign->subDesigns_)
         {
             // Put the sub design to the queue of designs that are to be parsed.
             designs.enqueue(subDesign);

             // Take the module name associated with the instantiated top component.
             QString name = subDesign->getTopInstance()->getModuleName();

             // Find the name from the set of existing names.
             QMap<QString,int>::iterator nameIter = names.find(name);
             int count = 0;

             if (nameIter == names.end())
             {
                 // This is the first encounter, so insert it to the list.
                 names.insert(name,1);
             }
             else
             {
                 // This is not the first one -> See how many there are so far and increase the count.
                 count = *nameIter;
                 *nameIter = count + 1;
             }

             // Set the module name as the existing module name + number of encounter before this one.
             subDesign->getTopInstance()->setModuleName(name + "_" + QString::number(count));
         }
    }

    return retval;
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseDesign()
//-----------------------------------------------------------------------------
void MetaDesign::parseDesign()
{
    parseInstances();
    parseInterconnections();
    parseAdHocs();
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseInstances()
//-----------------------------------------------------------------------------
void MetaDesign::parseInstances()
{
    // Go through each component instance in the design.
    foreach(QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
        // We must get its component through VLNV.
        VLNV instanceVLNV = design_->getHWComponentVLNV(instance->getInstanceName());
        QSharedPointer<Component> component = library_->getModel(instanceVLNV).dynamicCast<Component>();

        if (!component)
        {
            messages_->errorMessage(QObject::tr("Design %1: Component of instance %2 was not found: %3")
                .arg(design_->getVlnv().toString(), instance->getInstanceName(), instanceVLNV.toString()));
            continue;
        }

        // The instance may have an active view in the design configuration.
        QSharedPointer<View> activeView;

        if (designConf_)
        {
            QString activeViewName = designConf_->getActiveView(instance->getInstanceName());
            activeView = component->getModel()->findView(activeViewName);
        }

        // Cull the CEVS for the instance.
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs
             (new QList<QSharedPointer<ConfigurableElementValue> >);

        // Instance may have CEVs pointing to component parameters.
        cevs->append(*(instance->getConfigurableElementValues()));

        // Design configuration may have CEVs pointing to module parameters.
        if (designConf_)
        {
            QSharedPointer<ViewConfiguration> viewConfig = designConf_->getViewConfiguration(instance->getInstanceName());

            if (viewConfig)
            {
                cevs->append(*viewConfig->getViewConfigurableElements());
            }
        }

        // Now create the instance, using what we know as the parameters.
        QSharedPointer<MetaInstance> mInstance(new MetaInstance(library_, messages_, component, activeView));
        // Parse.
        mInstance->parseInstance(instance, topFinder_, cevs);
        // Map using the name.
        instances_->insert(instance->getInstanceName(), mInstance);

        // Find also the hierarchical references if applicable.
        findHierarchy(mInstance);
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseInterconnections()
//-----------------------------------------------------------------------------
void MetaDesign::parseInterconnections()
{
    // Go through each non-ad-hoc interconnection in the design.
    foreach (QSharedPointer<Interconnection> connection, *design_->getInterconnections())
    {
        // Mop up all the interfaces in the interconnection.
        QList<QSharedPointer<ActiveInterface> > interfaces = *connection->getActiveInterfaces();
        interfaces.append(connection->getStartInterface());

        // Found interfaces for the interconnection.
        QList<QSharedPointer<MetaInterface> > foundInterInterfaces;
        QList<QSharedPointer<MetaInterface> > foundHierInterfaces;

        // Go through the interfaces.
        foreach (QSharedPointer<ActiveInterface> connectionInterface, interfaces)
        {
            // The matching instance must exist.
            QSharedPointer<MetaInstance> mInstance = instances_->
                value(connectionInterface->getComponentReference());

            if (!mInstance)
            {
                messages_->errorMessage(QObject::tr("Design %1: Instance %2 referred by interconnection %3"
                    " does not exist.")
                    .arg(design_->getVlnv().toString(),
                    connectionInterface->getComponentReference(),
                    connection->name()));
                continue;
            }

            // The interface must be found within the interfaces recognized for the instance.
            QSharedPointer<MetaInterface> mInterface = mInstance->getInterfaces()->
                value(connectionInterface->getBusReference());

            if (!mInterface)
            {
                messages_->errorMessage(QObject::tr("Design %1: Bus interface %2 referred by interconnection %3"
                    " does not exist within component %4.")
                    .arg(design_->getVlnv().toString(),
                    connectionInterface->getBusReference(),
                    connection->name(),
                    mInstance->getComponent()->getVlnv().toString()));
                continue;
            }

            // TODO: Errr if mInterface already has an interconnection!

            // Append to the list.
            foundInterInterfaces.append(mInterface);
        }

        // Go through the hierarchical interfaces.
        foreach (QSharedPointer<HierInterface> connectionInterface, *connection->getHierInterfaces())
        {
            // The interface must be found within the interfaces recognized for the top instance.
            QSharedPointer<MetaInterface> mInterface = topInstance_->getInterfaces()->
                value(connectionInterface->getBusReference());

            if (!mInterface)
            {
                messages_->errorMessage(QObject::tr("Design %1: Bus interface %2 referred by interconnection %3"
                    " does not exist within component %4.")
                    .arg(design_->getVlnv().toString(),
                    connectionInterface->getBusReference(),
                    connection->name(),
                    topInstance_->getComponent()->getVlnv().toString()));
                continue;
            }

            // TODO: Errr if mInterface already has an interconnection!

            // Append to the lists.
            foundHierInterfaces.append(mInterface);
        }

        // If not enough interfaces are in the interconnect, drop it.
        if (foundInterInterfaces.size() + foundHierInterfaces.size() < 2)
        {
            messages_->errorMessage(QObject::tr("Design %1: No bus interfaces were found for ad-hoc"
                " interconnection %2.")
                .arg(design_->getVlnv().toString(),
                connection->name()));
            continue;
        }

        QSharedPointer<MetaInterconnection> mIterconnect;

        // First, try to recycle an existing interconnection:
        // If any of the found interfaces has an interconnection already, use it.
        // TODO: Have validators enforce the rule that each interface may have only one interconnection!
        // Then this step must be omitted.
        foreach (QSharedPointer<MetaInterface> mInterface, foundInterInterfaces)
        {
            if (mInterface->upInterconnection_)
            {
                mIterconnect = mInterface->upInterconnection_;
                break;
            }
        }

        foreach (QSharedPointer<MetaInterface> mInterface, foundHierInterfaces)
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

            // Append to the pool of detected interconnections.
            interconnections_->append(mIterconnect);

            // The interconnection needs to be knowledgeable of the hierarchical interfaces connected to it.
            mIterconnect->hierIfs_ = foundHierInterfaces;
        }

        // Associate the interfaces with the interconnect.
        foreach (QSharedPointer<MetaInterface> mInterface, foundInterInterfaces)
        {
            mInterface->upInterconnection_ = mIterconnect;
            wireInterfacePorts(mInterface, mIterconnect, false);
        }

        // Associate the interfaces with the interconnect.
        foreach (QSharedPointer<MetaInterface> mInterface, foundHierInterfaces)
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
    foreach (QSharedPointer<MetaPort> mPort, mInterface->ports_)
    {
        foreach (QSharedPointer<PortAbstraction> pAbs, *mInterface->absDef_->getLogicalPorts())
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

            // ...and associate them with the wire.
            foreach (QSharedPointer<MetaPortAssignment> mpa, assignments)
            {
                QSharedPointer<MetaWire> mWire = mIterconnect->wires_.value(pAbs->getLogicalName());

                if (!mWire)
                {
                    mWire = QSharedPointer<MetaWire>(new MetaWire);
                    mWire->name_ = mIterconnect->name_ + pAbs->getLogicalName();

                    mIterconnect->wires_.insert(pAbs->getLogicalName(), mWire);
                }

                mpa->wire_ = mWire;
                // Also assign larger bounds for wire, if applicable.
                assignLargerBounds(mWire, mpa->logicalBounds_);

                // Associate the wire with the hierarchical ports.
                if (isHierarchical && !mWire->hierPorts_.contains(mPort))
                {
                    mWire->hierPorts_.append(mPort);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseAdHocs()
//-----------------------------------------------------------------------------
void MetaDesign::parseAdHocs()
{
    // Go through the ad hoc connections within the design.
    foreach(QSharedPointer<AdHocConnection> connection, *design_->getAdHocConnections())
    {
        // Found ports for the interconnection.
        QList<QSharedPointer<MetaPort> > foundPorts;
        QList<QSharedPointer<MetaPort> > foundHierPorts;
        // The part select is expected to be in the same index as its matching port reference.
        QList<QSharedPointer<PartSelect> > matchingPartSelects;

        // Go through the port references within the ad-hoc connection.
        foreach(QSharedPointer<PortReference> portRef, *connection->getInternalPortReferences())
        {
            // The matching instance must exist.
            QSharedPointer<MetaInstance> mInstance = instances_->value(portRef->getComponentRef());

            if (!mInstance)
            {
                messages_->errorMessage(QObject::tr("Design %1: Instance %2 referred by ad-hoc connection %3 does not exist.")
                    .arg(design_->getVlnv().toString(),
                    portRef->getComponentRef(),
                    connection->name()));
                continue;
            }

            // The port must be found within the ad-hoc ports recognized for the instance.
            QSharedPointer<MetaPort> mPort = mInstance->getPorts()->value(portRef->getPortRef());

            if (!mPort)
            {
                messages_->errorMessage(QObject::tr("Design %1: Port %2 referred by ad-hoc connection %3 does"
                    " not exist within component %4.")
                    .arg(design_->getVlnv().toString(),
                    portRef->getPortRef(),
                    connection->name(),
                    mInstance->getComponent()->getVlnv().toString()));
                continue;
            }

            // Append to the lists.
            foundPorts.append(mPort);
            matchingPartSelects.append(portRef->getPartSelect());
        }

        // Go through the hierarchical port references.
        foreach(QSharedPointer<PortReference> portRef, *connection->getExternalPortReferences())
        {
            // The port must be found within the ad-hoc ports recognized for the top instance.
            QSharedPointer<MetaPort> mPort = topInstance_->getPorts()->value(portRef->getPortRef());

            if (!mPort)
            {
                messages_->errorMessage(QObject::tr("Design %1: Port %2 referred by ad-hoc connection %3"
                    " does not exist within component %4.")
                    .arg(design_->getVlnv().toString(),
                    portRef->getPortRef(),
                    connection->name(),
                    topInstance_->getComponent()->getVlnv().toString()));
                continue;
            }

            // Append to the lists.
            foundPorts.append(mPort);
            foundHierPorts.append(mPort);
            matchingPartSelects.append(portRef->getPartSelect());
        }

        // If not enough ports are in the connection, drop it.
        if (foundPorts.size() < 1)
        {
            messages_->errorMessage(QObject::tr("Design %1: No ports were found for ad-hoc connection %2.")
                .arg(design_->getVlnv().toString(),
                connection->name()));
            continue;
        }

        // Create a new wire, but only if at least two ports refer to it.
        QSharedPointer<MetaWire> mWire;
        QString wireName = connection->name();

        if (foundPorts.size() > 1)
        {
            mWire = QSharedPointer<MetaWire>(new MetaWire);
            mWire->name_ = wireName;

            // Append to the pool of detected interconnections.
            adHocWires_->append(mWire);

            // The interconnection needs to be knowledgeable of the hierarchical interfaces connected to it.
            mWire->hierPorts_ = foundHierPorts;
        }

        // Go through each matching port.
        for (int i = 0; i < foundPorts.size(); ++i)
        {
            QSharedPointer<MetaPort> mPort = foundPorts[i];

            // Try to find a default value.
            QString defaultValue;

            if (connection->getTiedValue() == "open")
            {
                defaultValue = "";
            }
            else if (connection->getTiedValue() == "default")
            {
                defaultValue = mPort->defaultValue_;
            }
            else
            {
                defaultValue = connection->getTiedValue();
            }

            // No wire and default value means no assignment.
            if (defaultValue.isEmpty() && !mWire)
            {
                continue;
            }

            bool isHierarchical = foundHierPorts.contains(mPort);

            // New port assignment must be created for the each port.
            QSharedPointer<MetaPortAssignment> mpa(new MetaPortAssignment);

            // Associate the port assignments with the wire.
            mpa->wire_ = mWire;
            // Assign the resolved default value.
            mpa->defaultValue_ = defaultValue;

            // Map the port assignment to the port using the name of the wire.
            if (isHierarchical)
            {
                mPort->downAssignments_.insert(wireName, mpa);
            }
            else
            {
                mPort->upAssignments_.insert(wireName, mpa);
            }

            // Assigning bounds.
            QSharedPointer<PartSelect> ps = matchingPartSelects[i];
            if (ps && !ps->getLeftRange().isEmpty() && !ps->getRightRange().isEmpty())
            {
                // If part select exists, it shall be used.
                mpa->physicalBounds_.first = ps->getLeftRange();
                mpa->physicalBounds_.second = ps->getRightRange();
            }
            else
            {
                // Else just choose the port bounds.
                mpa->physicalBounds_ = mPort->vectorBounds_;
            }

            // Logical bounds are still used to determine wire bounds.
            mpa->logicalBounds_ = mpa->physicalBounds_;

            // Also assign larger bounds for the wire, if applicable.
            if (mWire)
            {
                assignLargerBounds(mWire, mpa->logicalBounds_);
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

    // Must have active view to have hierarchy reference!
    if (!activeView)
    {
        return;
    }

    QSharedPointer<Component> component = mInstance->getComponent();

    // Try to find the instantiations.
    QSharedPointer<DesignInstantiation> dis = component->getModel()->
        findDesignInstantiation(activeView->getDesignInstantiationRef());
    QSharedPointer<DesignConfigurationInstantiation> disg = component->getModel()->
        findDesignConfigurationInstantiation(activeView->getDesignConfigurationInstantiationRef());

    // Try to find the referred documents.
    QSharedPointer<Design> subDesign;
    QSharedPointer<DesignConfiguration> subDesignConfiguration;

    if (dis && dis->getDesignReference())
    {
        // Try to find the referred design.
        subDesign = library_->getModel(*(dis->getDesignReference())).dynamicCast<Design>();

        // If instantiation exists, the referred document must exist!
        if (!subDesign)
        {
            messages_->errorMessage(QObject::tr
                ("Design %1: Subdesign referred by instantiation did not exist: %2")
                .arg(design_->getVlnv().toString(),
                dis->getDesignReference()->toString()));
            return;
        }
    }

    if (disg && disg->getDesignConfigurationReference())
    {
        // Try to find the referred design configuration.
        subDesignConfiguration = library_->getModel(*(disg->getDesignConfigurationReference()))
            .dynamicCast<DesignConfiguration>();

        // If instantiation exists, the referred document must exist!
        if (!subDesignConfiguration)
        {
            messages_->errorMessage(QObject::tr
                ("Design %1: Design configuration referred by instantiation did not exist: %2")
                .arg(design_->getVlnv().toString(),
                disg->getDesignConfigurationReference()->toString()));

            return;
        }

        if (subDesign)
        {
            // If the design is already found, check for discrepancy.
            if (subDesignConfiguration->getDesignRef() != subDesign->getVlnv())
            {
                messages_->errorMessage(QObject::tr
                    ("Design %1: Design configuration %2 of sub design %3 refers to different VLNV: %4")
                    .arg(design_->getVlnv().toString(),
                    subDesignConfiguration->getVlnv().toString(),
                    subDesign->getVlnv().toString(),
                    subDesignConfiguration->getDesignRef().toString()));
                return;
            }
        }
        else
        {
            // Else pick the referred design as sub design.
            subDesign = library_->getModel(subDesignConfiguration->getDesignRef()).dynamicCast<Design>();
        }
    }

    // If a sub design exists, it must be also parsed.
    if (subDesign)
    {
        QSharedPointer<MetaDesign> subMetaDesign(new MetaDesign
            (library_, messages_, subDesign, subDesignConfiguration, mInstance));
        subDesigns_.append(subMetaDesign);
    }
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