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


//-----------------------------------------------------------------------------
// Function: DesignReader::DesignReader()
//-----------------------------------------------------------------------------
DesignReader::DesignReader(QObject* parent /* = 0 */):
DocumentReader(parent)
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

    QDomNode designNode = document.firstChildElement();

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
void DesignReader::parseComponentInstances(const QDomNode& designNode, QSharedPointer<Design> newDesign) const
{
    QDomNode componentInstancesNode = designNode.firstChildElement("ipxact:componentInstances");

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
void DesignReader::parseInterconnections(const QDomNode& designNode, QSharedPointer<Design> newDesign) const
{
    QDomNode multipleInterconnectionsNode = designNode.firstChildElement("ipxact:interconnections");

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
void DesignReader::parseSingleInterconnection(const QDomNode& interconnectionNode,
    QSharedPointer<Design> newDesign) const
{
    QString name = interconnectionNode.firstChildElement("ipxact:name").firstChild().nodeValue();
    QString displayName = interconnectionNode.firstChildElement("ipxact:displayName").firstChild().nodeValue();
    QString description = interconnectionNode.firstChildElement("ipxact:description").firstChild().nodeValue();
    QString isPresent = interconnectionNode.firstChildElement("ipxact:isPresent").firstChild().nodeValue();

    if (interconnectionNode.nodeName() == "ipxact:interconnection")
    {
        parseComponentInterconnection(interconnectionNode, newDesign, name, displayName, description, isPresent);
    }

    else if (interconnectionNode.nodeName() == "ipxact:monitorInterconnection")
    {
        parseMonitorInterconnection(interconnectionNode, newDesign, name, displayName, description, isPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseComponentInerconnection()
//-----------------------------------------------------------------------------
void DesignReader::parseComponentInterconnection(const QDomNode& interconnectionNode,
    QSharedPointer<Design> newDesign, QString const& name, QString const& displayName, QString const& description,
    QString const& isPresent) const
{
    QDomNode startInterfaceNode = interconnectionNode.firstChildElement("ipxact:activeInterface");

    QSharedPointer<ActiveInterface> startInterface (new ActiveInterface);
    parseActiveInterface(startInterfaceNode, startInterface);

    QSharedPointer<Interconnection> newInterconnection (
        new Interconnection(name, startInterface, displayName, description));
    newInterconnection->setIsPresent(isPresent);

    QDomNodeList interfaceNodes = interconnectionNode.childNodes();

    for (int interfaceIndex = 0; interfaceIndex < interfaceNodes.size(); ++interfaceIndex)
    {
        QDomNode singleInterfaceNode = interfaceNodes.at(interfaceIndex);
        if (singleInterfaceNode.nodeName() == "ipxact:activeInterface" 
            && singleInterfaceNode != startInterfaceNode)
        {
            QSharedPointer<ActiveInterface> activeInterface (new ActiveInterface());
            parseActiveInterface(singleInterfaceNode, activeInterface);

            newInterconnection->getActiveInterfaces()->append(activeInterface);
        }
        else if (singleInterfaceNode.nodeName() == "ipxact:hierInterface")
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
    QDomNode extensionNode = interconnectionNode.firstChildElement("ipxact:vendorExtensions"); 
    QDomElement offPageElement = extensionNode.firstChildElement("kactus2:offPage");

    if (!offPageElement.isNull())
    {
        interconnection->setOffPage(true);
    }

    parseVendorExtensions(interconnectionNode, interconnection);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseActiveInterface()
//-----------------------------------------------------------------------------
void DesignReader::parseActiveInterface(const QDomNode& interfaceNode,
    QSharedPointer<ActiveInterface> newInterface) const
{
    parseHierInterface(interfaceNode, newInterface);

    QDomNamedNodeMap attributeMap = interfaceNode.attributes();

    newInterface->setComponentReference(attributeMap.namedItem("componentRef").nodeValue());

    QDomNode baseExcludePortsNode = interfaceNode.firstChildElement("ipxact:excludePorts");
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
void DesignReader::parseHierInterface(const QDomNode& interfaceNode, QSharedPointer<HierInterface> newInterface)
    const
{
    QDomNamedNodeMap attributeMap = interfaceNode.attributes();
    
    newInterface->setBusReference(attributeMap.namedItem("busRef").nodeValue());
    newInterface->setIsPresent(interfaceNode.firstChildElement("ipxact:isPresent").firstChild().nodeValue());
    newInterface->setDescription(interfaceNode.firstChildElement("ipxact:description").firstChild().nodeValue());

    parseHierInterfaceExtensions(interfaceNode, newInterface);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseHierInterfaceExtensions()
//-----------------------------------------------------------------------------
void DesignReader::parseHierInterfaceExtensions(const QDomNode& interfaceNode,
    QSharedPointer<HierInterface> newInterface) const
{
    QDomNode extensionsNode = interfaceNode.firstChildElement("ipxact:vendorExtensions");

    QDomElement routeElement = extensionsNode.firstChildElement("kactus2:route");
    if (!routeElement.isNull())
    {
        QList<QPointF> newRoute;

        QDomNodeList positionNodesList = routeElement.elementsByTagName("kactus2:position");

        for (int positionIndex = 0; positionIndex < positionNodesList.count(); ++positionIndex)
        {
            QDomNode positionNode = positionNodesList.at(positionIndex);
            QDomNamedNodeMap positionAttributes = positionNode.attributes();
            int positionX = positionAttributes.namedItem("x").nodeValue().toInt();
            int positionY = positionAttributes.namedItem("y").nodeValue().toInt();

            newRoute.append(QPointF(positionX, positionY));
        }

        newInterface->setRoute(newRoute);
    }

    parseVendorExtensions(interfaceNode, newInterface);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseMonitorInterconnection()
//-----------------------------------------------------------------------------
void DesignReader::parseMonitorInterconnection(const QDomNode& monitorNode, QSharedPointer<Design> newDesign,
    QString const& name, QString const& displayName, QString const& description, QString const& isPresent) const
{
    QDomNode activeInterfaceNode = monitorNode.firstChildElement("ipxact:monitoredActiveInterface");

    QSharedPointer<MonitorInterface> monitoredActiveInterface (new MonitorInterface());

    parseMonitorInterface(activeInterfaceNode, monitoredActiveInterface);

    QSharedPointer<MonitorInterconnection> newMonitorConnection (new MonitorInterconnection(name,
        monitoredActiveInterface, displayName, description));
    newMonitorConnection->setIsPresent(isPresent);

    QDomNodeList monitorChildNodes = monitorNode.childNodes();

    for (int monitorInterfaceIndex = 0; monitorInterfaceIndex < monitorChildNodes.size(); ++monitorInterfaceIndex)
    {
        QDomNode monitorInterfaceNode = monitorChildNodes.at(monitorInterfaceIndex);
        if (monitorInterfaceNode.nodeName() == "ipxact:monitorInterface")
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
void DesignReader::parseMonitorInterface(const QDomNode& interfaceNode,
    QSharedPointer<MonitorInterface> newInterface) const
{
    parseHierInterface(interfaceNode, newInterface);

    QDomNamedNodeMap attributeMap = interfaceNode.attributes();

    newInterface->setComponentReference(attributeMap.namedItem("componentRef").nodeValue());
    newInterface->setPath(attributeMap.namedItem("path").nodeValue());
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseAdHocConnections()
//-----------------------------------------------------------------------------
void DesignReader::parseAdHocConnections(const QDomNode& designNode, QSharedPointer<Design> newDesign) const
{
    QDomNode adHocConnectionsNode = designNode.firstChildElement("ipxact:adHocConnections");

    QDomNodeList adHocNodeList = adHocConnectionsNode.childNodes();

    for (int adHocIndex = 0; adHocIndex < adHocNodeList.size(); ++adHocIndex)
    {
        QDomNode currentAdHocNode = adHocNodeList.at(adHocIndex);

        QString name = currentAdHocNode.firstChildElement("ipxact:name").firstChild().nodeValue();
        QString displayName = currentAdHocNode.firstChildElement("ipxact:displayName").firstChild().nodeValue();
        QString description = currentAdHocNode.firstChildElement("ipxact:description").firstChild().nodeValue();
        QString isPresent = currentAdHocNode.firstChildElement("ipxact:isPresent").firstChild().nodeValue();
        QString tiedValue = currentAdHocNode.firstChildElement("ipxact:tiedValue").firstChild().nodeValue();

        QSharedPointer<AdHocConnection> newAdHocConnection (new AdHocConnection(name, displayName, description,
            tiedValue));
        newAdHocConnection->setIsPresent(isPresent);

        QDomElement portReferencesNode = currentAdHocNode.firstChildElement("ipxact:portReferences");
        QDomNodeList internalReferenceNodes = portReferencesNode.elementsByTagName("ipxact:internalPortReference");
        QDomNodeList externalReferenceNodes = portReferencesNode.elementsByTagName("ipxact:externalPortReference");

        for (int i = 0; i < internalReferenceNodes.size(); ++i)
        {
            QDomNode internalNode = internalReferenceNodes.at(i);
            QSharedPointer<PortReference> internalReference = createPortReference(internalNode);
            
            QDomNamedNodeMap attributeMap = internalNode.attributes();
            internalReference->setComponentRef(attributeMap.namedItem("componentRef").nodeValue());

            newAdHocConnection->getInternalPortReferences()->append(internalReference);
        }

        for (int i = 0;i < externalReferenceNodes.size(); ++i)
        {
            QDomNode externalNode = externalReferenceNodes.at(i);
            QSharedPointer<PortReference> externalReference = createPortReference(externalNode);

            newAdHocConnection->getExternalPortReferences()->append(externalReference);
        }

        parseVendorExtensions(currentAdHocNode, newAdHocConnection);

        newDesign->getAdHocConnections()->append(newAdHocConnection);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignReader::createPortReference()
//-----------------------------------------------------------------------------
QSharedPointer<PortReference> DesignReader::createPortReference(const QDomNode& portReferenceNode) const
{
    QDomNamedNodeMap attributeMap = portReferenceNode.attributes();

    QString portReference = attributeMap.namedItem("portRef").nodeValue();

    QSharedPointer<PortReference> newPortReference (new PortReference(portReference));

    newPortReference->setIsPresent(
        portReferenceNode.firstChildElement("ipxact:isPresent").firstChild().nodeValue());

    QDomNode partSelectNode = portReferenceNode.firstChildElement("ipxact:partSelect");

    if (!partSelectNode.isNull())
    {
        QDomNode rangeNode = partSelectNode.firstChildElement("ipxact:range");

        QString leftRange = rangeNode.firstChildElement("ipxact:left").firstChild().nodeValue();
        QString rightRange = rangeNode.firstChildElement("ipxact:right").firstChild().nodeValue();

        QSharedPointer<PartSelect> newPartSelect (new PartSelect(leftRange, rightRange));

        QDomElement indicesNode = partSelectNode.firstChildElement("ipxact:indices");

        if (!indicesNode.isNull())
        {
            QDomNodeList indexNodes = indicesNode.elementsByTagName("ipxact:index");
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
// Function: DesignReader::parseKactusAndVendorExtensions()
//-----------------------------------------------------------------------------
void DesignReader::parseDesignExtensions(QDomNode const& documentNode, QSharedPointer<Design> design) const
{
    QDomNodeList extensionNodes = documentNode.firstChildElement("ipxact:vendorExtensions").childNodes();
    int extensionCount = extensionNodes.count();

    for (int extensionIndex = 0; extensionIndex < extensionCount; ++extensionIndex)
    {
        QDomNode singleExtensionNode = extensionNodes.at(extensionIndex);
        if (singleExtensionNode.nodeName() == "kactus2:columnLayout")
        {
            parseColumnLayout(singleExtensionNode, design);
        }
        if (singleExtensionNode.nodeName() == "kactus2:swInstances")
        {
            parseSwInstances(singleExtensionNode, design);
        }
        if (singleExtensionNode.nodeName() == "kactus2:adHocVisibilities")
        {
            parseAdHocPortPositions(singleExtensionNode, design);
        }
        if (singleExtensionNode.nodeName() == "kactus2:apiConnections")
        {
            parseApiConnections(singleExtensionNode, design);
        }
        if (singleExtensionNode.nodeName() == "kactus2:hierApiDependencies")
        {
            parseHierApiConnections(singleExtensionNode, design);
        }
        if (singleExtensionNode.nodeName() == "kactus2:comConnections")
        {
            parseComConnections(singleExtensionNode, design);
        }
        if (singleExtensionNode.nodeName() == "kactus2:hierComConnections")
        {
            parseHierComConnections(singleExtensionNode, design);
        }
    }

    parseKactusAndVendorExtensions(documentNode, design);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseColumnLayout()
//-----------------------------------------------------------------------------
void DesignReader::parseColumnLayout(QDomNode const& columnNode, QSharedPointer<Design> design) const
{
    QList<QSharedPointer<ColumnDesc> > columnLayout;

    QDomNodeList columnNodeList = columnNode.childNodes();
    int columnCount = columnNodeList.count();
    for (int columnIndex = 0 ;columnIndex < columnCount; ++columnIndex)
    {
        QDomNode columnNode = columnNodeList.at(columnIndex);
        if (columnNode.nodeName() == "kactus2:column")
        {
            QSharedPointer<ColumnDesc> newColumn (new ColumnDesc(columnNode));
            columnLayout.append(newColumn);
        }
    }
    design->setColumns(columnLayout);
}

//-----------------------------------------------------------------------------
// Function: DesignReader::parseSwInstances()
//-----------------------------------------------------------------------------
void DesignReader::parseSwInstances(QDomNode const& swInstancesNode, QSharedPointer<Design> design) const
{
    QList<QSharedPointer<SWInstance> > swInstanceList;

    QDomNodeList swNodeList = swInstancesNode.childNodes();
    int swCount = swNodeList.count();

    for (int swIndex = 0;swIndex < swCount; ++swIndex)
    {
        QDomNode swNode = swNodeList.at(swIndex);
        if (swNode.nodeName() == "kactus2:swInstance")
        {
            QSharedPointer<SWInstance> newSWInstance (new SWInstance(swNode));
            swInstanceList.append(newSWInstance);
        }
    }

    design->setSWInstances(swInstanceList);
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
        if (adHocPortPositionNode.nodeName() == "kactus2:adHocVisible")
        {
            QDomNamedNodeMap adHocAttributes = adHocPortPositionNode.attributes();
            QString portName = adHocAttributes.namedItem("portName").nodeValue();
            int positionX = adHocAttributes.namedItem("x").nodeValue().toInt();
            int positionY = adHocAttributes.namedItem("y").nodeValue().toInt();

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
// Function: DesignReader::parseHierApiConnections()
//-----------------------------------------------------------------------------
void DesignReader::parseHierApiConnections(QDomNode const& hierApiConnectionsNode, QSharedPointer<Design> design)
    const
{
    QList<QSharedPointer<HierApiInterconnection> > hierApiConnections;

    QDomNodeList hierApiConnectionNodeList = hierApiConnectionsNode.childNodes();
    int hierApiConnectionsCount = hierApiConnectionNodeList.count();
    for (int connectionIndex = 0; connectionIndex < hierApiConnectionsCount; ++connectionIndex)
    {
        QDomNode singleHierApiConnectionNode = hierApiConnectionNodeList.at(connectionIndex);

        QSharedPointer<HierApiInterconnection> newConnection (
            new HierApiInterconnection(singleHierApiConnectionNode));

        hierApiConnections.append(newConnection);
    }

    design->setHierApiDependencies(hierApiConnections);
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
// Function: DesignReader::parseHierComConnections()
//-----------------------------------------------------------------------------
void DesignReader::parseHierComConnections(QDomNode const& hierComConnectionsNode, QSharedPointer<Design> design)
    const
{
    QList<QSharedPointer<HierComInterconnection> > hierComConnections;

    QDomNodeList hierComConnectionNodeList = hierComConnectionsNode.childNodes();
    int hierComConnectionsCount = hierComConnectionNodeList.count();
    for (int connectionIndex = 0; connectionIndex < hierComConnectionsCount; ++connectionIndex)
    {
        QDomNode singleHierComConnectionNode = hierComConnectionNodeList.at(connectionIndex);

        QSharedPointer<HierComInterconnection> newConnection (
            new HierComInterconnection(singleHierComConnectionNode));

        hierComConnections.append(newConnection);
    }

    design->setHierComConnections(hierComConnections);
}