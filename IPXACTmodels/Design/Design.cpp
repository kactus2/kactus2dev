//-----------------------------------------------------------------------------
// File: Design.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.08.2015
//
// Description:
// Describes the ipxact:design element in an IP-XACT document
//-----------------------------------------------------------------------------

#include "Design.h"

#include <IPXACTmodels/XmlUtils.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>
#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

//-----------------------------------------------------------------------------
// Function: Design::Design()
//-----------------------------------------------------------------------------
Design::Design(const VLNV &vlnv) :
Document(vlnv),
componentInstances_(new QList<QSharedPointer<ComponentInstance> > ()),
interconnections_(new QList<QSharedPointer<Interconnection> > ()),
monitorInterconnections_(new QList<QSharedPointer<MonitorInterconnection> > ()),
adHocConnections_(new QList<QSharedPointer<AdHocConnection> > ())
{

}

//-----------------------------------------------------------------------------
// Function: Design::Design()
//-----------------------------------------------------------------------------
Design::Design() :
Document(),
componentInstances_(new QList<QSharedPointer<ComponentInstance> > ()),
interconnections_(new QList<QSharedPointer<Interconnection> > ()),
monitorInterconnections_(new QList<QSharedPointer<MonitorInterconnection> > ()),
adHocConnections_(new QList<QSharedPointer<AdHocConnection> > ())
{

}

//-----------------------------------------------------------------------------
// Function: Design::Design()
//-----------------------------------------------------------------------------
Design::Design(Design const& other) :
Document(other),
componentInstances_(new QList<QSharedPointer<ComponentInstance> > ()),
interconnections_(new QList<QSharedPointer<Interconnection> > ()),
monitorInterconnections_(new QList<QSharedPointer<MonitorInterconnection> > ()),
adHocConnections_(new QList<QSharedPointer<AdHocConnection> > ())
{
    copySharedLists(other);
}

