//-----------------------------------------------------------------------------
// File: HDLDesignParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 06.07.2016
//
// Description:
// Preliminary design parsing for HDL generation.
//-----------------------------------------------------------------------------

#include "HDLDesignParser.h"
#include "HDLComponentParser.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/MultipleParameterFinder.h>
#include <editors/ComponentEditor/common/ListParameterFinder.h>

#include <IPXACTmodels/common/PortAlignment.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Model.h>

#include <QSet>

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::HDLParser()
//-----------------------------------------------------------------------------
HDLDesignParser::HDLDesignParser(LibraryInterface* library, QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConf) : QObject(0),
library_(library),
design_(design),
designConf_(designConf),
retval_(new GenerationDesign),
topFinder_(new ListParameterFinder)
{
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::~HDLParser()
//-----------------------------------------------------------------------------
HDLDesignParser::~HDLDesignParser()
{
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::parseComponentInstances()
//-----------------------------------------------------------------------------
void HDLDesignParser::parseDesign(QSharedPointer<GenerationComponent> topComponent,
    QSharedPointer<View> topComponentView, QSharedPointer<GenerationInstance> topInstance)
{
    // Clear any existing designs.
    parsedDesigns_.clear();
    // Set the top level component.
    retval_->topComponent_ = topComponent;
    // Set the instance from upper level.
    retval_->topInstance_ = topInstance;

    QSharedPointer<QList<QSharedPointer<Parameter> > > toplist;

    // If the top component is based on an instance, use parameters of the instance instead.
    if (topInstance)
    {
        toplist = QSharedPointer<QList<QSharedPointer<Parameter> > >
            (new QList<QSharedPointer<Parameter> >(topInstance->parameters));
    }
    else
    {
        toplist = QSharedPointer<QList<QSharedPointer<Parameter> > >
            (new QList<QSharedPointer<Parameter> >(topComponent->formattedParameters));
    }

    // Set the list for the finder.
    topFinder_->setParameterList(toplist);

    // Parse instances first.
	parseComponentInstances();
    // Then we can find interconnections, and assign them appropriately.
	findInterconnections();
	assignInterconnections();

    // Do the same for the internal ad hocs.
	findInternalAdhocs();
	assignInternalAdHocs();

    // Hiearchical ad hocs need to be considered separately.
	parseHierarchicallAdhocs();

    // Finally add the parsed design to the list.
    parsedDesigns_.append(retval_);

    // Thousand designs is currently deemed as the maximum, and it is pretty much already.
    if (parsedDesigns_.size() >= 1000)
    {
        emit reportError(tr("Hit the limit: THOUSAND DESIGNS IN ONE GENERATION."));
        return;
    }

    // Go through the parsed instances.
    foreach(QSharedPointer<GenerationInstance> gi, retval_->instances_)
    {
        if (gi->design_ && gi->designConfiguration_)
        {
            // If an instance has design AND design configuration, parse it as a design as well.
            HDLDesignParser parser(library_, gi->design_, gi->designConfiguration_);
            // NOTICE: The instance shall be fed as a parameter, since it carries significance in certain situations.
            parser.parseDesign(gi->component, gi->activeView_, gi);
            // Append its designs to the list.
            parsedDesigns_.append(parser.getParsedDesigns());
        }
        else if(gi->activeView_->isHierarchical())
        {
            // If the instance has a hierarchical design despite lacking design or design configuration, it is regarded as an error.
            emit reportError(tr("Design configuration %1: Active view %2 of instance %3 is hierarchical, but is missing design or design configuration.")
                .arg(designConf_->getVlnv().toString(), gi->activeView_->name(), gi->componentInstance_->getInstanceName()));
        }
    }

    // If this was the topmost, make sure that all parsed designs have a unique module name.
    if (!topInstance)
    {
        // Each name is associated with the count of the same name.
        QMap<QString,int> names;

        foreach(QSharedPointer<GenerationDesign> gd, parsedDesigns_)
        {
            QString name = gd->topComponent_->moduleName_;

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
            gd->topComponent_->moduleName_ = name + "_" + QString::number(count);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::parseExpression()
//-----------------------------------------------------------------------------
QString HDLDesignParser::parseExpression(IPXactSystemVerilogParser& parser, const QString& expression) const
{
    QString value = parser.parseExpression(expression);

    if (value == "x")
    {
        return "0";
    }

    return value;
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::parseComponentInstances()
//-----------------------------------------------------------------------------
void HDLDesignParser::parseComponentInstances()
{
	// Go through each component instance in the design.
	foreach(QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
	{
		// We must get its instance through VLNV.
        VLNV instanceVLNV = design_->getHWComponentVLNV(instance->getInstanceName());

        QSharedPointer<Component> component = library_->getModel(instanceVLNV).dynamicCast<Component>();

		if (!component)
        {
            emit reportError(tr("Design %1: Component of instance %2 was not found: %3")
                .arg(design_->getVlnv().toString(), instance->getInstanceName(), instanceVLNV.toString()));
			continue;
        }

		// The instance must have an active view in the design configuration.
        QString activeViewName = designConf_->getActiveView(instance->getInstanceName());
		QSharedPointer<View> activeView = component->getModel()->findView(activeViewName);

		if (!activeView)
        {
            emit reportError(tr("Design configuration %1: Active view %2 of instance %3 was not found.")
                .arg(designConf_->getVlnv().toString(), activeViewName, instance->getInstanceName()));
			continue;
		}

        // Find also the hierarchical references if applicable.
        QSharedPointer<DesignInstantiation> dis = component->getModel()->
            findDesignInstantiation(activeView->getDesignInstantiationRef());
        QSharedPointer<DesignConfigurationInstantiation> disg = component->getModel()->
            findDesignConfigurationInstantiation(activeView->getDesignConfigurationInstantiationRef());

        // Parse the component first
        HDLComponentParser componentParser(library_, component);
        componentParser.parseComponent(activeView);
        QSharedPointer<GenerationComponent> gc = componentParser.getParsedComponent();

		// Mop up this information, append to the list.
		QSharedPointer<GenerationInstance> gi(new GenerationInstance);
		gi->component = gc;
		gi->componentInstance_ = instance;
		gi->activeView_ = activeView;
		retval_->instances_.insert(instance->getInstanceName(), gi);

        // If there are references, assign them as well.
        if (disg && disg->getDesignConfigurationReference())
        {
            gi->designConfiguration_ = library_->getModel(*(disg->getDesignConfigurationReference()))
                .dynamicCast<DesignConfiguration>();

            // If the design configuration is found, use its design reference.
            if (gi->designConfiguration_)
            {
                gi->design_ = library_->getModel(gi->designConfiguration_->getDesignRef()).dynamicCast<Design>();
            }
        }
        else if (dis && dis->getDesignReference())
        {
            gi->design_ = library_->getModel(*(dis->getDesignReference())).dynamicCast<Design>();
        }

		// Go through the culled parameters, find if any exists in CEVs.
		foreach(QSharedPointer<Parameter> parameter, gc->originalParameters)
		{
			// Get the existing value.
			QString paraValue = parameter->getValue();

			foreach(QSharedPointer<ConfigurableElementValue> cev, *instance->getConfigurableElementValues())
			{
				// If a CEV refers to the parameter, its value shall be the value of the parameter.
				if (cev->getReferenceId() == parameter->getValueId())
				{
                    paraValue = cev->getConfigurableValue();
					break;
				}
            }

            foreach(QSharedPointer<ConfigurableElementValue> cev,
                *designConf_->getViewConfiguration(instance->getInstanceName())->getViewConfigurableElements())
            {
                // If a CEV refers to the parameter, its value shall be the value of the parameter.
                if (cev->getReferenceId() == parameter->getValueId())
                {
                    paraValue = cev->getConfigurableValue();
                    break;
                }
            }

			// Make a copy of the parameter.
			QSharedPointer<Parameter> parameterCopy(new Parameter(*parameter));
			// Assign its value.
			parameterCopy->setValue(paraValue);

			// Append to the list of the parameters which shall be used.
			gi->parameters.append(parameterCopy);
        }

        // Initialize the parameter parsing: Find parameters from both the instance and the top component.
        QSharedPointer<QList<QSharedPointer<Parameter> > > ilist(new QList<QSharedPointer<Parameter> >(gi->parameters));
        QSharedPointer<ListParameterFinder> instanceFinder(new ListParameterFinder);
        instanceFinder->setParameterList(ilist);

        QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
        multiFinder->addFinder(instanceFinder);
        multiFinder->addFinder(topFinder_);

        IPXactSystemVerilogParser instanceParser(multiFinder);

        // Parse values.
        foreach(QSharedPointer<Parameter> parameter, gi->parameters)
        {
            parameter->setValue(parseExpression(instanceParser, parameter->getValue()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::findInterconnections()
//-----------------------------------------------------------------------------
void HDLDesignParser::findInterconnections()
{
	// Go through each non-ad-hoc interconnection in the design.
	foreach (QSharedPointer<Interconnection> connection, *design_->getInterconnections())
	{
		// Mop up all the interface in the interconnection.
		QList<QSharedPointer<ActiveInterface> > interfaces = *connection->getActiveInterfaces();
		interfaces.append(connection->getStartInterface());

		// "Our" interconnection matching the interconnections.
		QSharedPointer<GenerationInterconnection> gic;

		// The instance-interface pairs connected to the interconnection.
		QList<QSharedPointer<GenerationInterface> > foundInterfaces;

        QString typeName;

		// Go through the interfaces.
		foreach (QSharedPointer<ActiveInterface> connectionInterface, interfaces)
		{
			// Name of the instance + name of the interface is unique within the design.
            QSharedPointer<GenerationInstance> gis = retval_->instances_.value(connectionInterface->getComponentReference());

            // The matching instance must exist.
            if (!gis)
            {
                continue;
            }

            QSharedPointer<GenerationInterface> ourInterface = gis->component->interfaces.value(connectionInterface->getBusReference());

            // The matching interface must exist within the component.
            if (!ourInterface)
            {
                continue;
            }

            typeName = ourInterface->interface->getBusType().getName();

			// Append to the list.
			foundInterfaces.append(ourInterface);

			// Go through existing "our" interconnections.
			foreach(QSharedPointer<GenerationInterconnection> existing, retval_->interConnections_)
			{
				// Go through their interfaces.
                if (existing->interfaces.contains(ourInterface))
                {
                    gic = existing;
                    break;
				}
			}
		}

		// If a matching "our" interconnection did not exist, create a new one.
		if (!gic)
		{
			gic = QSharedPointer<GenerationInterconnection>(new GenerationInterconnection);
            gic->name = connection->name();

            // First one defines the type.
            gic->typeName = typeName;

            // Remember the top interface: It marks this as a hierarchical interconnection.
            if (!connection->getHierInterfaces()->isEmpty())
            {
                QSharedPointer<HierInterface> topInterface = connection->getHierInterfaces()->first();
                gic->topInterface_ = retval_->topComponent_->interfaces.value(topInterface->getBusReference());
            }

			// Append to the pool of detected interconnections.
			retval_->interConnections_.append(gic);
		}

		// Finally, append the found interfaces to the "our" interconnection: They belong to it.
		gic->interfaces.append(foundInterfaces);
	}
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::assignInterconnections()
//-----------------------------------------------------------------------------
void HDLDesignParser::assignInterconnections()
{
	// Go through detected instances.
	foreach (QSharedPointer<GenerationInstance> gi, retval_->instances_)
	{
		QSharedPointer<GenerationComponent> component = gi->component;
		QSharedPointer<ComponentInstance> instance = gi->componentInstance_;

		// Go through interfaces of the component.
		foreach(QSharedPointer<GenerationInterface> gif, component->interfaces)
        {
            QSharedPointer<GenerationInterconnection> interconnect;

            // Go through detected interconnections.
            foreach (QSharedPointer<GenerationInterconnection> gic, retval_->interConnections_)
            {
                if (gic->interfaces.contains(gif))
                {
                    interconnect = gic;
                    break;
                }
            }

            // No interconnection means no interface assignment.
            if (!interconnect)
            {
                continue;
            }

            // Find correct abstraction type.
            QSharedPointer<AbstractionType> absType = gif->absType;

            // Must have an abstraction type, for else there cannot be a defined interface.
            if (!absType)
            {
                emit reportError(tr("Component %1: No abstraction type was found for bus interface %2 with view %3.")
                    .arg(component->component->getVlnv().toString(), gif->interface->name(), 
                    gi->activeView_->name()));
                continue;
            }

			// Try to find an abstraction definition matching it.
			QSharedPointer<AbstractionDefinition> absDef = gif->absDef;

            // Abstraction definition is mandatory.
            if (!absDef)
            {
                emit reportError(tr("Component %1: Abstraction definition %2 was not found. Bus interface: %3 Active view: %4.")
                    .arg(component->component->getVlnv().toString(), absType->getAbstractionRef()->toString(),
                    gif->interface->name(), gi->activeView_->name()));
                continue;
            }

			// Go through port maps within the abstraction type.
            parsePortMaps(absType, gi, interconnect, absDef, gif->interface);

            // Create an interface assignment, coupling the interface with the interconnection.
            QSharedPointer<GenerationInterfaceAssignment> gifa(new GenerationInterfaceAssignment);
            gifa->interface_ = gif;
            gifa->interConnection_ = interconnect;
            gi->interfaceAssignments_.append(gifa);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::parsePortMaps()
//-----------------------------------------------------------------------------
void HDLDesignParser::parsePortMaps(QSharedPointer<AbstractionType> absType,
    QSharedPointer<GenerationInstance> gi, QSharedPointer<GenerationInterconnection> gic,
    QSharedPointer<AbstractionDefinition> absDef, QSharedPointer<BusInterface> busInterface)
{
    foreach(QSharedPointer<PortMap> portMap, *absType->getPortMaps())
    {
        // Detect the existing port assignment.
        QSharedPointer<GenerationPortAssignMent> gpa = gi->portAssignments_.value(portMap->getPhysicalPort()->name_);

        // No duplicates allowed.
        if (gpa)
        {
            continue;
        }

        // The bounds for port assignment must be found.
        QPair<QString, QString> portBounds;

        // The port map ought to use some physical port.
        QSharedPointer<GenerationPort> physicalPort = gi->component->ports.value(portMap->getPhysicalPort()->name_);

        // Find the logical bounds of the port map.
        portBounds = logicalPortBoundsInInstance(gi, portMap);

        // If it does not exist, use the physical bounds instead.
        if (portBounds.first.isEmpty() || portBounds.second.isEmpty())
        {
            portBounds = physicalPortBoundsInInstance(gi, physicalPort);
        }

        // Now create the port assignment, with the physical port as identifier, and the found port bounds.
        gpa = QSharedPointer<GenerationPortAssignMent>(new GenerationPortAssignMent);
        gpa->port = physicalPort;
        gpa->bounds = portBounds;
        gpa->adhoc = false;
        gi->portAssignments_.insert(portMap->getPhysicalPort()->name_,gpa);

        if (gic->topInterface_)
        {
            // A hierarchical interconnection detected: No wire is used, but the matching
            // port from the connected top interface must be found.
            QSharedPointer<PortMap> matchingTopPort;

            foreach(QSharedPointer<PortMap> topPortMap, *gic->topInterface_->interface->getPortMaps())
            {
                // The logical port is the one that must match.
                if (topPortMap->getLogicalPort()->name_ == portMap->getLogicalPort()->name_)
                {
                    matchingTopPort = topPortMap;
                    break;
                }
            }

            // "Connect" the port assignment directly to the top port.
            if (matchingTopPort)
            {
                gpa->topPort = matchingTopPort->getPhysicalPort()->name_;
            }
        }
        else if (!portMap->getLogicalPort()->name_.isEmpty())
        {
            // A non-hierarchical interconnection detected: A wire must be found or created.
            QSharedPointer<GenerationWire> gw = gic->wires_.value(portMap->getLogicalPort()->name_);

            if (!gw)
            {
                // Create a new one...
                gw = QSharedPointer<GenerationWire>(new GenerationWire);
                // ...and use the logical port name as the key.
                gic->wires_[portMap->getLogicalPort()->name_] = gw;
                // Form the name for the wire.
                gw->name = gic->name + "_" + portMap->getLogicalPort()->name_;
            }

            // Try find the width defined by abstraction definition.
            QSharedPointer<PortAbstraction> portAbs = absDef->getPort(
                portMap->getLogicalPort()->name_, busInterface->getInterfaceMode());
            QString absDefWidth;

            if (portAbs && portAbs->getWire())
            {
                absDefWidth = portAbs->getWire()->getWidth(busInterface->getInterfaceMode());
            }

            QPair<int,int> boundCand;

            // If found, the width shall dictate wire bounds, else it is the same as port bounds.
            if (!absDefWidth.isEmpty())
            {
                QSharedPointer<QList<QSharedPointer<Parameter> > > ilist(new QList<QSharedPointer<Parameter> >(gi->parameters));
                QSharedPointer<ListParameterFinder> instanceFinder(new ListParameterFinder);
                instanceFinder->setParameterList(ilist);

                QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
                multiFinder->addFinder(instanceFinder);
                multiFinder->addFinder(topFinder_);

                // Parse the bounds.
                IPXactSystemVerilogParser portParser(multiFinder);

                boundCand.first = parseExpression(portParser, absDefWidth + "-1").toInt();
                boundCand.second = 0;
            }
            else
            {
                boundCand.first = portBounds.first.toInt();
                boundCand.second = portBounds.second.toInt();
            }

            // Assign larger bounds for wire, if applicable.
            assignLargerBounds(gw, boundCand);

            // The wire shall be the wire of the port assignment.
            gpa->wire = gw;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::assignLargerBounds()
//-----------------------------------------------------------------------------
void HDLDesignParser::assignLargerBounds(QSharedPointer<GenerationWire> wire, QPair<int,int> &boundCand)
{
    // Do the comparison only there are existing bounds.
    if (!wire->bounds.first.isEmpty() && !wire->bounds.second.isEmpty())
    {
        // Check the size of the new bounds.
        int maxAlignment1 = qMax(boundCand.first, boundCand.second);
        int minAlignment1 = qMin(boundCand.first, boundCand.second);

        QPair<int,int> existingBound;

        // Check the size of the new bounds.
        existingBound.first = wire->bounds.first.toInt();
        existingBound.second = wire->bounds.second.toInt();;

        // Compare.
        int maxAlignment2 = qMax(existingBound.first, existingBound.second);
        int minAlignment2 = qMin(existingBound.first, existingBound.second);

        // Assign.
        wire->bounds.first = QString::number(qMax(maxAlignment1,maxAlignment2));
        wire->bounds.second = QString::number(qMin(minAlignment1,minAlignment2));
    }
    else
    {
        // No existing bounds -> This shall be the new one.
        wire->bounds.first = QString::number(boundCand.first);
        wire->bounds.second = QString::number(boundCand.second);
    }
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::findInternalAdhocs()
//-----------------------------------------------------------------------------
void HDLDesignParser::findInternalAdhocs()
{
	// Go through the ad hoc connections within the design.
	foreach(QSharedPointer<AdHocConnection> adHocConnection, *design_->getAdHocConnections())
	{
		// Since this is about internal ad-hocs, skip if any externals are found.
		if (adHocConnection->getExternalPortReferences()->size() > 0)
		{
			continue;
		}

		// A new ad-hoc connection...
		QSharedPointer<GenerationAdHoc> gah;

		// ...and ports connected to it.
		QList<QPair<QString,QString> > foundPorts;

		// Go through the port references within the ad-hoc connection.
		foreach(QSharedPointer<PortReference> internalPort, *adHocConnection->getInternalPortReferences())
		{
			// Pair the instance and the port.
			QPair<QString,QString> port;
			port.first = internalPort->getComponentRef();
			port.second = internalPort->getPortRef();
			foundPorts.append(port);

			// Go through existing detected ad-hocs.
			foreach(QSharedPointer<GenerationAdHoc> existing, retval_->adHocs_)
			{
				typedef QPair<QString, QString> customPair;
				foreach(customPair theirPort, existing->ports)
				{
					// If both instance and port matches, we use this ad-hoc connection.
					if (theirPort.first == port.first && theirPort.second == port.second)
					{
						gah = existing;
						break;
					}
				}

				if(gah)
				{
					break;
				}
			}
		}

		// If the instance and its port was not found in existing ones, create a new.
		if (!gah)
		{
			gah = QSharedPointer<GenerationAdHoc>(new GenerationAdHoc);

			// Create the wire of the ad hoc connection.
			QSharedPointer<GenerationWire> gw(new GenerationWire);
			gw->name = adHocConnection->name();
			gah->wire = gw;

			// If any tied value exists, it will be recorded as well.
			gah->tieOff = adHocConnection->getTiedValue();

			// Add to the pool of existing ones.
			retval_->adHocs_.append(gah);
		}

		// Finally, the found pairs are appended to the connection.
		gah->ports.append(foundPorts);
	}
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::assignInternalAdHocs()
//-----------------------------------------------------------------------------
void HDLDesignParser::assignInternalAdHocs()
{
	// Go through each instance.
	foreach (QSharedPointer<GenerationInstance> gi, retval_->instances_)
	{
		// Go through each detected internal adhoc interconnection.
		foreach(QSharedPointer<GenerationAdHoc> existing, retval_->adHocs_)
		{
			// Go through each port reference associated with the interconnection.
			typedef QPair<QString, QString> customPair;
			foreach(customPair theirPort, existing->ports)
			{
				// If the instance name does not match, it does not belong to this instance.
				if (theirPort.first != gi->componentInstance_->getInstanceName())
				{
					continue;
				}

				// If the port is not found from the component of the instance, it cannot be used.
                QSharedPointer<GenerationPort> ourPort = gi->component->ports.value(theirPort.second);

				if (!ourPort)
				{
					continue;
				}

				// Find the matching port assignment.
				QSharedPointer<GenerationPortAssignMent> gpa = gi->portAssignments_.value(ourPort->port->name());

				// Create a new one if not found.
				if (!gpa)
				{
					gpa = QSharedPointer<GenerationPortAssignMent>(new GenerationPortAssignMent);
					gi->portAssignments_.insert(ourPort->port->name(),gpa);
                    gpa->port = ourPort;
                    gpa->adhoc = true;
				}

                // Try to apply a tie off value.
                gpa->tieOff = connectTieOff(existing->tieOff, ourPort, DirectionTypes::IN);

				if (gpa->tieOff.isEmpty())
				{
					// If no tie-off connection cannot be applied, the wire is used.
					QSharedPointer<GenerationWire> gw = existing->wire;
					gpa->wire = gw;

					// Since it is an ad-hoc connection, a physical connection is the only choice.
					QPair<QString,QString> bounds = physicalPortBoundsInInstance(gi, ourPort);
					gpa->bounds = bounds;

					// Also, if no prior bounds in wire, use these.
					// According to IP-XACT, number bits of connected ah-hoc ports is the same,
					// so we trust in validation here.
					if (gw->bounds.first.isEmpty() || gw->bounds.second.isEmpty())
					{
                        gw->bounds = bounds;
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::parseHierarchicallAdhocs()
//-----------------------------------------------------------------------------
void HDLDesignParser::parseHierarchicallAdhocs()
{
	// For each ad-hoc connection in design...
	foreach(QSharedPointer<AdHocConnection> adHocConnection, *design_->getAdHocConnections())
	{
		// ...go trough the external ports.
		foreach(QSharedPointer<PortReference> externalPort, *adHocConnection->getExternalPortReferences())
		{
			QSharedPointer<GenerationPort> topPort = retval_->topComponent_->ports.value(externalPort->getPortRef());

			// No top port means no action.
			if (!topPort)
			{
				continue;
			}

			// No internal port references means direct tie-off without instance, if any tie-off exists.
			if (adHocConnection->getInternalPortReferences()->size() < 1)
			{
                QString value = connectTieOff(adHocConnection->getTiedValue(), topPort, DirectionTypes::OUT);
                retval_->portTiedValues_.insert(topPort->port->name(), value);
			}

			// Find connected instances.
			foreach (QSharedPointer<GenerationInstance> gi, retval_->instances_)
			{
				// Go through connected internal ports.
				foreach(QSharedPointer<PortReference> internalPort, *adHocConnection->getInternalPortReferences())
				{
					// The instance must match the reference.
					if (internalPort->getComponentRef() != gi->componentInstance_->getInstanceName())
					{
						continue;
					}

					// The referred port must exist in the component.
					QSharedPointer<GenerationPort> ourPort = gi->component->ports.value(internalPort->getPortRef());

					if (!ourPort)
					{
						continue;
					}

					// Create a new port assignment.
					QSharedPointer<GenerationPortAssignMent> gpa(new GenerationPortAssignMent);
        			gpa->port = ourPort;
					gi->portAssignments_.insert(ourPort->port->name(),gpa);

					// It will not have a wire, but reference to the top port.
					gpa->topPort = externalPort->getPortRef();
                    gpa->adhoc = true;

					// Since it is ad hoc, the physical bounds will be used.
					QPair<QString,QString> bounds = physicalPortBoundsInInstance(gi, ourPort);
					gpa->bounds = bounds;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::physicalBoundsInInstance()
//-----------------------------------------------------------------------------
QPair<QString, QString> HDLDesignParser::physicalPortBoundsInInstance(QSharedPointer<GenerationInstance> instance,
    QSharedPointer<GenerationPort> port) const
{
	QPair<QString, QString> bounds("", "");

	// Must have components, both the target and the top, as well as the target port.
	if (!instance || !port)
	{
		return bounds;
	}

    // Find parameters from both the component and the top component, as the component may refer to the top.
    QSharedPointer<QList<QSharedPointer<Parameter> > > ilist(new QList<QSharedPointer<Parameter> >(instance->parameters));
    QSharedPointer<ListParameterFinder> instanceFinder(new ListParameterFinder);
    instanceFinder->setParameterList(ilist);

    QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
    multiFinder->addFinder(instanceFinder);
    multiFinder->addFinder(topFinder_);

	// Parse the bounds.
	IPXactSystemVerilogParser portParser(multiFinder);

	// Use the physical bounds of the port, as promised.
    // Notice: We are taking the bounds directly from the IP-XACT port, as those are not formatted.
	bounds.first = parseExpression(portParser, port->port->getLeftBound());
	bounds.second = parseExpression(portParser, port->port->getRightBound());

	return bounds;
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::logicalBoundsInInstance()
//-----------------------------------------------------------------------------
QPair<QString, QString> HDLDesignParser::logicalPortBoundsInInstance(QSharedPointer<GenerationInstance> instance,
    QSharedPointer<PortMap> portMap) const
{
    QPair<QString, QString> bounds("", "");

	// Must have components, both the target and the top, as well as the target port map.
	if (!instance || !portMap)
	{
		return bounds;
	}

    // Find parameters from both the component and the top component, as the component may refer to the top.
    QSharedPointer<QList<QSharedPointer<Parameter> > > ilist(new QList<QSharedPointer<Parameter> >(instance->parameters));
    QSharedPointer<ListParameterFinder> instanceFinder(new ListParameterFinder);
    instanceFinder->setParameterList(ilist);

    QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
    multiFinder->addFinder(instanceFinder);
    multiFinder->addFinder(topFinder_);

    // Parse the bounds.
    IPXactSystemVerilogParser portParser(multiFinder);

	// Logical port for the map must exist, as well as the range.
	if (portMap->getLogicalPort() && portMap->getLogicalPort()->range_)
	{
		// Pick the range expressions as the logical bounds.
		bounds.first = parseExpression(portParser,portMap->getLogicalPort()->range_->getLeft());
        bounds.second = parseExpression(portParser,portMap->getLogicalPort()->range_->getRight());
    }

    return bounds;
}

//-----------------------------------------------------------------------------
// Function: HDLDesignParser::connectTieOff()
//-----------------------------------------------------------------------------
QString HDLDesignParser::connectTieOff(QString tieOff, QSharedPointer<GenerationPort> port,
	DirectionTypes::Direction requiredDirection)
{
	QString tieOffValue = tieOff;

	// Must not be empty.
	if (!tieOffValue.isEmpty())
	{
		// Must be valid direction.
		if (port->port->getDirection() == requiredDirection || port->port->getDirection() == DirectionTypes::INOUT)
		{
			// Check certain constants as defined in the standard.
			if (QString::compare(tieOffValue, "default", Qt::CaseInsensitive) == 0)
			{
				// Pick the default value of the physical port.
				tieOffValue = port->port->getDefaultValue();
			}
			else if (QString::compare(tieOffValue, "open", Qt::CaseInsensitive) == 0)
			{
				// Leave it open.
				tieOffValue = "";
			}
		}
		else
		{
			// Else leave it open.
			tieOffValue = "";
		}

        // Finally, return the value.
		return tieOffValue;
	}

	// No tie-off for the connection.
	return "";
}