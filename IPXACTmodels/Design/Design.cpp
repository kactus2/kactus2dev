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
QSharedPointer<Document> Design::clone() const {
	return QSharedPointer<Document>(new Design(*this));
}

/*
bool Design::isValid( QStringList& errorList ) const {
	bool valid = true;
	QString thisIdentifier(QObject::tr("containing design"));

	if (!vlnv_) {
		errorList.append(QObject::tr("No vlnv specified for the design."));
		valid = false;
	}
	else if (!vlnv_->isValid(errorList, thisIdentifier)) {
		valid = false;
	}
	else {
		thisIdentifier = QObject::tr("design %1").arg(vlnv_->toString());
	}

	QStringList instanceNames;
    QStringList hwUUIDs;

	foreach (ComponentInstance const& instance, componentInstances_)
    {		
        hwUUIDs.append(instance.getUuid());

		// if there are several instances with same name
		if (instanceNames.contains(instance.getInstanceName())) {
			errorList.append(QObject::tr("Design contains several instances with"
				" name %1").arg(instance.getInstanceName()));
			valid = false;
		}
		else {
			instanceNames.append(instance.getInstanceName());
		}

		if (!instance.isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

    QStringList swInstanceNames;

    foreach (SWInstance const& instance, swInstances_)
    {
        if (swInstanceNames.contains(instance.getInstanceName()))
        {
            errorList.append(QObject::tr("Design contains several SW instances "
                                         "with name %1").arg(instance.getInstanceName()));
            valid = false;
        }
        else
        {
            swInstanceNames.append(instance.getInstanceName());
        }

        if (!instance.isValid(errorList, hwUUIDs, thisIdentifier))
        {
            valid = false;
        }
    }

	QStringList interconnectionNames;
	foreach (Interconnection interconnection, interconnections_) {
		
		// if there are several interconnections with same name
		if (interconnectionNames.contains(interconnection.name())) {
			errorList.append(QObject::tr("Design contains several interconnections"
				" with name '%1'").arg(interconnection.name()));
			valid = false;
		}
		else {
			interconnectionNames.append(interconnection.name());
		}

		if (!interconnection.isValid(instanceNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

    QStringList comConnectionNames;
    QStringList allInstanceNames = instanceNames + swInstanceNames;

    foreach (ComConnection const& conn, comConnections_)
    {
        if (comConnectionNames.contains(conn.name()))
        {
            errorList.append(QObject::tr("Design contains several COM connections "
                                         "with name '%1'").arg(conn.name()));
            valid = false;
        }
        else
        {
            comConnectionNames.append(conn.name());
        }

        if (!conn.isValid(errorList, allInstanceNames, thisIdentifier))
        {
            valid = false;
        }
    }

	QStringList hierComNames;
	foreach (const HierComConnection& hierComm, hierComConnections_) {
		if (hierComNames.contains(hierComm.name())) {
			errorList.append(QObject::tr("Design contains several hierarchical COM "
				"connections with name %1").arg(hierComm.name()));
			valid = false;
		}
		else {
			hierComNames.append(hierComm.name());
		}

		if (!hierComm.isValid(errorList, allInstanceNames, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList apiDepNames;
	foreach (const ApiConnection& apiDep, apiDependencies_) {
		if (apiDepNames.contains(apiDep.name())) {
			errorList.append(QObject::tr("Design contains several API dependencies"
				" with name %1").arg(apiDep.name()));
			valid = false;
		}
		else {
			apiDepNames.append(apiDep.name());
		}

		if (!apiDep.isValid(errorList, allInstanceNames, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList hierAPINames;
	foreach (const HierApiDependency& hierAPI, hierApiDependencies_) {
		if (hierAPINames.contains(hierAPI.name())) {
			errorList.append(QObject::tr("Design contains several hierarchical "
				"API dependencies with name %1").arg(hierAPI.name()));
			valid = false;
		}
		else {
			hierAPINames.append(hierAPI.name());
		}

		if (!hierAPI.isValid(errorList, allInstanceNames, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList adHocNames;
	foreach (AdHocConnection adHoc, adHocConnections_) {
		if (adHocNames.contains(adHoc.name())) {
			errorList.append(QObject::tr("Design contains several ad hoc connections"
				" with name %1").arg(adHoc.name()));
			valid = false;
		}
		else {
			adHocNames.append(adHoc.name());
		}

		if (!adHoc.isValid(instanceNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	foreach (HierConnection hierConn, hierConnections_) {
		if (!hierConn.isValid(instanceNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}*/