//-----------------------------------------------------------------------------
// Function: Design::operator=()
//-----------------------------------------------------------------------------
Design& Design::operator=( const Design& other )
{
	if (this != &other)
    {
		Document::operator=(other);

        componentInstances_->clear();
        interconnections_->clear();
        monitorInterconnections_->clear();
        adHocConnections_->clear();

        copySharedLists(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Design::~Design()
//-----------------------------------------------------------------------------
Design::~Design()
{
    componentInstances_.clear();
    interconnections_.clear();
    monitorInterconnections_.clear();
    adHocConnections_.clear();
}

//-----------------------------------------------------------------------------
// Function: Design::close()
//-----------------------------------------------------------------------------
QSharedPointer<Document> Design::clone() const
{
	return QSharedPointer<Document>(new Design(*this));
}

//-----------------------------------------------------------------------------
// Function: Design::getComponentInstances()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ComponentInstance> > > Design::getComponentInstances() const
{
	return componentInstances_;
}

//-----------------------------------------------------------------------------
// Function: Design::getInterconnections()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Interconnection> > > Design::getInterconnections() const
{
    return interconnections_;
}

//-----------------------------------------------------------------------------
// Function: Design::getAdHocConnections()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<AdHocConnection> > > Design::getAdHocConnections() const
{
    return adHocConnections_;
}

//-----------------------------------------------------------------------------
// Function: Design::setComponentInstances()
//-----------------------------------------------------------------------------
void Design::setComponentInstances(QSharedPointer<QList<QSharedPointer<ComponentInstance> > > newComponentInstances)
{
    componentInstances_ = newComponentInstances;
}

//-----------------------------------------------------------------------------
// Function: Design::setInterconnections()
//-----------------------------------------------------------------------------
void Design::setInterconnections(QSharedPointer<QList<QSharedPointer<Interconnection> > > newInterconnections)
{
    interconnections_ = newInterconnections;
}

//-----------------------------------------------------------------------------
// Function: Design::setAdHocConnections()
//-----------------------------------------------------------------------------
void Design::setAdHocConnections(QSharedPointer<QList<QSharedPointer<AdHocConnection> > > newAdHocConnections)
{
    adHocConnections_ = newAdHocConnections;
}

//-----------------------------------------------------------------------------
// Function: Design::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList Design::getDependentFiles() const
{
	// TODO implement this
	return QStringList();
}

//-----------------------------------------------------------------------------
// Function: Design::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> Design::getDependentVLNVs() const
{
	QList<VLNV> instanceList;

	// go through all component instances within the design
    foreach (QSharedPointer<ComponentInstance> instance, *componentInstances_)
    {
		// if the pointer is valid and it is not already added to the list of
		// component instances (two instances of same component are not added twice)
        QSharedPointer<ConfigurableVLNVReference> componentReference = instance->getComponentRef();

        if (componentReference->isValid() && !instanceList.contains(*componentReference))
        {
            instanceList.append(*componentReference);
        }
	}

    QList<QSharedPointer<VendorExtension> > swExtensions =
        getGroupedExtensionsByType("kactus2:swInstances", "kactus2:swInstance");

    foreach (QSharedPointer<VendorExtension> extension, swExtensions)
    {
        QSharedPointer<SWInstance> swInstance = extension.dynamicCast<SWInstance>();

        if (swInstance->getComponentRef()->isValid() && !instanceList.contains(*swInstance->getComponentRef()))
        {
            instanceList.append(*swInstance->getComponentRef());
        }
    }

	return instanceList;
}

//-----------------------------------------------------------------------------
// Function: Design::getPortAdHocVisibilities()
//-----------------------------------------------------------------------------
QMap<QString, bool> Design::getPortAdHocVisibilities() const
{
    QList<QSharedPointer<VendorExtension> > portAdHocExtensions =
        getGroupedExtensionsByType("kactus2:adHocVisibilities", "kactus2:adHocVisible");

    QMap<QString, bool> portAdHocVisibilities;

    foreach (QSharedPointer<VendorExtension> extension, portAdHocExtensions)
    {
        QSharedPointer<Kactus2Placeholder> portAdHocVisibility = extension.dynamicCast<Kactus2Placeholder>();
        
        QString portName = portAdHocVisibility->getAttributeValue("portName");

        portAdHocVisibilities.insert(portName, true);
    }

    return portAdHocVisibilities;
}

//-----------------------------------------------------------------------------
// Function: Design::getAdHocPortPositions()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> Design::getAdHocPortPositions() const
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:adHocVisibilities")
        {
            return extension;
        }
    }

    return QSharedPointer<VendorExtension>();
   // return getGroupedExtensionsByType("kactus2:adHocVisibilities", "kactus2:adHocVisible");
}

//-----------------------------------------------------------------------------
// Function: Design::getColumns()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ColumnDesc> > Design::getColumns() const
{
    QList<QSharedPointer<ColumnDesc> > columnList;

    QSharedPointer<Kactus2Group> columnExtensions = getLayoutExtension();
    if (!columnExtensions.isNull())
    {  
        foreach (QSharedPointer<VendorExtension> extension, columnExtensions->getByType("kactus2:column"))
        {
            columnList.append(extension.dynamicCast<ColumnDesc>());
        }
    }

    return columnList;
}

//-----------------------------------------------------------------------------
// Function: Design::getRoutes()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ConnectionRoute> > Design::getRoutes() const
{
    QList<QSharedPointer<ConnectionRoute> > routes;

    QSharedPointer<Kactus2Group> routeExtensions = getRoutesExtension();
    if (!routeExtensions.isNull())
    {  
        foreach (QSharedPointer<VendorExtension> extension, routeExtensions->getByType("kactus2:route"))
        {
            routes.append(extension.dynamicCast<ConnectionRoute>());
        }
    }

    return routes;
}

//-----------------------------------------------------------------------------
// Function: Design::setPortAdHocVisibilities()
//-----------------------------------------------------------------------------
void Design::setPortAdHocVisibilities(QMap<QString, bool> const& portAdHocVisibilities)
{
    /*QMap<QString, QPointF> adHocPortPositions = getAdHocPortPositions();

    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:adHocVisibilities")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!portAdHocVisibilities.isEmpty())
    {
        QSharedPointer<Kactus2Group> portAdHocs (new Kactus2Group("kactus2:adHocVisibilities"));
        
        QMapIterator<QString, bool> portIterator(portAdHocVisibilities);

        while (portIterator.hasNext())
        {
            portIterator.next();

            QSharedPointer<Kactus2Placeholder> newAdHocPort (new Kactus2Placeholder("kactus2:adHocVisible"));
            newAdHocPort->setAttribute("portName", portIterator.key());

            if (!adHocPortPositions.isEmpty())
            {
                QMapIterator<QString, QPointF> adHocPositionIterator(adHocPortPositions);
                while (adHocPositionIterator.hasNext())
                {
                    adHocPositionIterator.next();

                    if (portIterator.key() == adHocPositionIterator.key())
                    {
                        newAdHocPort->setAttribute("x", QString::number(int(adHocPositionIterator.value().x())));
                        newAdHocPort->setAttribute("y", QString::number(int(adHocPositionIterator.value().y())));
                    }
                }
            }

            portAdHocs->addToGroup(newAdHocPort);
        }

        getVendorExtensions()->append(portAdHocs);
    }*/
}

//-----------------------------------------------------------------------------
// Function: Design::setAdHocPortPositions()
//-----------------------------------------------------------------------------
void Design::setAdHocPortPositions(QMap<QString, QPointF> const& val)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:adHocVisibilities")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!val.isEmpty())
    {
        QSharedPointer<Kactus2Group> portAdHocs (new Kactus2Group("kactus2:adHocVisibilities"));

        QMapIterator<QString, QPointF> positionIterator(val);

        while (positionIterator.hasNext())
        {
            positionIterator.next();

            QSharedPointer<Kactus2Placeholder> newAdHocPort (new Kactus2Placeholder("kactus2:adHocVisible"));

            newAdHocPort->setAttribute("portName", positionIterator.key());
            newAdHocPort->setAttribute("x", QString::number(positionIterator.value().x()));
            newAdHocPort->setAttribute("y", QString::number(positionIterator.value().y()));

            portAdHocs->addToGroup(newAdHocPort);
        }

        getVendorExtensions()->append(portAdHocs);
    }
}

