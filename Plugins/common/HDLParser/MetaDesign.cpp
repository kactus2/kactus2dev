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

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/MultipleParameterFinder.h>
#include <editors/ComponentEditor/common/ListParameterFinder.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/common/PortAlignment.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Model.h>

//-----------------------------------------------------------------------------
// Function: MetaDesign::MetaDesign()
//-----------------------------------------------------------------------------
MetaDesign::MetaDesign(LibraryInterface* library, QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConf, QSharedPointer<MetaInstance> topInstance) :
library_(library),
design_(design),
designConf_(designConf),
topInstance_(topInstance),
topFinder_(new ListParameterFinder)
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > toplist =
        QSharedPointer<QList<QSharedPointer<Parameter> > >
        (new QList<QSharedPointer<Parameter> >(topInstance->parameters_));
    topFinder_->setParameterList(toplist);
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::~MetaDesign()
//-----------------------------------------------------------------------------
MetaDesign::~MetaDesign()
{
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseDesign()
//-----------------------------------------------------------------------------
void MetaDesign::parseDesign(QSharedPointer<GenerationComponent> topComponent,
    QSharedPointer<View> topComponentView, QSharedPointer<GenerationInstance> topInstance)
{
    parseInstances();
    parseInsterconnections();
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseExpression()
//-----------------------------------------------------------------------------
QString MetaDesign::parseExpression(IPXactSystemVerilogParser& parser, const QString& expression) const
{
    QString value = parser.parseExpression(expression);

    if (value == "x")
    {
        return "0";
    }

    return value;
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseInstances()
//-----------------------------------------------------------------------------
void MetaDesign::parseInstances()
{
    // Go through each component instance in the design.
    foreach(QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
        // We must get its instance through VLNV.
        VLNV instanceVLNV = design_->getHWComponentVLNV(instance->getInstanceName());

        QSharedPointer<Component> component = library_->getModel(instanceVLNV).dynamicCast<Component>();

        if (!component)
        {
            //emit reportError(tr("Design %1: Component of instance %2 was not found: %3")
            //    .arg(design_->getVlnv().toString(), instance->getInstanceName(), instanceVLNV.toString()));
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
            cevs->append(*(designConf_->getViewConfiguration(instance->getInstanceName())->
                getViewConfigurableElements()));
        }

        // Now create the instance, using what we know as the parameters.
        QSharedPointer<MetaInstance> mInstance(new MetaInstance
            (library_, component, activeView, instance, topFinder_, cevs));
        // Map using the name.
        instances_.insert(instance->getInstanceName(), mInstance);

        // Find also the hierarchical references if applicable.
        QSharedPointer<DesignInstantiation> dis = component->getModel()->
            findDesignInstantiation(activeView->getDesignInstantiationRef());
        QSharedPointer<DesignConfigurationInstantiation> disg = component->getModel()->
            findDesignConfigurationInstantiation(activeView->getDesignConfigurationInstantiationRef());

        // Try to find the referred documents.
        QSharedPointer<Design> subDesign;
        QSharedPointer<DesignConfiguration> subDesignConfiguration;

        if (disg && disg->getDesignConfigurationReference())
        {
            // The design configuration reference takes the precedence.
            subDesignConfiguration = library_->getModel(*(disg->getDesignConfigurationReference()))
                .dynamicCast<DesignConfiguration>();

            // If the design configuration is found, use its design reference.
            if (subDesignConfiguration)
            {
                subDesign = library_->getModel(subDesignConfiguration->getDesignRef()).dynamicCast<Design>();
            }
        }
        else if (dis && dis->getDesignReference())
        {
            // Try to use the design reference if no design configuration reference is stated.
            subDesign = library_->getModel(*(dis->getDesignReference())).dynamicCast<Design>();
        }

        // If a sub design exists, it must be also parsed.
        if (subDesign)
        {
            QSharedPointer<MetaDesign> subMetaDesign(new MetaDesign(library_, subDesign, subDesignConfiguration, mInstance));
            subDesigns_.append(subMetaDesign);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaDesign::parseInsterconnections()
//-----------------------------------------------------------------------------
void MetaDesign::parseInsterconnections()
{
    // Go through each non-ad-hoc interconnection in the design.
    foreach (QSharedPointer<Interconnection> connection, *design_->getInterconnections())
    {
        // Mop up all the interfaces in the interconnection.
        QList<QSharedPointer<ActiveInterface> > interfaces = *connection->getActiveInterfaces();
        interfaces.append(connection->getStartInterface());

        // Found interfaces for the interconnection.
        QList<QSharedPointer<MetaInterface> > foundInterfaces;
        QList<QSharedPointer<MetaInterface> > foundHierInterfaces;

        // Go through the interfaces.
        foreach (QSharedPointer<ActiveInterface> connectionInterface, interfaces)
        {
            // The matching instance must exist.
            QSharedPointer<MetaInstance> mInstance = instances_.value(connectionInterface->getComponentReference());

            if (!mInstance)
            {
                continue;
            }

            // The interface must be found within the interfaces recognized for the instance.
            QSharedPointer<MetaInterface> mInterface = mInstance->interfaces_.value(connectionInterface->getBusReference());

            if (!mInterface)
            {
                continue;
            }

            // Append to the list.
            foundInterfaces.append(mInterface);
        }

        // Go through the hierarchical interfaces.
        foreach (QSharedPointer<HierInterface> connectionInterface, *connection->getHierInterfaces())
        {
            QSharedPointer<MetaInterface> mInterface = topInstance_->interfaces_.value(connectionInterface->getBusReference());

            // The interface must be found within the interfaces recognized for the top instance.
            if (!mInterface)
            {
                continue;
            }

            // Append to the lists.
            foundInterfaces.append(mInterface);
            foundHierInterfaces.append(mInterface);
        }

        // If no interfaces are in the interconnect, drop it.
        if (foundInterfaces.size() < 1)
        {
            continue;
        }

        QSharedPointer<MetaInterconnection> mIterconnect;

        // First, try to recycle an existing interconnection:
        // If any of the found interfaces has an interconnection already, use it.
        // TODO: Have validators enforce the rule that each interface may have only one interconnection!
        // Then this step must be omitted.
        foreach (QSharedPointer<MetaInterface> mInterface, foundInterfaces)
        {
            if (mInterface->interconnection_)
            {
                mIterconnect = mInterface->interconnection_;
                break;
            }
        }

        // If a matching interconnection did not exist, create a new one.
        if (!mIterconnect)
        {
            mIterconnect = QSharedPointer<MetaInterconnection>(new MetaInterconnection);
            mIterconnect->name_ = connection->name();

            // Append to the pool of detected interconnections.
            interconnections_.append(mIterconnect);

            // The interconnection needs to be knowledgeable of the hierarchical interfaces connected to it.
            mIterconnect->hierIfs_ = foundHierInterfaces;

            // The abstraction definition of the interconnection is the one of the first interface.
            QSharedPointer<AbstractionDefinition> absDef = foundInterfaces.first()->absDef_;

            foreach (QSharedPointer<PortAbstraction> pAbs, *absDef->getLogicalPorts())
            {
                QSharedPointer<MetaWire> mWire = QSharedPointer<MetaWire>(new MetaWire);
                mWire->name_ = mIterconnect->name_ + pAbs->getLogicalName();

                foreach (QSharedPointer<MetaInterface> hierInterface, foundHierInterfaces)
                {
                    mWire->hierPorts_.append(hierInterface->ports_.values());
                }

                mIterconnect->wires_.insert(pAbs->getLogicalName(), mWire);
            }
        }

        // Associate the interfaces with the interconnect.
        foreach (QSharedPointer<MetaInterface> mInterface, foundInterfaces)
        {
            mInterface->interconnection_ = mIterconnect;

            // Associate the port assignments with the wires of the interconnect.
            foreach (QSharedPointer<MetaPort> mPort, mInterface->ports_)
            {
                // For each wire in the interconnect...
                QMap<QString, QSharedPointer<MetaWire> >::iterator iter =  mIterconnect->wires_.begin();
                QMap<QString, QSharedPointer<MetaWire> >::iterator end =  mIterconnect->wires_.end();
                for (;iter != end; ++iter)
                {
                    QSharedPointer<MetaWire> mWire = iter.value();

                    // ...get all port assignments in the interface utilizing its logical port...
                    QList<QSharedPointer<MetaPortAssignMent> > assignments = mPort->assignments_.values(iter.key());

                    // ...and associate them with the wire.
                    foreach (QSharedPointer<MetaPortAssignMent> mpa, assignments)
                    {
                        mpa->wire_ = mWire;
                        // Also assign larger bounds for wire, if applicable.
                        assignLargerBounds(mWire, mpa->bounds_);
                    }
                }
            }
        }
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