/*
bool Design::isValid() const {
	if (!vlnv_) {
		return false;
	}
	else if (!vlnv_->isValid()) {
		return false;
	}

	QStringList instanceNames;
    QStringList hwUUIDs;

	foreach (ComponentInstance const& instance, componentInstances_)
    {
        hwUUIDs.append(instance.getUuid());

		// if there are several instances with same name
		if (instanceNames.contains(instance.getInstanceName())) {
			return false;
		}
		else {
			instanceNames.append(instance.getInstanceName());
		}

		if (!instance.isValid()) {
			return false;
		}
	}

	// check the SW instances
	QStringList swInstanceNames;
	foreach (const SWInstance& swInstance, swInstances_) {
		
		if (swInstanceNames.contains(swInstance.getInstanceName())) {
			return false;
		}
		else {
			swInstanceNames.append(swInstance.getInstanceName());
		}

		if (!swInstance.isValid(hwUUIDs)) {
			return false;
		}
	}

	QStringList interconnectionNames;
	foreach (Interconnection interconnection, interconnections_) {

		// if there are several interconnections with same name
		if (interconnectionNames.contains(interconnection.name())) {
			return false;
		}
		else {
			interconnectionNames.append(interconnection.name());
		}

		if (!interconnection.isValid(instanceNames)) {
			return false;
		}
	}

	QStringList adHocNames;
	foreach (AdHocConnection adHoc, adHocConnections_) {
		if (adHocNames.contains(adHoc.name())) {
			return false;
		}
		else {
			adHocNames.append(adHoc.name());
		}

		if (!adHoc.isValid(instanceNames)) {
			return false;
		}
	}

	foreach (HierConnection hierConn, hierConnections_) {
		if (!hierConn.isValid(instanceNames)) {
			return false;
		}
	}

	QStringList comConnectionNames;
	QStringList allInstanceNames = instanceNames + swInstanceNames;

	foreach (ComConnection const& conn, comConnections_)
	{
		if (comConnectionNames.contains(conn.name())) {
			return false;
		}
		else {
			comConnectionNames.append(conn.name());
		}

		if (!conn.isValid(allInstanceNames)) {
			return false;
		}
	}

	QStringList hierComNames;
	foreach (const HierComConnection& hierComm, hierComConnections_) {
		if (hierComNames.contains(hierComm.name())) {
			return false;
		}
		else {
			hierComNames.append(hierComm.name());
		}

		if (!hierComm.isValid(allInstanceNames)) {
			return false;
		}
	}

	QStringList apiDepNames;
	foreach (const ApiConnection& apiDep, apiDependencies_) {
		if (apiDepNames.contains(apiDep.name())) {
			return false;
		}
		else {
			apiDepNames.append(apiDep.name());
		}

		if (!apiDep.isValid(allInstanceNames)) {
			return false;
		}
	}

	QStringList hierAPINames;
	foreach (const HierApiDependency& hierAPI, hierApiDependencies_) {
		if (hierAPINames.contains(hierAPI.name())) {
			return false;
		}
		else {
			hierAPINames.append(hierAPI.name());
		}

		if (!hierAPI.isValid(allInstanceNames)) {
			return false;
		}
	}

	return true;
}*/

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

