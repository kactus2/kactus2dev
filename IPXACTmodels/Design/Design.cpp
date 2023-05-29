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

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>
#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

#include <KactusAPI/include/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: Design::Design()
//-----------------------------------------------------------------------------
Design::Design(VLNV const& vlnv, Document::Revision revision) :
Document(vlnv, revision)
{

}

//-----------------------------------------------------------------------------
// Function: Design::Design()
//-----------------------------------------------------------------------------
Design::Design() :
Document()
{

}

//-----------------------------------------------------------------------------
// Function: Design::Design()
//-----------------------------------------------------------------------------
Design::Design(Design const& other) :
Document(other)
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
// Function: Design::close()
//-----------------------------------------------------------------------------
QSharedPointer<Document> Design::clone() const
{
	return QSharedPointer<Document>(new Design(*this));
}

//-----------------------------------------------------------------------------
// Function: Design::findComponentInstance()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> Design::findComponentInstance(std::string const& instanceName) const
{
    auto it = std::find_if(componentInstances_->cbegin(), componentInstances_->cend(),
        [&instanceName](auto const& instance) {return instance->getInstanceName() == instanceName; });

    if (it != componentInstances_->cend())
    {
        return *it;
    }

	return QSharedPointer<ComponentInstance>();
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
    for (auto const& instance : *componentInstances_)
    {
		// if the pointer is valid and it is not already added to the list of
		// component instances (two instances of same component are not added twice)
        QSharedPointer<ConfigurableVLNVReference> componentReference = instance->getComponentRef();

        if (componentReference->isValid() && !instanceList.contains(*componentReference))
        {
            instanceList.append(*componentReference);
        }
	}

	return instanceList;
}

//-----------------------------------------------------------------------------
// Function: Design::getPortAdHocVisibilities()
//-----------------------------------------------------------------------------
QMap<std::string, bool> Design::getPortAdHocVisibilities() const
{
    QList<QSharedPointer<VendorExtension> > portAdHocExtensions =
        getGroupedExtensionsByType("kactus2:adHocVisibilities", "kactus2:adHocVisible");

    QMap<std::string, bool> portAdHocVisibilities;

    for (auto const& extension : portAdHocExtensions)
    {
        QSharedPointer<Kactus2Placeholder> portAdHocVisibility = extension.dynamicCast<Kactus2Placeholder>();
        
        auto portName = portAdHocVisibility->getAttributeValue(std::string("portName"));

        portAdHocVisibilities.insert(portName, true);
    }

    return portAdHocVisibilities;
}

//-----------------------------------------------------------------------------
// Function: Design::getAdHocPortPositions()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> Design::getAdHocPortPositions() const
{
    return findVendorExtension("kactus2:adHocVisibilities");
}