//-----------------------------------------------------------------------------
// Function: Design::setVlnv()
//-----------------------------------------------------------------------------
void Design::setVlnv(VLNV const& vlnv)
{
    VLNV designVLNV(vlnv);
    designVLNV.setType(VLNV::DESIGN);
    Document::setVlnv(vlnv);
}

//-----------------------------------------------------------------------------
// Function: Design::setApiConnections()
//-----------------------------------------------------------------------------
void Design::setApiConnections(QList<QSharedPointer<ApiInterconnection> > newApiConnections)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:apiConnections")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newApiConnections.isEmpty())
    {
        QSharedPointer<Kactus2Group> newApiConnectionGroup (new Kactus2Group("kactus2:apiConnections"));

        foreach (QSharedPointer<ApiInterconnection> connection, newApiConnections)
        {
            newApiConnectionGroup->addToGroup(connection);
        }

        getVendorExtensions()->append(newApiConnectionGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: Design::setComConnections()
//-----------------------------------------------------------------------------
void Design::setComConnections(QList<QSharedPointer<ComInterconnection> > newComConnections)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:comConnections")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newComConnections.isEmpty())
    {
        QSharedPointer<Kactus2Group> newComConnectionGroup (new Kactus2Group("kactus2:comConnections"));

        foreach (QSharedPointer<ComInterconnection> connection, newComConnections)
        {
            newComConnectionGroup->addToGroup(connection);
        }

        getVendorExtensions()->append(newComConnectionGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: Design::getSWInstances()
//-----------------------------------------------------------------------------
QList<QSharedPointer<SWInstance> > Design::getSWInstances() const
{
    QList<QSharedPointer<VendorExtension> > swExtensions =
        getGroupedExtensionsByType("kactus2:swInstances", "kactus2:swInstance");

    QList<QSharedPointer<SWInstance> > swInstanceList;

    foreach (QSharedPointer<VendorExtension> extension, swExtensions)
    {
        swInstanceList.append(extension.dynamicCast<SWInstance>());
    }

    return swInstanceList;
}

//-----------------------------------------------------------------------------
// Function: Design::setSWInstances()
//-----------------------------------------------------------------------------
void Design::setSWInstances(QList<QSharedPointer<SWInstance> > newSWInstances)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:swInstances")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newSWInstances.isEmpty())
    {
        QSharedPointer<Kactus2Group> newSwGroup (new Kactus2Group("kactus2:swInstances"));

        foreach(QSharedPointer<SWInstance> swInstance, newSWInstances)
        {
            newSwGroup->addToGroup(swInstance);
        }

        getVendorExtensions()->append(newSwGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: Design::getApiDependencies()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ApiInterconnection> > Design::getApiConnections() const
{
    QList<QSharedPointer<VendorExtension> > apiConnectionExtensions =
        getGroupedExtensionsByType("kactus2:apiConnections", "kactus2:apiConnection");

    QList<QSharedPointer<ApiInterconnection> > connectionList;

    foreach (QSharedPointer<VendorExtension> extension, apiConnectionExtensions)
    {
        connectionList.append(extension.dynamicCast<ApiInterconnection>());
    }

    return connectionList;
}

//-----------------------------------------------------------------------------
// Function: Design::getComConnections()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ComInterconnection> > Design::getComConnections() const
{
    QList<QSharedPointer<VendorExtension> > comConnectionExtensions =
        getGroupedExtensionsByType("kactus2:comConnections", "kactus2:comConnection");

    QList<QSharedPointer<ComInterconnection> > comConnectionList;

    foreach (QSharedPointer<VendorExtension> extension, comConnectionExtensions)
    {
        comConnectionList.append(extension.dynamicCast<ComInterconnection>());
    }

    return comConnectionList;
}

//-----------------------------------------------------------------------------
// Function: design::hasInterconnection()
//-----------------------------------------------------------------------------
bool Design::hasInterconnection(QString const& instanceName, QString const& interfaceName) const 
{
    foreach (QSharedPointer<Interconnection> interconnection, *interconnections_)
    {
        if (interconnection->hasInterfaceReferencingComponent(instanceName, interfaceName))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: design::getHWComponentVLNV()
//-----------------------------------------------------------------------------
VLNV Design::getHWComponentVLNV(QString const& instanceName) const 
{
	foreach (QSharedPointer<ComponentInstance> instance, *componentInstances_) 
    {
		// if instance is found
		if (instanceName.compare(instance->getInstanceName(), Qt::CaseInsensitive) == 0) 
        {
			//return instance.getComponentRef();
            return *instance->getComponentRef();
		}
	}

	// no instance was found
	return VLNV();
}

//-----------------------------------------------------------------------------
// Function: design::containsHWInstance()
//-----------------------------------------------------------------------------
bool Design::containsHWInstance(QString const& instanceName) const
{
    foreach (QSharedPointer<ComponentInstance> instance, *componentInstances_)
    {
		// if instance is found
        if (instanceName.compare(instance->getInstanceName(), Qt::CaseInsensitive) == 0)
        {
			return true;
		}
	}

	// no instance with given name was found
	return false;
}

//-----------------------------------------------------------------------------
// Function: design::hasConfElementValue()
//-----------------------------------------------------------------------------
bool Design::hasConfElementValue(QString const& instanceName, QString const& confElementName) const
{
	foreach (QSharedPointer<ComponentInstance> instance, *componentInstances_) 
    {
		// when the instance is found
        if (instanceName.compare(instance->getInstanceName(), Qt::CaseInsensitive) == 0)
        {
            return instance->getComponentRef()->hasConfigurableElementValue(confElementName);
		}
	}

	// specified instance was not found
	return false;
}

//-----------------------------------------------------------------------------
// Function: design::getConfElementValue()
//-----------------------------------------------------------------------------
QString Design::getConfElementValue(QString const& instanceName, QString const& confElementName) const
{
	foreach (QSharedPointer<ComponentInstance> instance, *componentInstances_)
    {
		// when the instance is found
        if (instanceName.compare(instance->getInstanceName(), Qt::CaseInsensitive) == 0)
        {
            return instance->getComponentRef()->getSingleConfigurableElementValue(confElementName);
		}
	}

	// specified instance was not found
	return QString();
}

//-----------------------------------------------------------------------------
// Function: design::setDesignImplementation()
//-----------------------------------------------------------------------------
void Design::setDesignImplementation(KactusAttribute::Implementation implementation)
{
    setImplementation(implementation);
}

//-----------------------------------------------------------------------------
// Function: design::getDesignImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation Design::getDesignImplementation() const
{
    return getImplementation();
}

//-----------------------------------------------------------------------------
// Function: design::getHWInstanceDescription()
//-----------------------------------------------------------------------------
QString Design::getHWInstanceDescription(QString const& instanceName) const
{
	foreach (QSharedPointer<ComponentInstance> instance, *componentInstances_)
    {
        if (instanceName.compare(instance->getInstanceName(), Qt::CaseInsensitive) == 0)
        {
            return instance->getDescription();
		}
	}

	return QString();
}

//-----------------------------------------------------------------------------
// Function: Design::getMonitorInterconnecions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<MonitorInterconnection> > > Design::getMonitorInterconnecions() const
{
    return monitorInterconnections_;
}

//-----------------------------------------------------------------------------
// Function: Design::setMonitorInterconnections()
//-----------------------------------------------------------------------------
void Design::setMonitorInterconnections(
    QSharedPointer<QList<QSharedPointer<MonitorInterconnection> > > newMonitorInterconnections)
{
    monitorInterconnections_ = newMonitorInterconnections;
}

//-----------------------------------------------------------------------------
// Function: Design::addColumn()
//-----------------------------------------------------------------------------
void Design::addColumn(QSharedPointer<ColumnDesc> column)
{
    QSharedPointer<Kactus2Group> layoutGroup = getLayoutExtension();

    if (layoutGroup.isNull())
    {
        layoutGroup = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:columnLayout"));
        getVendorExtensions()->append(layoutGroup);
    }
    
    layoutGroup->addToGroup(column);
}

//-----------------------------------------------------------------------------
// Function: Design::removeColumn()
//-----------------------------------------------------------------------------
void Design::removeColumn(QSharedPointer<ColumnDesc> column)
{
    QSharedPointer<Kactus2Group> layoutGroup = getLayoutExtension();
    if (!layoutGroup.isNull())
    {
        layoutGroup->removeFromGroup(column);
    }
}

//-----------------------------------------------------------------------------
// Function: Design::addRoute()
//-----------------------------------------------------------------------------
void Design::addRoute(QSharedPointer<ConnectionRoute> route)
{
    QSharedPointer<Kactus2Group> routesGroup = getRoutesExtension();

    if (routesGroup.isNull())
    {
        routesGroup = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:routes"));
        getVendorExtensions()->append(routesGroup);
    }

    routesGroup->addToGroup(route);
}

//-----------------------------------------------------------------------------
// Function: Design::removeRoute()
//-----------------------------------------------------------------------------
void Design::removeRoute(QSharedPointer<ConnectionRoute> route)
{
    QSharedPointer<Kactus2Group> routesGroup = getRoutesExtension();

    if (!routesGroup.isNull())
    {
        routesGroup->removeFromGroup(route);

        if (routesGroup->getByType("kactus2:route").isEmpty())
        {
            getVendorExtensions()->removeAll(routesGroup);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::copySharedLists()
//-----------------------------------------------------------------------------
void Design::copySharedLists(Design const& other)
{
    foreach (QSharedPointer<ComponentInstance> instance, *other.componentInstances_)
    {
        if (instance)
        {
            QSharedPointer<ComponentInstance> copy(new ComponentInstance(*instance));
            componentInstances_->append(copy);
        }
    }

    foreach (QSharedPointer<Interconnection> monitorConnection, *other.interconnections_)
    {
        if (monitorConnection)
        {
            QSharedPointer<Interconnection> interconnectionCopy(new Interconnection(*monitorConnection));
            interconnections_->append(interconnectionCopy);
        }
    }

    foreach (QSharedPointer<MonitorInterconnection> monitorConnection, *other.monitorInterconnections_)
    {
        if (monitorConnection)
        {
            QSharedPointer<MonitorInterconnection> monitorCopy(new MonitorInterconnection(*monitorConnection));
            monitorInterconnections_->append(monitorCopy);
        }
    }

    foreach (QSharedPointer<AdHocConnection> adHocConnection, *other.adHocConnections_)
    {
        if (adHocConnection)
        {
            QSharedPointer<AdHocConnection> copy(new AdHocConnection(*adHocConnection));
            adHocConnections_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::getLayoutExtension()
//-----------------------------------------------------------------------------
QSharedPointer<Kactus2Group> Design::getLayoutExtension() const
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:columnLayout")
        {
            return extension.dynamicCast<Kactus2Group>();
        }
    } 
    
    return QSharedPointer<Kactus2Group>();
}

//-----------------------------------------------------------------------------
// Function: Design::getRoutesExtension()
//-----------------------------------------------------------------------------
QSharedPointer<Kactus2Group> Design::getRoutesExtension() const
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:routes")
        {
            return extension.dynamicCast<Kactus2Group>();
        }
    } 

    return QSharedPointer<Kactus2Group>();
}

//-----------------------------------------------------------------------------
// Function: Design::getInterfacePositions()
//-----------------------------------------------------------------------------
QList<QSharedPointer<InterfaceGraphicsData> > Design::getInterfaceGraphicsData() const
{
    QList<QSharedPointer<InterfaceGraphicsData> > interfaceGrahics;

    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:interfaceGraphics")
        {
            QSharedPointer<InterfaceGraphicsData> graphicsData = extension.dynamicCast<InterfaceGraphicsData>();
            if (graphicsData)
            {
                interfaceGrahics.append(extension.dynamicCast<InterfaceGraphicsData>());
            }
        }
    }

    return interfaceGrahics;
}

//-----------------------------------------------------------------------------
// Function: Design::removeInterfacePosition()
//-----------------------------------------------------------------------------
void Design::removeInterfaceGraphicsData(QString const& name)
{
    foreach (QSharedPointer<InterfaceGraphicsData> containedInterface, getInterfaceGraphicsData())
    {
        if (name == containedInterface->getName())
        {
            getVendorExtensions()->removeAll(containedInterface);
            return;
        }
    }
}
