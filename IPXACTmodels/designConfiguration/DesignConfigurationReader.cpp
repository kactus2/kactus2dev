//-----------------------------------------------------------------------------
// File: DesignConfigurationReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.08.2015
//
// Description:
// XML reader class for IP-XACT design configuration element.
//-----------------------------------------------------------------------------

#include "DesignConfigurationReader.h"

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::DesignConfigurationReader()
//-----------------------------------------------------------------------------
DesignConfigurationReader::DesignConfigurationReader(QObject* parent /* = 0 */):
DocumentReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::~DesignConfigurationReader()
//-----------------------------------------------------------------------------
DesignConfigurationReader::~DesignConfigurationReader()
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::createDesignConfigurationFrom()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> DesignConfigurationReader::createDesignConfigurationFrom(
    QDomDocument const& document) const
{
    QSharedPointer<DesignConfiguration> newDesignConfiguration (new DesignConfiguration);

    parseTopComments(document, newDesignConfiguration);

    parseXMLProcessingInstructions(document, newDesignConfiguration);

    QDomNode designConfigurationNode = document.firstChildElement();

    parseVLNVElements(designConfigurationNode, newDesignConfiguration, VLNV::DESIGNCONFIGURATION);

    parseDesignReference(designConfigurationNode, newDesignConfiguration);

    parseGeneratorChainConfigurations(document, newDesignConfiguration);

    parseInterconnectionConfigurations(document, newDesignConfiguration);

    parseViewConfigurations(document, newDesignConfiguration);

    parseDescription(designConfigurationNode, newDesignConfiguration);

    parseParameters(designConfigurationNode, newDesignConfiguration);

    parseAssertions(designConfigurationNode, newDesignConfiguration);

    parseDesignConfigurationExtensions(designConfigurationNode, newDesignConfiguration);

    return newDesignConfiguration;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseDesignReference()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::parseDesignReference(QDomNode const& designConfigurationNode,
    QSharedPointer<DesignConfiguration> newDesignConfiguration) const
{
    QDomNode designReferenceNode = designConfigurationNode.firstChildElement(QStringLiteral("ipxact:designRef"));

    QDomNamedNodeMap attributeMap = designReferenceNode.attributes();

    QString vendor = attributeMap.namedItem(QStringLiteral("vendor")).nodeValue();
    QString library = attributeMap.namedItem(QStringLiteral("library")).nodeValue();
    QString name = attributeMap.namedItem(QStringLiteral("name")).nodeValue();
    QString version = attributeMap.namedItem(QStringLiteral("version")).nodeValue();

    VLNV designReferenceVLNV(VLNV::DESIGN, vendor, library, name, version);
    newDesignConfiguration->setDesignRef(designReferenceVLNV);
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseGeneratorChainConfigurations()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::parseGeneratorChainConfigurations(QDomDocument const& designConfigurationDocument,
    QSharedPointer<DesignConfiguration> newDesignConfiguration) const
{
    QDomNodeList chainConfigurationElements =
        designConfigurationDocument.elementsByTagName(QStringLiteral("ipxact:generatorChainConfiguration"));

    for (int i = 0; i < chainConfigurationElements.size(); ++i)
    {
        parseSingleGeneratorChainConfiguration(chainConfigurationElements.at(i), newDesignConfiguration);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseSingleGeneratorChainConfiguration()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::parseSingleGeneratorChainConfiguration(QDomNode const& chainConfigurationNode,
    QSharedPointer<DesignConfiguration> newDesignConfiguration) const
{
    QSharedPointer<ConfigurableVLNVReference> newChainConfiguration = 
        parseConfigurableVLNVReference(chainConfigurationNode, VLNV::GENERATORCHAIN);

    newDesignConfiguration->getGeneratorChainConfs()->append(newChainConfiguration);
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseInterconnectionConfigurations()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::parseInterconnectionConfigurations(QDomDocument const& designConfigurationDocument,
    QSharedPointer<DesignConfiguration> newDesignConfiguration) const
{
    QDomNodeList interconnectionElements =
        designConfigurationDocument.elementsByTagName(QStringLiteral("ipxact:interconnectionConfiguration"));

    for (int interconConfIndex = 0; interconConfIndex < interconnectionElements.size(); ++interconConfIndex)
    {
        QDomNode interconnectionConfigurationNode = interconnectionElements.at(interconConfIndex);

        QSharedPointer<InterconnectionConfiguration> newInterconnectionConfiguration (
            new InterconnectionConfiguration());

        newInterconnectionConfiguration->setIsPresent (
            interconnectionConfigurationNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());

        newInterconnectionConfiguration->setInterconnectionReference(interconnectionConfigurationNode.
            firstChildElement(QStringLiteral("ipxact:interconnectionRef")).firstChild().nodeValue());

        parseMultipleAbstractors(interconnectionConfigurationNode, newInterconnectionConfiguration);
        
        newDesignConfiguration->getInterconnectionConfs()->append(newInterconnectionConfiguration);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseMultipleAbstractors()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::parseMultipleAbstractors(QDomNode const& interconnectionConfigurationNode,
    QSharedPointer<InterconnectionConfiguration> newInterconnectionConfiguration) const
{
    QDomNodeList multipleAbstractorsNodes =
        getNamedChildNodes(interconnectionConfigurationNode, QStringLiteral("ipxact:abstractorInstances"));

    for (int multipleAbstractorsIndex = 0; multipleAbstractorsIndex < multipleAbstractorsNodes.size();
        ++multipleAbstractorsIndex)
    {
        QDomNode multipleAbstractorNode = multipleAbstractorsNodes.at(multipleAbstractorsIndex);

        QSharedPointer<MultipleAbstractorInstances> newMultipleAbstractors (new MultipleAbstractorInstances());

        newMultipleAbstractors->setIsPresent(
            multipleAbstractorNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());

        parseInterfaceReferences(multipleAbstractorNode, newMultipleAbstractors);

        parseAbstractorInstances(multipleAbstractorNode, newMultipleAbstractors);

        newInterconnectionConfiguration->getAbstractorInstances()->append(newMultipleAbstractors);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseMultipleAbstractors()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::parseInterfaceReferences(QDomNode const& multipleAbstractorsNode,
    QSharedPointer<MultipleAbstractorInstances> newMultipleAbstractorInstances) const
{
    QDomNodeList interfaceRefNodes = getNamedChildNodes(multipleAbstractorsNode, QStringLiteral("ipxact:interfaceRef"));

    for (int interfaceRefIndex = 0; interfaceRefIndex < interfaceRefNodes.size(); ++interfaceRefIndex)
    {
        QDomNode singleInterfaceRefNode = interfaceRefNodes.at(interfaceRefIndex);

        QDomNamedNodeMap attributeMap = singleInterfaceRefNode.attributes();

        QSharedPointer<InterfaceRef> newInterfaceRef (new InterfaceRef());

        newInterfaceRef->setComponentRef(attributeMap.namedItem(QStringLiteral("componentRef")).nodeValue());
        newInterfaceRef->setBusRef(attributeMap.namedItem(QStringLiteral("busRef")).nodeValue());
        newInterfaceRef->setIsPresent(
            singleInterfaceRefNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());

        newMultipleAbstractorInstances->getInterfaceReferences()->append(newInterfaceRef);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseAbstractorInstances()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::parseAbstractorInstances(QDomNode const& multipleAbstractorsNode,
    QSharedPointer<MultipleAbstractorInstances> newMultipleAbstractorInstances) const
{
    QDomNodeList abstractorInstanceNodes = getNamedChildNodes(multipleAbstractorsNode, QStringLiteral("ipxact:abstractorInstance"));

    for (int i = 0; i < abstractorInstanceNodes.size(); ++i)
    {
        QDomNode singleAbstractorInstanceNode = abstractorInstanceNodes.at(i);

        QSharedPointer<AbstractorInstance> newAbstractorInstance (new AbstractorInstance());

        newAbstractorInstance->setInstanceName(
            singleAbstractorInstanceNode.firstChildElement(QStringLiteral("ipxact:instanceName")).firstChild().nodeValue());
        newAbstractorInstance->setDisplayName(
            singleAbstractorInstanceNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue());
        newAbstractorInstance->setDescription(
            singleAbstractorInstanceNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue());
        newAbstractorInstance->setViewName(
            singleAbstractorInstanceNode.firstChildElement(QStringLiteral("ipxact:viewName")).firstChild().nodeValue());

        QDomNode abstractorReferenceNode
            = singleAbstractorInstanceNode.firstChildElement(QStringLiteral("ipxact:abstractorRef"));

        QSharedPointer<ConfigurableVLNVReference> newAbstractorRef = 
            parseConfigurableVLNVReference(abstractorReferenceNode, VLNV::ABSTRACTOR);

        newAbstractorInstance->setAbstractorRef(newAbstractorRef);

        newMultipleAbstractorInstances->getAbstractorInstances()->append(newAbstractorInstance);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::getNamedChildNodes()
//-----------------------------------------------------------------------------
QDomNodeList DesignConfigurationReader::getNamedChildNodes(QDomNode const& targetNode, QString elementName) const
{
    QDomNodeList allChildNodes = targetNode.childNodes();

    QDomDocument targetDocument(QStringLiteral("temporaryDoc"));

    for (int i = 0; i < allChildNodes.size(); ++i)
    {
        QDomNode possibleNode = allChildNodes.at(i);
        if (possibleNode.nodeName() == elementName)
        {
            targetDocument.appendChild(possibleNode);
        }
    }

    return targetDocument.childNodes();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseViewConfigurations()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::parseViewConfigurations(QDomDocument const& designConfigurationDocument,
    QSharedPointer<DesignConfiguration> newDesignConfiguration) const
{
    QDomNodeList viewConfigurationNodes = designConfigurationDocument.elementsByTagName(QStringLiteral("ipxact:viewConfiguration"));

    for (int i = 0; i < viewConfigurationNodes.size(); ++i)
    {
        QDomNode singleViewConfigurationNode = viewConfigurationNodes.at(i);

        QSharedPointer<ViewConfiguration> newViewConfiguration (new ViewConfiguration());

        newViewConfiguration->setInstanceName(
            singleViewConfigurationNode.firstChildElement(QStringLiteral("ipxact:instanceName")).firstChild().nodeValue());
        newViewConfiguration->setIsPresent(
            singleViewConfigurationNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());

        QDomNode viewNode = singleViewConfigurationNode.firstChildElement(QStringLiteral("ipxact:view"));
        
        QDomNamedNodeMap attributeMap = viewNode.attributes();

        newViewConfiguration->setViewReference(attributeMap.namedItem(QStringLiteral("viewRef")).nodeValue());

        QDomNode multipleConfigurableElementsNode = viewNode.firstChildElement(QStringLiteral("ipxact:configurableElementValues"));

        QDomNodeList configurableElementNodes = multipleConfigurableElementsNode.childNodes();

        for (int i = 0; i < configurableElementNodes.size(); ++i)
        {
            QSharedPointer<ConfigurableElementValue> newConfigurableElementValue =
                parseConfigurableElementValue(configurableElementNodes.at(i));

            newViewConfiguration->getViewConfigurableElements()->append(newConfigurableElementValue);
        }

        newDesignConfiguration->getViewConfigurations()->append(newViewConfiguration);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseDesignConfigurationExtensions()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::parseDesignConfigurationExtensions(QDomNode const& designConfigurationNode,
    QSharedPointer<DesignConfiguration> newDesignConfiguration) const
{
    QDomNode extensionsNode = designConfigurationNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));

    QDomElement configurableElementsNode = extensionsNode.firstChildElement(
        QStringLiteral("kactus2:configurableElementValues"));
    if (!configurableElementsNode.isNull())
    {
        parseInstanceConfigurableElementValues(configurableElementsNode, newDesignConfiguration);
    }

    QDomElement viewOverridesNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:viewOverrides"));
    if (!viewOverridesNode.isNull())
    {
        parseViewOverrides(viewOverridesNode, newDesignConfiguration);
    }

    parseKactusAndVendorExtensions(designConfigurationNode, newDesignConfiguration);
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseConfigurableElementValues()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::parseInstanceConfigurableElementValues(QDomElement const& configurableElementsNode,
    QSharedPointer<DesignConfiguration> newDesignConfiguration) const
{
    QDomNodeList componentInstanceList =
        configurableElementsNode.elementsByTagName(QStringLiteral("kactus2:componentInstance"));
    for (int instanceIndex = 0; instanceIndex < componentInstanceList.count(); ++instanceIndex)
    {
        QDomNode instance = componentInstanceList.at(instanceIndex);
        QDomElement instanceElement = instance.toElement();

        if (!instanceElement.isNull())
        {
            QString instanceUUID = instanceElement.firstChildElement(QStringLiteral("kactus2:uuid")).firstChild().nodeValue();
            QMap<QString, QString> configurableElementValues;

            QDomNodeList elementNodes = instanceElement.elementsByTagName(QStringLiteral("kactus2:configurableElementValue"));
            for (int elementIndex = 0; elementIndex < elementNodes.count(); ++elementIndex)
            {
                QDomNode singleElementNode = elementNodes.at(elementIndex);
                QDomNamedNodeMap elementAttributes = singleElementNode.attributes();

                QString referenceID = elementAttributes.namedItem(QStringLiteral("referenceId")).nodeValue();
                QString configuredValue = elementAttributes.namedItem(QStringLiteral("value")).nodeValue();
                configurableElementValues.insert(referenceID, configuredValue);
            }

            newDesignConfiguration->setConfigurableElementValues(instanceUUID, configurableElementValues);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseViewOverrides()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::parseViewOverrides(QDomElement const& viewOverridesNode,
    QSharedPointer<DesignConfiguration> newDesignConfiguration) const
{
    QMap<QString, QString> newViewOverrides;

    QDomNodeList viewOverrideList = viewOverridesNode.elementsByTagName(QStringLiteral("kactus2:instanceView"));
    for (int overrideIndex = 0; overrideIndex < viewOverrideList.count(); ++overrideIndex)
    {
        QDomNode viewOverride = viewOverrideList.at(overrideIndex);

        QDomNamedNodeMap overrideAttributes = viewOverride.attributes();
        QString instanceID = overrideAttributes.namedItem(QStringLiteral("id")).nodeValue();
        QString viewName = overrideAttributes.namedItem(QStringLiteral("viewName")).nodeValue();

        newViewOverrides.insert(instanceID, viewName);
    }

    newDesignConfiguration->setKactus2ViewOverrides(newViewOverrides);
}