//-----------------------------------------------------------------------------
// Function: Design::getColumns()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ColumnDesc> > Design::getColumns() const
{
    QList<QSharedPointer<ColumnDesc> > columnList;

    QSharedPointer<Kactus2Group> columnExtensions = getLayoutExtension();
    if (columnExtensions != nullptr)
    {  
        auto columnIdentifier = std::string("kactus2:column");
        for (auto const& extension : columnExtensions->getByType(columnIdentifier))
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
    if (routeExtensions != nullptr)
    {  
        auto routeIdentifier = std::string("kactus2:route");
        for (auto const& extension : routeExtensions->getByType(routeIdentifier))
        {
            routes.append(extension.dynamicCast<ConnectionRoute>());
        }
    }

    return routes;
}

//-----------------------------------------------------------------------------
// Function: Design::setAdHocPortPositions()
//-----------------------------------------------------------------------------
void Design::setAdHocPortPositions(QMap<std::string, QPointF> const& val)
{
    auto extension = findVendorExtension("kactus2:adHocVisibilities");
    getVendorExtensions()->removeAll(extension);

    if (!val.isEmpty())
    {
        QSharedPointer<Kactus2Group> portAdHocs(new Kactus2Group("kactus2:adHocVisibilities"));

        for (auto it = val.cbegin(); it != val.cend(); ++it)
        {
            QSharedPointer<Kactus2Placeholder> newAdHocPort(new Kactus2Placeholder("kactus2:adHocVisible"));

            newAdHocPort->setAttribute(QStringLiteral("portName"), QString::fromStdString(it.key()));
            newAdHocPort->setAttribute(QStringLiteral("x"), QString::number(it.value().x()));
            newAdHocPort->setAttribute(QStringLiteral("y"), QString::number(it.value().y()));

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
    auto extension = findVendorExtension("kactus2:apiConnections");
    getVendorExtensions()->removeAll(extension);

    if (!newApiConnections.isEmpty())
    {
        QSharedPointer<Kactus2Group> newApiConnectionGroup(new Kactus2Group("kactus2:apiConnections"));

        for (auto const& connection : newApiConnections)
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
    auto extension = findVendorExtension("kactus2:comConnections");
    getVendorExtensions()->removeAll(extension);

    if (!newComConnections.isEmpty())
    {
        QSharedPointer<Kactus2Group> newComConnectionGroup(new Kactus2Group("kactus2:comConnections"));

        for (auto const& connection : newComConnections)
        {
            newComConnectionGroup->addToGroup(connection);
        }

        getVendorExtensions()->append(newComConnectionGroup);
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

    for (auto const& extension : apiConnectionExtensions)
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

    for (auto const& extension : comConnectionExtensions)
    {
        comConnectionList.append(extension.dynamicCast<ComInterconnection>());
    }

    return comConnectionList;
}

//-----------------------------------------------------------------------------
// Function: design::hasInterconnection()
//-----------------------------------------------------------------------------
bool Design::hasInterconnection(std::string const& instanceName, std::string const& interfaceName) const
{
    return std::any_of(interconnections_->cbegin(), interconnections_->cend(), 
        [&instanceName, &interfaceName](auto const& interconnection)
    {return interconnection->hasInterfaceReferencingComponent(instanceName, interfaceName); });
}

//-----------------------------------------------------------------------------
// Function: design::getHWComponentVLNV()
//-----------------------------------------------------------------------------
VLNV Design::getHWComponentVLNV(std::string const& instanceName) const 
{
    if (auto const instance = findComponentInstance(instanceName); instance != nullptr)
    {
        return *instance->getComponentRef();
    }

	// no instance was found
	return VLNV();
}

//-----------------------------------------------------------------------------
// Function: design::containsHWInstance()
//-----------------------------------------------------------------------------
bool Design::containsHWInstance(std::string const& instanceName) const
{
    return findComponentInstance(instanceName) != nullptr;
}

//-----------------------------------------------------------------------------
// Function: design::hasConfElementValue()
//-----------------------------------------------------------------------------
bool Design::hasConfElementValue(std::string const& instanceName, std::string const& confElementName) const
{
    if (auto const instance = findComponentInstance(instanceName); instance != nullptr)
    {
        return instance->getComponentRef()->hasConfigurableElementValue(QString::fromStdString(confElementName));
    }

	// specified instance was not found
	return false;
}

//-----------------------------------------------------------------------------
// Function: design::getConfElementValue()
//-----------------------------------------------------------------------------
std::string Design::getConfElementValue(std::string const& instanceName, std::string const& confElementName) const
{
    if (auto const instance = findComponentInstance(instanceName); instance != nullptr)
    {
        return instance->getComponentRef()->getSingleConfigurableElementValue(
            QString::fromStdString(confElementName)).toStdString();
    }

	// specified instance was not found
	return std::string();
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
std::string Design::getHWInstanceDescription(std::string const& instanceName) const
{
    if (auto const instance = findComponentInstance(instanceName); instance != nullptr)
    {
        return instance->description().toStdString();
    }

	return std::string();
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
    for (auto const& instance : *other.componentInstances_)
    {
        if (instance)
        {
            QSharedPointer<ComponentInstance> copy(new ComponentInstance(*instance));
            componentInstances_->append(copy);
        }
    }

    for (auto const& monitorConnection : *other.interconnections_)
    {
        if (monitorConnection)
        {
            QSharedPointer<Interconnection> interconnectionCopy(new Interconnection(*monitorConnection));
            interconnections_->append(interconnectionCopy);
        }
    }

    for (auto const& monitorConnection : *other.monitorInterconnections_)
    {
        if (monitorConnection)
        {
            QSharedPointer<MonitorInterconnection> monitorCopy(new MonitorInterconnection(*monitorConnection));
            monitorInterconnections_->append(monitorCopy);
        }
    }

    for (auto const& adHocConnection : *other.adHocConnections_)
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
    return findVendorExtension("kactus2:columnLayout").dynamicCast<Kactus2Group>();
}

//-----------------------------------------------------------------------------
// Function: Design::getRoutesExtension()
//-----------------------------------------------------------------------------
QSharedPointer<Kactus2Group> Design::getRoutesExtension() const
{
    return findVendorExtension("kactus2:routes").dynamicCast<Kactus2Group>();
}

//-----------------------------------------------------------------------------
// Function: Design::getInterfacePositions()
//-----------------------------------------------------------------------------
QList<QSharedPointer<InterfaceGraphicsData> > Design::getInterfaceGraphicsData() const
{
    QList<QSharedPointer<InterfaceGraphicsData> > interfaceGrahics;

    for (auto const& extension : *getVendorExtensions())
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
void Design::removeInterfaceGraphicsData(std::string const& name)
{
    for (auto const& containedInterface : getInterfaceGraphicsData())
    {
        if (name == containedInterface->getName())
        {
            getVendorExtensions()->removeAll(containedInterface);
            return;
        }
    }
}