QStringList Design::getDependentFiles() const
{
	// TODO implement this
	return QStringList();
}

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
// Function: parseVendorExtensions()
//-----------------------------------------------------------------------------
void Design::parseVendorExtensions(QDomNode &node)
{
	QDomNodeList childNodes = node.childNodes();

    for (int i = 0; i < childNodes.size(); i++)
    {
        QDomNode childNode = childNodes.at(i);

        // Compatibility note: 
        // Kactus extensions are found under kactus:extensions or spirit:vendorExtensions.
        if (childNode.nodeName() == "kactus2:extensions")
        {            
            parseVendorExtensions(childNode);
        }

        // Otherwise read the interconnection routes if they were found.
        else if (childNode.nodeName() == "kactus2:routes")
        {
            parseRoutes(childNode);
        }
        else if (childNode.nodeName() == "kactus2:note")
        {
            //vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(childNode)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::parseRoutes()
//-----------------------------------------------------------------------------
void Design::parseRoutes(QDomNode& routesNode)
{
    QDomNodeList connNodes = routesNode.childNodes();

    for (int i = 0; i < connNodes.size(); ++i)
    {
        QDomNode connNode = connNodes.at(i);

        if (connNode.nodeName() == "kactus2:route")
        {
            parseRoute(connNode);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::parseRoute()
//-----------------------------------------------------------------------------
void Design::parseRoute(QDomNode& routeNode)
{
    QString name = routeNode.attributes().namedItem("kactus2:connRef").nodeValue();
    QString offPageValue = routeNode.attributes().namedItem("kactus2:offPage").nodeValue();
    QList<QPointF> route;

    // Parse the route.
    for (int i = 0; i < routeNode.childNodes().size(); ++i)
    {
        QDomNode posNode = routeNode.childNodes().at(i);

        if (posNode.nodeName() == "kactus2:position")
        {
            route.append(XmlUtils::parsePoint(posNode));
        }
    }

    // Apply the route to the correct interconnection or ad-hoc connection.
    bool offPage = offPageValue == "true";
    bool found = false;

    /*
    for (int i = 0; i < interconnections_.size() && !found; ++i)
    {
        if (interconnections_[i].name() == name)
        {
            interconnections_[i].setRoute(route);
            interconnections_[i].setOffPage(offPage);
            found = true;
        }
    }*/

    /*
    for (int i = 0; i < adHocConnections_.size() && !found; ++i)
    {
        if (adHocConnections_[i].name() == name)
        {
            adHocConnections_[i].setRoute(route);
            adHocConnections_[i].setOffPage(offPage);
        }
    }*/

    for (int i = 0; i < adHocConnections_->size() && !found; ++i)
    {
        if (adHocConnections_->at(i)->name() == name)
        {
            adHocConnections_->at(i)->setRoute(route);
            adHocConnections_->at(i)->setOffPage(offPage);
            found = true;
        }
    }
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
QMap<QString, QPointF> Design::getAdHocPortPositions() const
{
    QList<QSharedPointer<VendorExtension> > portAdHocExtensions =
        getGroupedExtensionsByType("kactus2:adHocVisibilities", "kactus2:adHocVisible");

    QMap<QString, QPointF> portAdHocPositions;

    foreach (QSharedPointer<VendorExtension> extension, portAdHocExtensions)
    {
        QSharedPointer<Kactus2Placeholder> portPosition = extension.dynamicCast<Kactus2Placeholder>();

        QString portName = portPosition->getAttributeValue("portName");
        int positionX = portPosition->getAttributeValue("x").toInt();
        int positionY = portPosition->getAttributeValue("y").toInt();

        QPointF newPosition(positionX, positionY);
        portAdHocPositions.insert(portName, newPosition);
    }

    return portAdHocPositions;
}

//-----------------------------------------------------------------------------
// Function: Design::getColumns()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ColumnDesc> > Design::getColumns() const
{
    QList<QSharedPointer<VendorExtension> > columnExtensions =
        getGroupedExtensionsByType("kactus2:columnLayout", "kactus2:column");

    QList<QSharedPointer<ColumnDesc> > columnList;
    foreach (QSharedPointer<VendorExtension> extension, columnExtensions)
    {
        columnList.append(extension.dynamicCast<ColumnDesc>());
    }

    return columnList;
}

//-----------------------------------------------------------------------------
// Function: Design::setPortAdHocVisibilities()
//-----------------------------------------------------------------------------
void Design::setPortAdHocVisibilities(QMap<QString, bool> const& portAdHocVisibilities)
{
    QMap<QString, QPointF> adHocPortPositions = getAdHocPortPositions();

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
    }
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
// Function: Design::setColumns()
//-----------------------------------------------------------------------------
void Design::setColumns(QList<QSharedPointer<ColumnDesc> > newColumns)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:columnLayout")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newColumns.isEmpty())
    {
        QSharedPointer<Kactus2Group> newColumnLayout (new Kactus2Group("kactus2:columnLayout"));

        foreach(QSharedPointer<ColumnDesc> columnExtension, newColumns)
        {
            newColumnLayout->addToGroup(columnExtension);
        }

        getVendorExtensions()->append(newColumnLayout);
    }
}

//-----------------------------------------------------------------------------
// Function: Design::getComponents()
//-----------------------------------------------------------------------------
QList<VLNV> Design::getComponents() const
{
	QList<VLNV> list;

    foreach(QSharedPointer<ComponentInstance> instance, *componentInstances_)
    {
        if (!instance->isDraft())
        {
            list.append(*instance->getComponentRef());
        }
	}

    QList<QSharedPointer<VendorExtension> > swExtensions =
        getGroupedExtensionsByType("kactus2:swInstances", "kactus2:swInstance");

    foreach (QSharedPointer<VendorExtension> extensions, swExtensions)
    {
        QSharedPointer<SWInstance> swInstance = extensions.dynamicCast<SWInstance>();
        if (swInstance->isDraft())
        {
            list.append(*swInstance->getComponentRef());
        }
    }

	return list;
}

//-----------------------------------------------------------------------------
// Function: Design::setVlnv()
//-----------------------------------------------------------------------------
void Design::setVlnv( const VLNV& vlnv )
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
// Function: Design::setHierApiDependencies()
//-----------------------------------------------------------------------------
void Design::setHierApiDependencies(QList<QSharedPointer<HierApiInterconnection> > newHierApiDependencies)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:hierApiDependencies")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newHierApiDependencies.isEmpty())
    {
        QSharedPointer<Kactus2Group> newHierApiGroup (new Kactus2Group("kactus2:hierApiDependencies"));

        foreach (QSharedPointer<HierApiInterconnection> dependency, newHierApiDependencies)
        {
            newHierApiGroup->addToGroup(dependency);
        }

        getVendorExtensions()->append(newHierApiGroup);
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
// Function: Design::setHierComConnections()
//-----------------------------------------------------------------------------
void Design::setHierComConnections(QList<QSharedPointer<HierComInterconnection> > newHierComConnections)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:hierComConnections")
        {
            getVendorExtensions()->removeAll(extension);
            break;
        }
    }

    if (!newHierComConnections.isEmpty())
    {
        QSharedPointer<Kactus2Group> newHierComConnectionGroup (new Kactus2Group("kactus2:hierComConnections"));

        foreach (QSharedPointer<HierComInterconnection> connection, newHierComConnections)
        {
            newHierComConnectionGroup->addToGroup(connection);
        }

        getVendorExtensions()->append(newHierComConnectionGroup);
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
// Function: Design::getHierApiDependencies()
//-----------------------------------------------------------------------------
QList<QSharedPointer<HierApiInterconnection> > Design::getHierApiDependencies() const
{
    QList<QSharedPointer<VendorExtension> > hierApiDependencyExtensions =
        getGroupedExtensionsByType("kactus2:hierApiDependencies", "kactus2:hierApiDependency");

    QList<QSharedPointer<HierApiInterconnection> > dependencyList;

    foreach (QSharedPointer<VendorExtension> extension, hierApiDependencyExtensions)
    {
        dependencyList.append(extension.dynamicCast<HierApiInterconnection>());
    }

    return dependencyList;
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
// Function: Design::getHierComConnections()
//-----------------------------------------------------------------------------
QList<QSharedPointer<HierComInterconnection> > Design::getHierComConnections() const
{
    QList<QSharedPointer<VendorExtension> > hierComConnectionExtensions =
        getGroupedExtensionsByType("kactus2:hierComConnections", "kactus2:hierComConnection");

    QList<QSharedPointer<HierComInterconnection> > hierComConnectionList;

    foreach (QSharedPointer<VendorExtension> extension, hierComConnectionExtensions)
    {
        hierComConnectionList.append(extension.dynamicCast<HierComInterconnection>());
    }

    return hierComConnectionList;
}

//-----------------------------------------------------------------------------
// Function: design::hasInterconnection()
//-----------------------------------------------------------------------------
bool Design::hasInterconnection( const QString& instanceName, const QString& interfaceName ) const 
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

/*
//-----------------------------------------------------------------------------
// Function: design::getConnectedInterfaces()
//-----------------------------------------------------------------------------
QList<Interface> Design::getConnectedInterfaces( const QString& instanceName, const QString& interfaceName ) const
{
	Interface interface(instanceName, interfaceName);
	return getConnectedInterfaces(interface);
}*/

/*
//-----------------------------------------------------------------------------
// Function: design::getConnectedInterfaces()
//-----------------------------------------------------------------------------
QList<Interface> Design::getConnectedInterfaces( const Interface& sourceInterface ) const 
{
	QList<Interface> interfaces;

	foreach (Interconnection conn, interconnections_) {
        QPair<Interface, Interface> connectionInterfaces = conn.getInterfaces();

		if (connectionInterfaces.first == sourceInterface) {
			interfaces.append(connectionInterfaces.second);
		}
		else if (connectionInterfaces.second == sourceInterface) {
			interfaces.append(connectionInterfaces.first);
		}
	}

	return interfaces;
}*/

//-----------------------------------------------------------------------------
// Function: design::getHWComponentVLNV()
//-----------------------------------------------------------------------------
VLNV Design::getHWComponentVLNV( const QString& instanceName ) const 
{
	foreach (QSharedPointer<ComponentInstance> instance, *componentInstances_) 
    {
		// if instance is found
		if (0 == instanceName.compare(instance->getInstanceName(), Qt::CaseInsensitive)) 
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
bool Design::containsHWInstance( const QString& instanceName ) const
{
    foreach (QSharedPointer<ComponentInstance> instance, *componentInstances_)
    {
		// if instance is found
        if (0 == instanceName.compare(instance->getInstanceName(), Qt::CaseInsensitive))
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
bool Design::hasConfElementValue( const QString& instanceName, const QString& confElementName ) const
{
	foreach (QSharedPointer<ComponentInstance> instance, *componentInstances_) 
    {
		// when the instance is found
        if (0 == instanceName.compare(instance->getInstanceName(), Qt::CaseInsensitive))
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
QString Design::getConfElementValue( const QString& instanceName, const QString& confElementName ) const
{
	foreach (QSharedPointer<ComponentInstance> instance, *componentInstances_)
    {
		// when the instance is found
        if (0 == instanceName.compare(instance->getInstanceName(), Qt::CaseInsensitive))
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
QString Design::getHWInstanceDescription( const QString& instanceName ) const
{
	foreach (QSharedPointer<ComponentInstance> instance, *componentInstances_)
    {
        if (0 == instanceName.compare(instance->getInstanceName(), Qt::CaseInsensitive))
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
// Function: Design::copySharedLists()
//-----------------------------------------------------------------------------
void Design::copySharedLists(Design const& other)
{
    foreach (QSharedPointer<ComponentInstance> instance, *other.componentInstances_)
    {
        if (instance)
        {
            QSharedPointer<ComponentInstance> copy =
                QSharedPointer<ComponentInstance>(new ComponentInstance(*instance.data()));
            componentInstances_->append(copy);
        }
    }

    foreach (QSharedPointer<Interconnection> interconnection, *other.interconnections_)
    {
        if (interconnection)
        {
            QSharedPointer<Interconnection> copy =
                QSharedPointer<Interconnection> (new Interconnection(*interconnection.data()));
            interconnections_->append(copy);
        }
    }

    foreach (QSharedPointer<MonitorInterconnection> interconnection, *other.monitorInterconnections_)
    {
        if (interconnection)
        {
            QSharedPointer<MonitorInterconnection> copy =
                QSharedPointer<MonitorInterconnection> (new MonitorInterconnection(*interconnection.data()));
            monitorInterconnections_->append(copy);
        }
    }

    foreach (QSharedPointer<AdHocConnection> connection, *other.adHocConnections_)
    {
        if (connection)
        {
            QSharedPointer<AdHocConnection> copy =
                QSharedPointer<AdHocConnection>(new AdHocConnection(*connection.data()));
            adHocConnections_->append(copy);
        }
    }
}
