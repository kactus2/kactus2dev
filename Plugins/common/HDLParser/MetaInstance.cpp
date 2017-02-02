//-----------------------------------------------------------------------------
// File: MetaInstance.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 11.01.2017
//
// Description:
// An instance of a component instance within a hierarchy with its parameters and interfaces parsed.
//-----------------------------------------------------------------------------

#include "MetaInstance.h"

#include <library/LibraryManager/libraryinterface.h>

#include "editors/ComponentEditor/common/ComponentParameterFinder.h"
#include <editors/ComponentEditor/common/MultipleParameterFinder.h>
#include "editors/ComponentEditor/common/ExpressionFormatter.h"

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>

//-----------------------------------------------------------------------------
// Function: MetaInstance::MetaInstance()
//-----------------------------------------------------------------------------
MetaInstance::MetaInstance(LibraryInterface* library,
    MessagePasser* messages,
    QSharedPointer<Component> component,
    QSharedPointer<View> activeView,
    QSharedPointer<ComponentInstance> componentInstance,
    QSharedPointer<ListParameterFinder> topFinder,
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs) :
    library_(library),
    messages_(messages),
    component_(component),
    componentInstance_(componentInstance),
    activeView_(activeView)
{
    // Try to find a component instantiation for the view.
    if (activeView_)
    {
        activeInstantiation_ = component_->getModel()->
            findComponentInstantiation(activeView_->getComponentInstantiationRef());

        if (activeInstantiation_)
        {
            // If there is a named component instantiation, its module name shall be used.
            moduleName_ = activeInstantiation_->getModuleName();
        }
    }

    if (moduleName_.isEmpty())
    {
        // No module name. -> Take the name from the VLNV of the component.
        moduleName_ = component_->getVlnv().getName();
    }

    // Must cull the parameters before can use them!
    cullParameters();

    // Initialize the parameter parsing: Find parameters from both the instance and the top component.
    QSharedPointer<QList<QSharedPointer<Parameter> > > ilist(new QList<QSharedPointer<Parameter> >(parameters_));
    QSharedPointer<ListParameterFinder> instanceFinder(new ListParameterFinder);
    instanceFinder->setParameterList(ilist);

    QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
    multiFinder->addFinder(instanceFinder);

    // Use the top finder only if it actually exists.
    if (topFinder)
    {
        multiFinder->addFinder(topFinder);
    }

    IPXactSystemVerilogParser instanceParser(multiFinder);

    parseParameters(instanceParser, cevs);
    parseInterfaces();
    parsePorts(instanceParser);
}

//-----------------------------------------------------------------------------
// Function: MetaInstance::MetaInstance()
//-----------------------------------------------------------------------------
MetaInstance::MetaInstance()
{
}

//-----------------------------------------------------------------------------
// Function: MetaInstance::~MetaInstance()
//-----------------------------------------------------------------------------
MetaInstance::~MetaInstance()
{
}

