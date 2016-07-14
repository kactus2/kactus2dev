//-----------------------------------------------------------------------------
// File: HDLParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 06.07.2016
//
// Description:
// Preliminary parsing for HDL generation.
//-----------------------------------------------------------------------------

#include "HDLParser.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/MultipleParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <designEditors/common/TopComponentParameterFinder.h>

#include <IPXACTmodels/common/PortAlignment.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Model.h>

#include <Plugins/VerilogImport/VerilogSyntax.h>

#include <QDateTime>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: HDLParser::HDLParser()
//-----------------------------------------------------------------------------
HDLParser::HDLParser(LibraryInterface* library, QSharedPointer<Component> component, QSharedPointer<View> topComponentView, 
	QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConf) : QObject(0), 
library_(library),
topComponent_(component),
design_(design),
designConf_(designConf)
{
}

//-----------------------------------------------------------------------------
// Function: HDLParser::~HDLParser()
//-----------------------------------------------------------------------------
HDLParser::~HDLParser()
{
}

//-----------------------------------------------------------------------------
// Function: HDLParser::parse()
//-----------------------------------------------------------------------------
/*QList<QSharedPointer<GenerationInstance> > HDLParser::parse(QSharedPointer<Component> component, QSharedPointer<View> topComponentView, 
	QString const& outputPath, QSharedPointer<Design> design,
	QSharedPointer<DesignConfiguration> designConf )
{
    topComponent_ = component;
    topComponentView_ = topComponentView;
    design_ = design;
	designConf_ = designConf;

    initializeWriters();

    if (design_ && designConf_)
    {
        parseComponentInstances();

		addWritersToTopInDesiredOrder();       
    }
	else
	{
		// If we are not generating based on a design, we must parse the existing implementation.
		QString implementation;
		QString postModule;

		if (!selectImplementation(outputPath, implementation, postModule))
		{
			// If parser says no-go, we dare do nothing.
			return;
		}

		// Must add a warning before the existing implementation.
		QSharedPointer<CommentWriter> tagWriter(new CommentWriter(VerilogSyntax::TAG_OVERRIDE));
		topWriter_->add(tagWriter);

		// Next comes the implementation.
		QSharedPointer<TextBodyWriter> implementationWriter(new TextBodyWriter(implementation));
		topWriter_->add(implementationWriter);

		// Also write any stuff that comes after the actual module.
		QSharedPointer<TextBodyWriter> postModuleWriter(new TextBodyWriter(postModule));
		topWriter_->setPostModule(postModuleWriter);
	}
}*/

//-----------------------------------------------------------------------------
// Function: HDLParser::createParserForComponent()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionFormatter> HDLParser::createFormatterForComponent(QSharedPointer<Component> targetComponent,
	QSharedPointer<View> targetView, QString instanceName)
{    
	QSharedPointer<MultipleParameterFinder> finder(new MultipleParameterFinder());

	QSharedPointer<TopComponentParameterFinder> topFinder(new TopComponentParameterFinder(topComponent_));
	topFinder->setActiveView(topComponentView_);
	finder->addFinder(topFinder);
	QSharedPointer<ComponentParameterFinder> instanceFinder(new ComponentParameterFinder(targetComponent));
	instanceFinder->setActiveView(targetView);
	finder->addFinder(instanceFinder);

	return QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(finder)); 
}

//-----------------------------------------------------------------------------
// Function: HDLParser::createParserForComponent()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionParser> HDLParser::createParserForComponent(QSharedPointer<Component> targetComponent,
	QSharedPointer<View> targetView, QString instanceName)
{    
	QSharedPointer<MultipleParameterFinder> finder(new MultipleParameterFinder());

	QSharedPointer<TopComponentParameterFinder> topFinder(new TopComponentParameterFinder(topComponent_));
	topFinder->setActiveView(topComponentView_);
	finder->addFinder(topFinder);
	QSharedPointer<ComponentParameterFinder> instanceFinder(new ComponentParameterFinder(targetComponent));
	instanceFinder->setActiveView(targetView);
	finder->addFinder(instanceFinder);

	return QSharedPointer<IPXactSystemVerilogParser>(new IPXactSystemVerilogParser(finder)); 
}

