//-----------------------------------------------------------------------------
// File: DesignReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.08.2015
//
// Description:
// XML reader class for IP-XACT design element.
//-----------------------------------------------------------------------------

#include "DesignReader.h"
#include "ComponentInstanceReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>

#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

#include <IPXACTmodels/utilities/XmlUtils.h>

//-----------------------------------------------------------------------------
// Function: DesignReader::DesignReader()
//-----------------------------------------------------------------------------
DesignReader::DesignReader(): DocumentReader()
{

}

//-----------------------------------------------------------------------------
// Function: DesignReader::~DesignReader()
//-----------------------------------------------------------------------------
DesignReader::~DesignReader()
{

}

//-----------------------------------------------------------------------------
// Function: DesignReader::createDesignFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Design> DesignReader::createDesignFrom(QDomDocument const& document) const
{
    QSharedPointer<Design> newDesign (new Design);

    parseTopComments(document, newDesign);

    parseXMLProcessingInstructions(document, newDesign);

    QDomNode designNode = document.firstChildElement();
    parseNamespaceDeclarations(designNode, newDesign);

    parseVLNVElements(designNode, newDesign, VLNV::DESIGN);

    parseComponentInstances(designNode, newDesign);

    parseInterconnections(designNode, newDesign);

    parseAdHocConnections(designNode, newDesign);

    parseDescription(designNode, newDesign);

    parseParameters(designNode, newDesign);

    parseAssertions(designNode, newDesign);

    parseDesignExtensions(designNode, newDesign);

    return newDesign;
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseComponentInstances()
//-----------------------------------------------------------------------------
void DesignReader::parseComponentInstances(QDomNode const& designNode, QSharedPointer<Design> newDesign) const
{
    QDomNode componentInstancesNode = designNode.firstChildElement(QStringLiteral("ipxact:componentInstances"));

    QDomNodeList instanceNodes = componentInstancesNode.childNodes();
    ComponentInstanceReader instanceReader;

    for (int i = 0; i < instanceNodes.size(); ++i)
    {
        QDomNode singleInstanceNode = instanceNodes.at(i);

        QSharedPointer<ComponentInstance> newComponentInstance =
            instanceReader.createComponentInstanceFrom(singleInstanceNode);

        newDesign->getComponentInstances()->append(newComponentInstance);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseInterconnections()
//-----------------------------------------------------------------------------
void DesignReader::parseInterconnections(QDomNode const& designNode, QSharedPointer<Design> newDesign) const
{
    QDomNode multipleInterconnectionsNode = designNode.firstChildElement(QStringLiteral("ipxact:interconnections"));

    QDomNodeList interconnectionNodes = multipleInterconnectionsNode.childNodes();

    for (int interconnectionIndex = 0; interconnectionIndex < interconnectionNodes.size(); ++interconnectionIndex)
    {
        QDomNode singleInterconnectionNode = interconnectionNodes.at(interconnectionIndex);
        parseSingleInterconnection(singleInterconnectionNode, newDesign);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseSingleInterconnection()
//-----------------------------------------------------------------------------
void DesignReader::parseSingleInterconnection(QDomNode const& interconnectionNode,
    QSharedPointer<Design> newDesign) const
{
    if (interconnectionNode.nodeName() == QStringLiteral("ipxact:interconnection"))
    {
        parseComponentInterconnection(interconnectionNode, newDesign);
    }
    else if (interconnectionNode.nodeName() == QStringLiteral("ipxact:monitorInterconnection"))
    {
        parseMonitorInterconnection(interconnectionNode, newDesign);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseComponentInerconnection()
//-----------------------------------------------------------------------------
void DesignReader::parseComponentInterconnection(QDomNode const& interconnectionNode,
    QSharedPointer<Design> newDesign) const
{
    NameGroupReader nameReader;
    QString name = nameReader.parseName(interconnectionNode);
    QString displayName = nameReader.parseDisplayName(interconnectionNode);
    QString description = nameReader.parseDescription(interconnectionNode);
    QString isPresent = interconnectionNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();

    QSharedPointer<ActiveInterface> startInterface (new ActiveInterface);
    QDomNode startInterfaceNode = interconnectionNode.firstChildElement(QStringLiteral("ipxact:activeInterface"));
    parseActiveInterface(startInterfaceNode, startInterface);

    QSharedPointer<Interconnection> newInterconnection (
        new Interconnection(name, startInterface, displayName, description));
    newInterconnection->setIsPresent(isPresent);

    QDomNodeList interfaceNodes = interconnectionNode.childNodes();

    for (int interfaceIndex = 0; interfaceIndex < interfaceNodes.size(); ++interfaceIndex)
    {
        QDomNode singleInterfaceNode = interfaceNodes.at(interfaceIndex);
        if (singleInterfaceNode.nodeName() == QLatin1String("ipxact:activeInterface") &&
            singleInterfaceNode != startInterfaceNode)
        {
            QSharedPointer<ActiveInterface> activeInterface (new ActiveInterface());
            parseActiveInterface(singleInterfaceNode, activeInterface);

            newInterconnection->getActiveInterfaces()->append(activeInterface);
        }
        else if (singleInterfaceNode.nodeName() == QLatin1String("ipxact:hierInterface"))
        {
            QSharedPointer<HierInterface> hierInterface (new HierInterface());
            parseHierInterface(singleInterfaceNode, hierInterface);

            newInterconnection->getHierInterfaces()->append(hierInterface);
        }
    }

    parseInterconnectionExtensions(interconnectionNode, newInterconnection);

    newDesign->getInterconnections()->append(newInterconnection);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseComponentInterconnectionExtensions()
//-----------------------------------------------------------------------------
void DesignReader::parseInterconnectionExtensions(const QDomNode& interconnectionNode,
    QSharedPointer<Interconnection> interconnection) const
{
    QDomNode extensionNode = interconnectionNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions")); 
    QDomElement offPageElement = extensionNode.firstChildElement(QStringLiteral("kactus2:offPage"));

    if (!offPageElement.isNull())
    {
        interconnection->setOffPage(true);
    }

    parseVendorExtensions(interconnectionNode, interconnection);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseActiveInterface()
//-----------------------------------------------------------------------------
void DesignReader::parseActiveInterface(QDomNode const& interfaceNode,
    QSharedPointer<ActiveInterface> newInterface) const
{
    parseHierInterface(interfaceNode, newInterface);

    QDomNamedNodeMap attributeMap = interfaceNode.attributes();

    newInterface->setComponentReference(attributeMap.namedItem(QStringLiteral("componentRef")).nodeValue());

    QDomNode baseExcludePortsNode = interfaceNode.firstChildElement(QStringLiteral("ipxact:excludePorts"));
    QDomNodeList excludePortNodes = baseExcludePortsNode.childNodes();

    for (int i = 0; i < excludePortNodes.size(); ++i)
    {
        QString portName = excludePortNodes.at(i).firstChild().nodeValue();
        newInterface->getExcludePorts()->append(portName);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseHierInterface()
//-----------------------------------------------------------------------------
void DesignReader::parseHierInterface(QDomNode const& interfaceNode, QSharedPointer<HierInterface> newInterface)
    const
{
    QDomNamedNodeMap attributeMap = interfaceNode.attributes();
    
    newInterface->setBusReference(attributeMap.namedItem(QStringLiteral("busRef")).nodeValue());
    newInterface->setIsPresent(interfaceNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());
    newInterface->setDescription(interfaceNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue());

    parseHierInterfaceExtensions(interfaceNode, newInterface);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseHierInterfaceExtensions()
//-----------------------------------------------------------------------------
void DesignReader::parseHierInterfaceExtensions(const QDomNode& interfaceNode,
    QSharedPointer<HierInterface> newInterface) const
{
    QDomNode extensionsNode = interfaceNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));

    QDomElement routeElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:route"));
    if (!routeElement.isNull())
    {
        QList<QPointF> newRoute;

        QDomNodeList positionNodesList = routeElement.elementsByTagName(QStringLiteral("kactus2:position"));

        for (int positionIndex = 0; positionIndex < positionNodesList.count(); ++positionIndex)
        {
            QDomNode positionNode = positionNodesList.at(positionIndex);
            QDomNamedNodeMap positionAttributes = positionNode.attributes();
            int positionX = positionAttributes.namedItem(QStringLiteral("x")).nodeValue().toInt();
            int positionY = positionAttributes.namedItem(QStringLiteral("y")).nodeValue().toInt();

            newRoute.append(QPointF(positionX, positionY));
        }

        newInterface->setRoute(newRoute);
    }

    parseVendorExtensions(interfaceNode, newInterface);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseMonitorInterconnection()
//-----------------------------------------------------------------------------
void DesignReader::parseMonitorInterconnection(QDomNode const& monitorNode, QSharedPointer<Design> newDesign) const
{
    NameGroupReader nameReader;
    QString name = nameReader.parseName(monitorNode);
    QString displayName = nameReader.parseDisplayName(monitorNode);
    QString description = nameReader.parseDescription(monitorNode);
    QString isPresent = monitorNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();

    QDomNode activeInterfaceNode = monitorNode.firstChildElement(QStringLiteral("ipxact:monitoredActiveInterface"));

    QSharedPointer<MonitorInterface> monitoredActiveInterface (new MonitorInterface());

    parseMonitorInterface(activeInterfaceNode, monitoredActiveInterface);

    QSharedPointer<MonitorInterconnection> newMonitorConnection (new MonitorInterconnection(name,
        monitoredActiveInterface, displayName, description));
    newMonitorConnection->setIsPresent(isPresent);

    QDomNodeList monitorChildNodes = monitorNode.childNodes();

    for (int monitorInterfaceIndex = 0; monitorInterfaceIndex < monitorChildNodes.size(); ++monitorInterfaceIndex)
    {
        QDomNode monitorInterfaceNode = monitorChildNodes.at(monitorInterfaceIndex);
        if (monitorInterfaceNode.nodeName() == QStringLiteral("ipxact:monitorInterface"))
        {
            QSharedPointer<MonitorInterface> newMonitorInterface (new MonitorInterface());
            parseMonitorInterface(monitorInterfaceNode, newMonitorInterface);

            newMonitorConnection->getMonitorInterfaces()->append(newMonitorInterface);
        }
    }

    newDesign->getMonitorInterconnecions()->append(newMonitorConnection);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseMonitorInterface()
//-----------------------------------------------------------------------------
void DesignReader::parseMonitorInterface(QDomNode const& interfaceNode,
    QSharedPointer<MonitorInterface> newInterface) const
{
    parseHierInterface(interfaceNode, newInterface);

    QDomNamedNodeMap attributeMap = interfaceNode.attributes();

    newInterface->setComponentReference(attributeMap.namedItem(QStringLiteral("componentRef")).nodeValue());
    newInterface->setPath(attributeMap.namedItem(QStringLiteral("path")).nodeValue());
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseAdHocConnections()
//-----------------------------------------------------------------------------
void DesignReader::parseAdHocConnections(QDomNode const& designNode, QSharedPointer<Design> newDesign) const
{
    QDomNode adHocConnectionsNode = designNode.firstChildElement(QStringLiteral("ipxact:adHocConnections"));

    QDomNodeList adHocNodeList = adHocConnectionsNode.childNodes();

    for (int adHocIndex = 0; adHocIndex < adHocNodeList.size(); ++adHocIndex)
    {
        parseSingleAdHocConnection(adHocNodeList.at(adHocIndex), newDesign);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseSingleAdHocConnection()
//-----------------------------------------------------------------------------
void DesignReader::parseSingleAdHocConnection(const QDomNode& adHocNode, QSharedPointer<Design> newDesign) const
{
    QString name = adHocNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue();
    QString displayName = adHocNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue();
    QString description = adHocNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue();
    QString isPresent = adHocNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();
    QString tiedValue = adHocNode.firstChildElement(QStringLiteral("ipxact:tiedValue")).firstChild().nodeValue();

    QSharedPointer<AdHocConnection> newAdHocConnection (new AdHocConnection(name, displayName, description,
        tiedValue));
    newAdHocConnection->setIsPresent(isPresent);

    QDomElement portReferencesNode = adHocNode.firstChildElement(QStringLiteral("ipxact:portReferences"));
    QDomNodeList internalReferenceNodes = portReferencesNode.elementsByTagName(QStringLiteral("ipxact:internalPortReference"));
    QDomNodeList externalReferenceNodes = portReferencesNode.elementsByTagName(QStringLiteral("ipxact:externalPortReference"));

    parseInternalPortReferences(internalReferenceNodes, newAdHocConnection);

    parseExternalPortReferences(externalReferenceNodes, newAdHocConnection);

    parseAdHocConnectionExtensions(adHocNode, newAdHocConnection);

    newDesign->getAdHocConnections()->append(newAdHocConnection);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseInternalPortReferences()
//-----------------------------------------------------------------------------
void DesignReader::parseInternalPortReferences(const QDomNodeList& internalNodes,
    QSharedPointer<AdHocConnection> newAdHocConnection) const
{
    for (int i = 0; i < internalNodes.size(); ++i)
    {
        QDomNode internalNode = internalNodes.at(i);
        QSharedPointer<PortReference> internalReference = createPortReference(internalNode);

        QDomNamedNodeMap attributeMap = internalNode.attributes();
        internalReference->setComponentRef(attributeMap.namedItem(QStringLiteral("componentRef")).nodeValue());

        newAdHocConnection->getInternalPortReferences()->append(internalReference);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseExternalPortReferences()
//-----------------------------------------------------------------------------
void DesignReader::parseExternalPortReferences(const QDomNodeList& externalNodes,
    QSharedPointer<AdHocConnection> newAdHocConnection) const
{
    for (int i = 0;i < externalNodes.size(); ++i)
    {
        QDomNode externalNode = externalNodes.at(i);
        QSharedPointer<PortReference> externalReference = createPortReference(externalNode);

        newAdHocConnection->getExternalPortReferences()->append(externalReference);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::createPortReference()
//-----------------------------------------------------------------------------
QSharedPointer<PortReference> DesignReader::createPortReference(QDomNode const& portReferenceNode) const
{
    QDomNamedNodeMap attributeMap = portReferenceNode.attributes();

    QString portReference = attributeMap.namedItem(QStringLiteral("portRef")).nodeValue();

    QSharedPointer<PortReference> newPortReference (new PortReference(portReference));

    QString isPresent = portReferenceNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();
    newPortReference->setIsPresent(isPresent);

    QDomNode partSelectNode = portReferenceNode.firstChildElement(QStringLiteral("ipxact:partSelect"));

    if (!partSelectNode.isNull())
    {
        QDomNode rangeNode = partSelectNode.firstChildElement(QStringLiteral("ipxact:range"));

        QString leftRange = rangeNode.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue();
        QString rightRange = rangeNode.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue();

        QSharedPointer<PartSelect> newPartSelect (new PartSelect(leftRange, rightRange));

        QDomElement indicesNode = partSelectNode.firstChildElement(QStringLiteral("ipxact:indices"));

        if (!indicesNode.isNull())
        {
            QDomNodeList indexNodes = indicesNode.elementsByTagName(QStringLiteral("ipxact:index"));
            for (int index = 0; index < indexNodes.size(); ++index)
            {
                QDomNode singleIndexNode = indexNodes.at(index);
                QString indexValue = singleIndexNode.firstChild().nodeValue();

                newPartSelect->getIndices()->append(indexValue);
            }
        }

        newPortReference->setPartSelect(newPartSelect);
    }

    return newPortReference;
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseAdHocConnectionExtensions()
//-----------------------------------------------------------------------------
void DesignReader::parseAdHocConnectionExtensions(const QDomNode& adHocNode,
    QSharedPointer<AdHocConnection> newAdHocConnection) const
{
    QDomNode extensionsNode = adHocNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));

    QDomElement offPageElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:offPage"));
    if (!offPageElement.isNull())
    {
        newAdHocConnection->setOffPage(true);
    }

    QDomElement routeElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:route"));
    if (!routeElement.isNull())
    {
        QList<QPointF> route;

        QDomNodeList positionNodes = routeElement.elementsByTagName(QStringLiteral("kactus2:position"));
        for (int i = 0; i < positionNodes.count(); ++i)
        {
            QDomNamedNodeMap positionAttributes = positionNodes.at(i).attributes();
            int positionX = positionAttributes.namedItem(QStringLiteral("x")).nodeValue().toInt();
            int positionY = positionAttributes.namedItem(QStringLiteral("y")).nodeValue().toInt();

            route.append(QPointF(positionX, positionY));
        }

        newAdHocConnection->setRoute(route);
    }

    parseVendorExtensions(adHocNode, newAdHocConnection);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseDesignExtensions()
//-----------------------------------------------------------------------------
void DesignReader::parseDesignExtensions(QDomNode const& documentNode, QSharedPointer<Design> design) const
{
    parseKactusAndVendorExtensions(documentNode, design);

    QDomElement extensionNode = documentNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));
   
    parseColumnLayout(extensionNode.firstChildElement(QStringLiteral("kactus2:columnLayout")), design);

    parseRoutes(extensionNode.firstChildElement(QStringLiteral("kactus2:routes")), design);

    parseSwInstances(extensionNode.firstChildElement(QStringLiteral("kactus2:swInstances")), design);

    parseAdHocPortPositions(extensionNode.firstChildElement(QStringLiteral("kactus2:adHocVisibilities")), design);

    parseApiConnections(extensionNode.firstChildElement(QStringLiteral("kactus2:apiConnections")), design);

    parseComConnections(extensionNode.firstChildElement(QStringLiteral("kactus2:comConnections")), design);

    parseInterfaceGraphics(extensionNode, design);

    parseNotes(extensionNode, design);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseColumnLayout()
//-----------------------------------------------------------------------------
void DesignReader::parseColumnLayout(QDomNode const& columnNode, QSharedPointer<Design> design) const
{
    QDomNodeList columnNodeList = columnNode.childNodes();
    int columnCount = columnNodeList.count();
    for (int columnIndex = 0; columnIndex < columnCount; ++columnIndex)
    {
        QDomNode columnNode = columnNodeList.at(columnIndex);
        if (columnNode.nodeName() == QLatin1String("kactus2:column"))
        {
            QDomNamedNodeMap attributes =  columnNode.attributes();

            ColumnTypes::ColumnContentType contentType = static_cast<ColumnTypes::ColumnContentType>(
                attributes.namedItem(QStringLiteral("contentType")).nodeValue().toInt());

            QSharedPointer<ColumnDesc> newColumn (new ColumnDesc());
            newColumn->setName(attributes.namedItem(QStringLiteral("name")).nodeValue());
            newColumn->setContentType(contentType);
            newColumn->setAllowedItems(attributes.namedItem(QStringLiteral("allowedItems")).nodeValue().toUInt());
           
            if (attributes.contains(QStringLiteral("minWidth")))
            {
                newColumn->setMinimumWidth(attributes.namedItem(QStringLiteral("minWidth")).nodeValue().toUInt());
            }
            else if (contentType == ColumnTypes::IO)
            {
                newColumn->setMinimumWidth(119);
            }

            if (attributes.contains(QStringLiteral("width")))
            {
                newColumn->setWidth(attributes.namedItem(QStringLiteral("width")).nodeValue().toUInt());
            }
            else if (contentType == ColumnTypes::IO)
            {
                newColumn->setWidth(119);
            }

            newColumn->setPosition(attributes.namedItem(QStringLiteral("x")).nodeValue().toUInt());

            design->addColumn(newColumn);

        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseRoutes()
//-----------------------------------------------------------------------------
void DesignReader::parseRoutes(QDomElement const& routesElement, QSharedPointer<Design> design) const
{
    QDomNodeList routeList = routesElement.childNodes();

    QSharedPointer<Kactus2Group> routesGroup(new Kactus2Group(QStringLiteral("kactus2:routes")));

    int routeCount = routeList.count();
    for (int i = 0; i < routeCount; ++i)
    {
        QDomElement routeNode = routeList.at(i).toElement();

        QString connectionName = routeNode.attribute(QStringLiteral("kactus2:connRef"));
        QSharedPointer<ConnectionRoute> route(new ConnectionRoute(connectionName));

        bool offpage = routeNode.attribute(QStringLiteral("kactus2:offPage")) == QLatin1String("true");
        route->setOffpage(offpage);

        QDomNodeList routePoints = routeNode.elementsByTagName(QStringLiteral("kactus2:position"));
        int pointCount = routePoints.count();
        for (int j = 0; j < pointCount; j++)
        {
            QDomElement pointNode = routePoints.at(j).toElement();
            QString x = pointNode.attribute(QStringLiteral("x"));
            QString y = pointNode.attribute(QStringLiteral("y"));

            QPointF point(x.toInt(), y.toInt());
            route->addPoint(point);
        }

        routesGroup->addToGroup(route);        
    }

    if (routeCount > 0)
    {
        design->getVendorExtensions()->append(routesGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseSwInstances()
//-----------------------------------------------------------------------------
void DesignReader::parseSwInstances(QDomNode const& swInstancesNode, QSharedPointer<Design> design) const
{
    QDomNodeList swNodeList = swInstancesNode.childNodes();
    
    int swInstanceCount = swNodeList.count();
    for (int swIndex = 0; swIndex < swInstanceCount; ++swIndex)
    {
        QDomNode swNode = swNodeList.at(swIndex);
        if (swNode.nodeName() == QLatin1String("kactus2:swInstance"))
        {
            QSharedPointer<ComponentInstance> newSWInstance = parseSwInstance(swNode);
            design->getComponentInstances()->append(newSWInstance);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseSwInstance()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> DesignReader::parseSwInstance(QDomNode const& node) const
{
    QSharedPointer<ComponentInstance> newSWInstance = QSharedPointer<ComponentInstance>(new ComponentInstance);

    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == QLatin1String("kactus2:instanceName"))
        {
            newSWInstance->setInstanceName(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:displayName"))
        {
            newSWInstance->setDisplayName(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:description"))
        {
            newSWInstance->setDescription(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:componentRef"))
        {
            QDomNamedNodeMap attributeMap = childNode.attributes();

            QString vendor = attributeMap.namedItem(QStringLiteral("vendor")).nodeValue();
            QString library = attributeMap.namedItem(QStringLiteral("library")).nodeValue();
            QString name = attributeMap.namedItem(QStringLiteral("name")).nodeValue();
            QString version = attributeMap.namedItem(QStringLiteral("version")).nodeValue();

            QSharedPointer<ConfigurableVLNVReference> vlnvReference(
                new ConfigurableVLNVReference(VLNV::COMPONENT, vendor, library, name, version));

            newSWInstance->setComponentRef(vlnvReference);
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:fileSetRef"))
        {
            newSWInstance->setFileSetRef(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:mapping"))
        {
            newSWInstance->setMapping(childNode.attributes().namedItem(QStringLiteral("hwRef")).nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:position"))
        {
            int positionX = childNode.attributes().namedItem(QStringLiteral("x")).nodeValue().toInt();
            int positionY = childNode.attributes().namedItem(QStringLiteral("y")).nodeValue().toInt();
            newSWInstance->setPosition(QPointF(positionX, positionY));
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:imported"))
        {
            newSWInstance->setImportRef(childNode.attributes().namedItem(QStringLiteral("importRef")).nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:propertyValues"))
        {
            QMap<QString, QString> newPropertyValues;

            for (int i = 0; i < childNode.childNodes().count(); ++i)
            {
                QDomNode propNode = childNode.childNodes().at(i);

                if (propNode.nodeName() == QLatin1String("kactus2:propertyValue"))
                {
                    QString name = propNode.attributes().namedItem(QStringLiteral("name")).nodeValue();
                    QString value = propNode.attributes().namedItem(QStringLiteral("value")).nodeValue();

                    newPropertyValues.insert(name, value);
                }
            }

            newSWInstance->setPropertyValues(newPropertyValues);
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:draft"))
        {
            newSWInstance->setDraft(true);
        }
    }

    return newSWInstance;
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseAdHocPositions()
//-----------------------------------------------------------------------------
void DesignReader::parseAdHocPortPositions(QDomNode const& adHocsNode, QSharedPointer<Design> design) const
{
    QMap<QString, QPointF> adHocPortPositions;

    QDomNodeList adHocPortPositionNodeList = adHocsNode.childNodes();
    int adHocPortPositionCount = adHocPortPositionNodeList.count();
    for (int portPositionIndex = 0; portPositionIndex < adHocPortPositionCount; ++portPositionIndex)
    {
        QDomNode adHocPortPositionNode = adHocPortPositionNodeList.at(portPositionIndex);
        if (adHocPortPositionNode.nodeName() == QLatin1String("kactus2:adHocVisible"))
        {
            QDomNamedNodeMap adHocAttributes = adHocPortPositionNode.attributes();
            QString portName = adHocAttributes.namedItem(QStringLiteral("portName")).nodeValue();
            int positionX = adHocAttributes.namedItem(QStringLiteral("x")).nodeValue().toInt();
            int positionY = adHocAttributes.namedItem(QStringLiteral("y")).nodeValue().toInt();

            QPointF adHocPosition (positionX, positionY);
            adHocPortPositions.insert(portName, adHocPosition);
        }
    }

    design->setAdHocPortPositions(adHocPortPositions);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseApiConnections()
//-----------------------------------------------------------------------------
void DesignReader::parseApiConnections(QDomNode const& apiConnectionsNode, QSharedPointer<Design>design) const
{
    QList<QSharedPointer<ApiInterconnection> > apiConnections;

    QDomNodeList apiConnectionNodeList = apiConnectionsNode.childNodes();

    int apiConnectionsCount = apiConnectionNodeList.count();
    for (int apiConnectionIndex = 0; apiConnectionIndex < apiConnectionsCount; ++apiConnectionIndex)
    {
        QDomNode singleApiConnectionNode = apiConnectionNodeList.at(apiConnectionIndex);

        QSharedPointer<ApiInterconnection> newApiConnection (new ApiInterconnection(singleApiConnectionNode));
        apiConnections.append(newApiConnection);
    }

    design->setApiConnections(apiConnections);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseComConnections()
//-----------------------------------------------------------------------------
void DesignReader::parseComConnections(QDomNode const& comConnectionsNode, QSharedPointer<Design> design) const
{
    QList<QSharedPointer<ComInterconnection> > comConnections;

    QDomNodeList comConnectionNodeList = comConnectionsNode.childNodes();
    int comConnectionsCount = comConnectionNodeList.count();
    for (int connectionIndex = 0; connectionIndex < comConnectionsCount; ++connectionIndex)
    {
        QDomNode singleComConnectionNode = comConnectionNodeList.at(connectionIndex);

        QSharedPointer<ComInterconnection> newConnection (new ComInterconnection(singleComConnectionNode));

        comConnections.append(newConnection);
    }

    design->setComConnections(comConnections);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseInterfaceGraphics()
//-----------------------------------------------------------------------------
void DesignReader::parseInterfaceGraphics(QDomElement const& extensionsNode, QSharedPointer<Design> design) const
{
    QDomNodeList graphicsExtensions = extensionsNode.elementsByTagName(QStringLiteral("kactus2:interfaceGraphics"));

    int extensionCount = graphicsExtensions.count();
    for (int i = 0; i < extensionCount; i++)
    {
        QDomElement extension = graphicsExtensions.at(i).toElement();
        
        QString name = extension.firstChildElement(QStringLiteral("kactus2:name")).firstChild().nodeValue();

        QDomElement positionElement = extension.firstChildElement(QStringLiteral("kactus2:position"));
        int xCoordinate = positionElement.attribute(QStringLiteral("x")).toInt();
        int yCoordinate = positionElement.attribute(QStringLiteral("y")).toInt();
        QPointF graphicsPosition (xCoordinate, yCoordinate);

        QDomElement directionElement = extension.firstChildElement(QStringLiteral("kactus2:direction"));
        int xDirection = directionElement.attribute(QStringLiteral("x")).toInt();
        int yDirection = directionElement.attribute(QStringLiteral("y")).toInt();
        QVector2D graphicsVector (xDirection, yDirection);

        QSharedPointer<InterfaceGraphicsData> graphicsData(new InterfaceGraphicsData(name, graphicsPosition, 
            graphicsVector));

        design->getVendorExtensions()->append(graphicsData);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseNotes()
//-----------------------------------------------------------------------------
void DesignReader::parseNotes(QDomElement const& extensionNode, QSharedPointer<Design> design) const
{
    QDomNodeList notesExtensions = extensionNode.elementsByTagName(QStringLiteral("kactus2:note"));

    int notesCount = notesExtensions.count();
    for (int noteIndex = 0; noteIndex < notesCount; ++noteIndex)
    {
        QSharedPointer<Kactus2Group> note(new Kactus2Group(QStringLiteral("kactus2:note")));
        
        int childCount = notesExtensions.at(noteIndex).childNodes().count();
        for (int childIndex = 0; childIndex < childCount; ++childIndex)
        {
            QDomNode childNode = notesExtensions.at(noteIndex).childNodes().at(childIndex);

            if (childNode.nodeName().compare(QLatin1String("kactus2:position")) == 0)
            {                
                QPointF point = XmlUtils::parsePoint(childNode);
                QSharedPointer<Kactus2Position> positionExtension(new Kactus2Position(point));
                note->addToGroup(positionExtension);
            }
            else if (childNode.nodeName().compare(QLatin1String("kactus2:associations")) == 0)
            {
                QSharedPointer<Kactus2Group> associations(new Kactus2Group("kactus2:associations"));

                int associationCount = childNode.childNodes().count();
                for (int associationIndex = 0; associationIndex < associationCount; ++associationIndex)
                {
                    QPointF position = XmlUtils::parsePoint(childNode.childNodes().at(associationIndex));
                    QSharedPointer<Kactus2Position> positionExtension(new Kactus2Position(position));
                    associations->addToGroup(positionExtension);
                }

                note->addToGroup(associations);
            }
            else
            {
                QSharedPointer<Kactus2Value> valueExtension(new Kactus2Value(childNode.nodeName(), 
                    childNode.firstChild().nodeValue()));
                note->addToGroup(valueExtension);
            }        
        }

        design->getVendorExtensions()->append(note);
    }
}