//-----------------------------------------------------------------------------
// Function: MetaInstance::findParameters()
//-----------------------------------------------------------------------------
void MetaInstance::cullParameters()
{
    // Cull all the component parameters for the original parameters.
    foreach(QSharedPointer<Parameter> parameterOrig, *component_->getParameters())
    {
        QSharedPointer<Parameter> parameterCpy(new Parameter(*parameterOrig));
        parameters_.append(parameterCpy);
    }

    // If there is an active component instantiation, take its module parameters as well.
    if (activeInstantiation_)
    {
        foreach(QSharedPointer<ModuleParameter> parameterOrig, *activeInstantiation_->getModuleParameters())
        {
            QSharedPointer<Parameter> parameterCpy(new Parameter(*parameterOrig));
            parameters_.append(parameterCpy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function:  MetaInstance::parseParameters()
//-----------------------------------------------------------------------------
void MetaInstance::parseParameters(IPXactSystemVerilogParser& parser,
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs)
{
    // If CEVs have been supplied, use them.
    if (cevs)
    {
        // Go through the culled parameters, find if any exists in CEVs.
        foreach(QSharedPointer<Parameter> parameter, parameters_)
        {
            foreach(QSharedPointer<ConfigurableElementValue> cev, *cevs)
            {
                // If a CEV refers to the parameter, its value shall be the value of the parameter.
                if (cev->getReferenceId() == parameter->getValueId())
                {
                    parameter->setValue(cev->getConfigurableValue());
                    break;
                }
            }
        }
    }

    // Parse values.
    foreach(QSharedPointer<Parameter> parameter, parameters_)
    {
        parameter->setValue(parseExpression(parser, parameter->getValue()));
    }
}

//-----------------------------------------------------------------------------
// Function: MetaInstance::parseInterfaces()
//-----------------------------------------------------------------------------
void MetaInstance::parseInterfaces()
{
    foreach(QSharedPointer<BusInterface> busInterface, *component_->getBusInterfaces())
    {
        // Find the correct abstraction type.
        QSharedPointer<AbstractionType> absType;

        // TODO: Find the abstraction type by the active view rather than the first one.
        if (!busInterface->getAbstractionTypes()->isEmpty())
        {
            absType = busInterface->getAbstractionTypes()->first();
        }

        if (!absType)
        {
            messages_->errorMessage(QObject::tr("Component %1: Bus interface %2 does not have an abstraction type!")
                .arg(component_->getVlnv().toString(),
                busInterface->name()));
            continue;
        }

        // An abstraction definition is needed. It comes through VLNV reference.
        QSharedPointer<ConfigurableVLNVReference> absRef = absType->getAbstractionRef();

        if (!absRef)
        {
            messages_->errorMessage(QObject::tr("Component %1: Abstraction type of bus interface %2 does not have abstraction reference!")
                .arg(component_->getVlnv().toString(),
                busInterface->name()));
            continue;
        }

        // Find the abstraction definition from the library.
        QSharedPointer<AbstractionDefinition> absDef =
            library_->getModel(*absRef).dynamicCast<AbstractionDefinition>();

        if (!absDef)
        {
            messages_->errorMessage(QObject::tr("Component %1: Abstraction definition for bus interface %2 was not found: %3")
                .arg(component_->getVlnv().toString(),
                busInterface->name(),
                absRef->toString()));
            continue;
        }

        // Create "our" interface for each IP-XACT interface. Take the relevant values.
        QSharedPointer<MetaInterface> mInterface(new MetaInterface);
        mInterface->mode_ = interfaceMode2Str(busInterface->getInterfaceMode());
        mInterface->interface_ = busInterface;
        mInterface->absType_ = absType;
        mInterface->absDef_ = absDef;

        // Insert to the interface to the list.
        interfaces_.insert(busInterface->name(), mInterface);
    }
}

//-----------------------------------------------------------------------------
// Function: MetaInstance::parsePorts()
//-----------------------------------------------------------------------------
void MetaInstance::parsePorts(IPXactSystemVerilogParser& parser)
{
    foreach (QSharedPointer<Port> cport, *component_->getPorts())
    {
        // Create generation port.
        QSharedPointer<MetaPort> mPort(new MetaPort);

        // Needs a reference to the IP-XACT port.
        mPort->port_ = cport;

        // Both vector and array bounds may be needed.
        mPort->arrayBounds_.first = parseExpression(parser, cport->getArrayLeft());
        mPort->arrayBounds_.second = parseExpression(parser, cport->getArrayRight());

        mPort->vectorBounds_.first = parseExpression(parser, cport->getLeftBound());
        mPort->vectorBounds_.second = parseExpression(parser, cport->getRightBound());

        // Parse the default value.
        mPort->defaultValue_ = parseExpression(parser, cport->getDefaultValue());

        // Try to find an interface where port belongs to.
        bool anyIfUsesThePort = false;
        ports_.insert(cport->name(), mPort);

        foreach (QSharedPointer<MetaInterface> mInterface, interfaces_)
        {
            bool thisIfUsesThePort = false;

            foreach(QSharedPointer<PortMap> pMap, *mInterface->absType_->getPortMaps())
            {
                if (pMap->getPhysicalPort()->name_ == cport->name())
                {
                    // The port mapping must have a corresponding port abstraction in the abstraction definition!
                    QSharedPointer<PortAbstraction> portAbstraction = mInterface->absDef_->getPort(pMap->getLogicalPort()->name_);

                    if (!portAbstraction)
                    {
                        messages_->errorMessage(QObject::tr("Component %1, Bus interface %2: Port abstraction was not found for logical port %3 at abstraction definition %4.")
                            .arg(component_->getVlnv().toString(),
                            mInterface->interface_->name(),
                            pMap->getLogicalPort()->name_,
                            mInterface->absDef_->getVlnv().toString()));
                        continue;
                    }

                    thisIfUsesThePort = true;

                    // Every mapping using the port creates a new assignment for the port.
                    QSharedPointer<MetaPortAssignment> mUpPortAssignment(new MetaPortAssignment);

                    // The default value comes from the port abstraction.
                    mUpPortAssignment->defaultValue_ = portAbstraction->getDefaultValue();

                    // Parse the port map bounds.
                    QPair<QString, QString> logicalBounds = logicalPortBoundsInMapping(parser, pMap);
                    QPair<QString, QString> physicalBounds = physicalPortBoundsInMapping(parser, pMap);

                    // If physical bounds do not exist, they are the same as the port bounds.
                    if (physicalBounds.first.isEmpty() || physicalBounds.second.isEmpty())
                    {
                        physicalBounds = mPort->vectorBounds_;
                    }

                    // If logical bounds do not exist, they are the same as the physical bounds.
                    if (logicalBounds.first.isEmpty() || logicalBounds.second.isEmpty())
                    {
                        logicalBounds = physicalBounds;
                    }

                    // Assign the values.
                    mUpPortAssignment->logicalBounds_ = logicalBounds;
                    mUpPortAssignment->physicalBounds_ = physicalBounds;

                    QSharedPointer<MetaPortAssignment> mDownPortAssignment(new MetaPortAssignment(*mUpPortAssignment.data()));
                    mPort->upAssignments_.insert(pMap->getLogicalPort()->name_, mUpPortAssignment);
                    mPort->downAssignments_.insert(pMap->getLogicalPort()->name_, mDownPortAssignment);
                }
            }

            // Associate the meta port with any interface that uses it.
            if (thisIfUsesThePort)
            {
                mInterface->ports_.insert(cport->name(), mPort);
                anyIfUsesThePort = true;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaInstance::parseExpression()
//-----------------------------------------------------------------------------
QString MetaInstance::parseExpression(IPXactSystemVerilogParser& parser, const QString& expression)
{
    QString value = parser.parseExpression(expression);

    if (value == "x")
    {
        return "0";
    }

    return value;
}

//-----------------------------------------------------------------------------
// Function: MetaInstance::logicalPortBoundsInMapping()
//-----------------------------------------------------------------------------
QPair<QString, QString> MetaInstance::logicalPortBoundsInMapping(IPXactSystemVerilogParser& parser,
    QSharedPointer<PortMap> portMap)
{
    QPair<QString, QString> bounds("", "");

    QSharedPointer<PortMap::LogicalPort> logicalPort = portMap->getLogicalPort();

    if (logicalPort && logicalPort->range_)
    {
        // Pick the range expressions as the logical bounds.
        bounds.first = parseExpression(parser, logicalPort->range_->getLeft());
        bounds.second = parseExpression(parser, logicalPort->range_->getRight());
    }

    return bounds;
}

//-----------------------------------------------------------------------------
// Function: MetaInstance::physicalPortBoundsInMapping()
//-----------------------------------------------------------------------------
QPair<QString, QString> MetaInstance::physicalPortBoundsInMapping(IPXactSystemVerilogParser& parser,
    QSharedPointer<PortMap> portMap)
{
    QPair<QString, QString> bounds("", "");

    QSharedPointer<PortMap::PhysicalPort> physPort = portMap->getPhysicalPort();

    if ((bounds.first.isEmpty() || bounds.second.isEmpty()) && physPort && physPort->partSelect_)
    {
        // Pick the part select expressions as the total width of the physical bounds.
        int left = parseExpression(parser, physPort->partSelect_->getLeftRange()).toInt();
        int right = parseExpression(parser, physPort->partSelect_->getRightRange()).toInt();
        int width = left - right;
        bounds.first = QString::number(width);
        bounds.second = "0";
    }

    return bounds;
}