//-----------------------------------------------------------------------------
// Function: HDLParser::parseComponentInstances()
//-----------------------------------------------------------------------------
void HDLParser::parseComponentInstances()
{
	foreach(QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
	{
		QSharedPointer<Component> component = getComponentForInstance(instance->getInstanceName());

		if (!component)
		{
			continue;
		}

		QSharedPointer<View> activeView = component->getModel()->
			findView(designConf_->getActiveView(instance->getInstanceName()));

		if (!activeView)
		{
			continue;
		}

		QSharedPointer<ComponentInstantiation> instantiation = component->getModel()->
			findComponentInstantiation(activeView->getComponentInstantiationRef());

		if (!instantiation)
		{
			continue;
		}

		QSharedPointer<GenerationInstance> gi(new GenerationInstance);
		gi->component = component;
		gi->componentInstance_ = instance;
		gi->componentInstantiation_ = instantiation;
		gi->activeView_ = activeView;
		gi->referencedComponent_ = component;

		instances_.append(gi);

		foreach (QSharedPointer<BusInterface> busInterface, *component->getBusInterfaces())
		{
			foreach (QSharedPointer<Interconnection> connection, *design_->getInterconnections())
			{
				QList<QSharedPointer<ActiveInterface> > interfaces = *connection->getActiveInterfaces();
				interfaces.append(connection->getStartInterface());

				QSharedPointer<BusInterface> topBusInterface;

				if (!connection->getHierInterfaces()->isEmpty())
				{
					QSharedPointer<HierInterface> topInterface = connection->getHierInterfaces()->first();
					topBusInterface = topComponent_->getBusInterface(topInterface->getBusReference());
				}

				foreach (QSharedPointer<ActiveInterface> connectionInterface, interfaces)
				{
					if (connectionInterface->getBusReference() != busInterface->name())
					{
						continue;
					}

					QSharedPointer<GenerationInterconnection> gic = gi->interfaces_[busInterface];

					if (!gic)
					{
						gic = interConnections_[connection];

						if (!gic)
						{
							gic = QSharedPointer<GenerationInterconnection>(new GenerationInterconnection);
							gic->name = connection->name();
							gi->interfaces_[busInterface] = gic;
							gic->topInterface_ = topBusInterface;
						}

						gi->interfaces_[busInterface] = gic;
					}

					interConnections_[connection] = gic;
				}
			}
		}
	}

	foreach(QSharedPointer<AdHocConnection> adHocConnection, *design_->getAdHocConnections())
	{
		if (adHocConnection->getExternalPortReferences()->size() > 0)
		{
			continue;
		}

		QSharedPointer<GenerationAdHoc> gah;

		QList<QPair<QString,QString> > foundPorts;

		foreach(QSharedPointer<PortReference> internalPort, *adHocConnection->getInternalPortReferences())
		{
			QPair<QString,QString> port;
			port.first = internalPort->getComponentRef();
			port.second = internalPort->getPortRef();
			foundPorts.append(port);

			foreach(QSharedPointer<GenerationAdHoc> existing, adHocs_)
			{
				typedef QPair<QString, QString> customPair;
				foreach(customPair theirPort, existing->ports)
				{
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

		if (!gah)
		{
			gah = QSharedPointer<GenerationAdHoc>(new GenerationAdHoc);
			adHocs_.append(gah);
			QSharedPointer<GenerationWire> gw(new GenerationWire);
			gw->name = adHocConnection->name();
			gah->wire = gw;
		}

		gah->ports.append(foundPorts);
	}

	foreach (QSharedPointer<GenerationInstance> gi, instances_)
	{
		QSharedPointer<Component> component = gi->component;
		QSharedPointer<ComponentInstance> instance = gi->componentInstance_;

		foreach(QSharedPointer<Port> port, *component->getPorts())
		{
			foreach(QSharedPointer<GenerationAdHoc> existing, adHocs_)
			{
				typedef QPair<QString, QString> customPair;
				foreach(customPair theirPort, existing->ports)
				{
					if (theirPort.first != instance->getInstanceName())
					{
						continue;
					}

					QSharedPointer<Port> ourPort = component->getPort(theirPort.second);

					if (!ourPort)
					{
						continue;
					}

					QSharedPointer<GenerationPortAssignMent> gpa = gi->portAssignments_[ourPort->name()];
					QSharedPointer<GenerationPort> gp;
						
					if (!gpa)
					{
						gpa = QSharedPointer<GenerationPortAssignMent>(new GenerationPortAssignMent);
						gi->portAssignments_.insert(ourPort->name(),gpa);
						gp = QSharedPointer<GenerationPort>(new GenerationPort);
						gpa->port = gp;
					}

					QSharedPointer<GenerationWire> gw = existing->wire;

					gpa->wire = gw;
					gw->ports.append(gp);

					QPair<QString,QString> bounds = physicalPortBoundsInInstance(gi->activeView_, component, ourPort);
					gpa->bounds = bounds;

					if (gw->bounds.first.isEmpty() || gw->bounds.second.isEmpty())
					{
						gw->bounds = bounds;
					}
				}
			}
		}

		foreach(QSharedPointer<AdHocConnection> adHocConnection, *design_->getAdHocConnections())
		{
			if (adHocConnection->getExternalPortReferences()->size() < 1)
			{
				continue;
			}

			foreach(QSharedPointer<PortReference> externalPort, *adHocConnection->getExternalPortReferences())
			{
				foreach(QSharedPointer<PortReference> internalPort, *adHocConnection->getInternalPortReferences())
				{
					if (internalPort->getComponentRef() == instance->getInstanceName())
					{
						QSharedPointer<Port> topPort = topComponent_->getPort(externalPort->getPortRef());
						QSharedPointer<Port> ourPort = component->getPort(internalPort->getPortRef());

						if (!topPort || !ourPort)
						{
							continue;
						}

						QSharedPointer<GenerationPortAssignMent> gpa(new GenerationPortAssignMent);
						gi->portAssignments_.insert(ourPort->name(),gpa);
						gpa->otherName = externalPort->getPortRef();

						QPair<QString,QString> bounds = physicalPortBoundsInInstance(gi->activeView_, component, ourPort);
						gpa->bounds = bounds;
					}
				}
			}
		}
	}

	foreach (QSharedPointer<GenerationInstance> gi, instances_)
	{
		QSharedPointer<Component> component = gi->component;
		QSharedPointer<ComponentInstance> instance = gi->componentInstance_;

		foreach (QSharedPointer<BusInterface> busInterface, *component->getBusInterfaces())
		{
			QSharedPointer<GenerationInterconnection> gic = gi->interfaces_[busInterface];

			if (!gic)
			{
				continue;
			}

			foreach(QSharedPointer<AbstractionType> absType, *busInterface->getAbstractionTypes())
			{
				QSharedPointer<AbstractionDefinition> absDef;

				if (absType->getAbstractionRef())
				{
					QSharedPointer<Document> docAbsDef = library_->getModel(*absType->getAbstractionRef());
					if (docAbsDef)
					{
						absDef = docAbsDef.dynamicCast<AbstractionDefinition>();
					}
				}

				foreach(QSharedPointer<PortMap> portMap, *absType->getPortMaps())
				{
					QPair<QString, QString> wireBounds;
					QPair<QString, QString> portBounds;
					QString absDefWidth;

					portBounds = logicalPortBoundsInInstance(gi->activeView_, component, portMap);

					if (portBounds.first.isEmpty() || portBounds.second.isEmpty())
					{
						portBounds = physicalPortBoundsInInstance(gi->activeView_, component,
							component->getPort(portMap->getPhysicalPort()->name_));
					}

					if (absDef)
					{
						QSharedPointer<PortAbstraction> portAbs = absDef->getPort(
							portMap->getLogicalPort()->name_, busInterface->getInterfaceMode());

						if (portAbs && portAbs->getWire())
						{
							absDefWidth = portAbs->getWire()->getWidth(busInterface->getInterfaceMode());
						}
					}

					if (!absDefWidth.isEmpty())
					{
						wireBounds.first = absDefWidth + "-1";
						wireBounds.second = "0";
					}
					else
					{
						wireBounds = portBounds;
					}

					QSharedPointer<GenerationPort> gp;
					QSharedPointer<GenerationPortAssignMent> gpa = gi->portAssignments_[portMap->getPhysicalPort()->name_];
					
					if (!gpa)
					{
						gpa = QSharedPointer<GenerationPortAssignMent>(new GenerationPortAssignMent);
						gp = QSharedPointer<GenerationPort>(new GenerationPort);
						gpa->port = gp;
						gpa->bounds = portBounds;
						gi->portAssignments_.insert(portMap->getPhysicalPort()->name_,gpa);
					}
					else
					{
						gp = gpa->port;
					}

					if (gic->topInterface_)
					{
						QSharedPointer<PortMap> matchingTopPort;

						foreach(QSharedPointer<PortMap> topPortMap, *gic->topInterface_->getPortMaps())
						{
							if (topPortMap->getLogicalPort()->name_ == portMap->getLogicalPort()->name_)
							{
								matchingTopPort = topPortMap;
								break;
							}
						}

						if (matchingTopPort)
						{
							gpa->otherName = matchingTopPort->getPhysicalPort()->name_;
						}
					}
					else if (!portMap->getLogicalPort()->name_.isEmpty())
					{
						QSharedPointer<GenerationWire> gw = gpa->wire;

						if (!gw)
						{
							gw = gic->wires_[portMap->getLogicalPort()->name_];

							if (!gw)
							{
								gw = QSharedPointer<GenerationWire>(new GenerationWire);
								gic->wires_[portMap->getLogicalPort()->name_] = gw;
								gw->name = gic->name + "_" + portMap->getLogicalPort()->name_;
								gw->bounds = wireBounds;
							}
						}

						gpa->wire = gw;
						gw->ports.append(gp);
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: HDLParser::logicalBoundsInInstance()
//-----------------------------------------------------------------------------
QPair<QString, QString> HDLParser::logicalPortBoundsInInstance(QSharedPointer<View> activeView, QSharedPointer<Component> component, QSharedPointer<PortMap> portMap) const
{
    QPair<QString, QString> bounds("", "");

    if (component && topComponent_)
	{
		QSharedPointer<ComponentParameterFinder> instanceFinder(new ComponentParameterFinder(component));
		instanceFinder->setActiveView(activeView->name());
		QSharedPointer<TopComponentParameterFinder> topFinder(new TopComponentParameterFinder(topComponent_));
		topFinder->setActiveView(topComponentView_);

		QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
		multiFinder->addFinder(instanceFinder);
		multiFinder->addFinder(topFinder);

        ExpressionFormatter instanceFormatter(multiFinder);

        if (portMap->getLogicalPort() && portMap->getLogicalPort()->range_)
        {
            bounds.first = instanceFormatter.formatReferringExpression(portMap->getLogicalPort()->range_->getLeft());
            bounds.second = instanceFormatter.formatReferringExpression(portMap->getLogicalPort()->range_->getRight());
        }
    }

    return bounds;
}

//-----------------------------------------------------------------------------
// Function: HDLParser::physicalBoundsInInstance()
//-----------------------------------------------------------------------------
QPair<QString, QString> HDLParser::physicalPortBoundsInInstance(QSharedPointer<View> activeView,
	QSharedPointer<Component> component, QSharedPointer<Port> port) const
{
	QPair<QString, QString> bounds("", "");

	if (component && topComponent_)
	{
		QSharedPointer<ComponentParameterFinder> instanceFinder(new ComponentParameterFinder(component));
		instanceFinder->setActiveView(activeView->name());
		QSharedPointer<TopComponentParameterFinder> topFinder(new TopComponentParameterFinder(topComponent_));
		topFinder->setActiveView(topComponentView_);

		QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
		multiFinder->addFinder(instanceFinder);
		multiFinder->addFinder(topFinder);

		ExpressionFormatter instanceFormatter(multiFinder);

		if (port)
		{
			bounds.first = instanceFormatter.formatReferringExpression(port->getLeftBound());
			bounds.second = instanceFormatter.formatReferringExpression(port->getRightBound());
		}
	}

	return bounds;
}

//-----------------------------------------------------------------------------
// Function: HDLParser::getComponentForInstance()
//-----------------------------------------------------------------------------
QSharedPointer<Component> HDLParser::getComponentForInstance(QString const& instanceName) const
{
    VLNV instanceVLNV = design_->getHWComponentVLNV(instanceName);
    return library_->getModel(instanceVLNV).dynamicCast<Component>();
}