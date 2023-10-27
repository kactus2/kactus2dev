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

#include <IPXACTmodels/common/DocumentReader.h>

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::createDesignConfigurationFrom()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> DesignConfigurationReader::createDesignConfigurationFrom(
    QDomDocument const& document)
{
    QDomNode designConfigurationNode = document.firstChildElement();

    VLNV vlnv = CommonItemsReader::createVLNVFrom(designConfigurationNode, VLNV::DESIGNCONFIGURATION);
    Document::Revision docRevision = DocumentReader::getXMLDocumentRevision(designConfigurationNode);

    QSharedPointer<DesignConfiguration> newDesignConfiguration (new DesignConfiguration(vlnv, docRevision));

    DocumentReader::parseTopComments(document, newDesignConfiguration);

    DocumentReader::parseXMLProcessingInstructions(document, newDesignConfiguration);

    DocumentReader::parseNamespaceDeclarations(designConfigurationNode, newDesignConfiguration);

    DocumentReader::parseDocumentNameGroup(designConfigurationNode, newDesignConfiguration);

    Details::parseDesignReference(designConfigurationNode, newDesignConfiguration);

    Details::parseGeneratorChainConfigurations(document, newDesignConfiguration);

    Details::parseInterconnectionConfigurations(document, newDesignConfiguration);

    Details::parseViewConfigurations(document, newDesignConfiguration);

    DocumentReader::parseParameters(designConfigurationNode, newDesignConfiguration);

    DocumentReader::parseAssertions(designConfigurationNode, newDesignConfiguration);

    Details::parseDesignConfigurationExtensions(designConfigurationNode, newDesignConfiguration);

    return newDesignConfiguration;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseDesignReference()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::Details::parseDesignReference(QDomNode const& designConfigurationNode,
    QSharedPointer<DesignConfiguration> newDesignConfiguration)
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
void DesignConfigurationReader::Details::parseGeneratorChainConfigurations(QDomDocument const& designConfigurationDocument,
    QSharedPointer<DesignConfiguration> newDesignConfiguration)
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
void DesignConfigurationReader::Details::parseSingleGeneratorChainConfiguration(QDomNode const& chainConfigurationNode,
    QSharedPointer<DesignConfiguration> newDesignConfiguration)
{
    QSharedPointer<ConfigurableVLNVReference> newChainConfiguration = 
        CommonItemsReader::parseConfigurableVLNVReference(chainConfigurationNode, VLNV::GENERATORCHAIN);

    newDesignConfiguration->getGeneratorChainConfs()->append(newChainConfiguration);
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseInterconnectionConfigurations()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::Details::parseInterconnectionConfigurations(QDomDocument const& designConfigurationDocument,
    QSharedPointer<DesignConfiguration> newDesignConfiguration)
{
    QDomNodeList interconnectionElements =
        designConfigurationDocument.elementsByTagName(QStringLiteral("ipxact:interconnectionConfiguration"));

    auto docRevision = newDesignConfiguration->getRevision();

    for (int interconConfIndex = 0; interconConfIndex < interconnectionElements.size(); ++interconConfIndex)
    {
        QDomNode interconnectionConfigurationNode = interconnectionElements.at(interconConfIndex);

        QSharedPointer<InterconnectionConfiguration> newInterconnectionConfiguration (
            new InterconnectionConfiguration());

        if (docRevision == Document::Revision::Std14)
        {
            newInterconnectionConfiguration->setIsPresent(
                interconnectionConfigurationNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());
        }

        newInterconnectionConfiguration->setInterconnectionReference(interconnectionConfigurationNode.
            firstChildElement(QStringLiteral("ipxact:interconnectionRef")).firstChild().nodeValue());

        parseMultipleAbstractors(interconnectionConfigurationNode, newInterconnectionConfiguration, docRevision);
        
        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsReader::parseVendorExtensions(interconnectionConfigurationNode, newInterconnectionConfiguration);
        }

        newDesignConfiguration->getInterconnectionConfs()->append(newInterconnectionConfiguration);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseMultipleAbstractors()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::Details::parseMultipleAbstractors(QDomNode const& interconnectionConfigurationNode,
    QSharedPointer<InterconnectionConfiguration> newInterconnectionConfiguration, 
    Document::Revision docRevision)
{
    QDomNodeList multipleAbstractorsNodes =
        getNamedChildNodes(interconnectionConfigurationNode, QStringLiteral("ipxact:abstractorInstances"));

    for (int multipleAbstractorsIndex = 0; multipleAbstractorsIndex < multipleAbstractorsNodes.size();
        ++multipleAbstractorsIndex)
    {
        QDomNode multipleAbstractorNode = multipleAbstractorsNodes.at(multipleAbstractorsIndex);

        QSharedPointer<MultipleAbstractorInstances> newMultipleAbstractors (new MultipleAbstractorInstances());

        if (docRevision == Document::Revision::Std14)
        {
            newMultipleAbstractors->setIsPresent(
                multipleAbstractorNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());
        }

        parseInterfaceReferences(multipleAbstractorNode, newMultipleAbstractors, docRevision);

        parseAbstractorInstances(multipleAbstractorNode, newMultipleAbstractors, docRevision);

        newInterconnectionConfiguration->getAbstractorInstances()->append(newMultipleAbstractors);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseMultipleAbstractors()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::Details::parseInterfaceReferences(QDomNode const& multipleAbstractorsNode,
    QSharedPointer<MultipleAbstractorInstances> newMultipleAbstractorInstances, Document::Revision docRevision)
{
    QDomNodeList interfaceRefNodes = getNamedChildNodes(multipleAbstractorsNode, QStringLiteral("ipxact:interfaceRef"));

    for (int interfaceRefIndex = 0; interfaceRefIndex < interfaceRefNodes.size(); ++interfaceRefIndex)
    {
        QDomNode singleInterfaceRefNode = interfaceRefNodes.at(interfaceRefIndex);

        QDomNamedNodeMap attributeMap = singleInterfaceRefNode.attributes();

        QSharedPointer<InterfaceRef> newInterfaceRef (new InterfaceRef());

        newInterfaceRef->setComponentRef(attributeMap.namedItem(QStringLiteral("componentRef")).nodeValue());
        newInterfaceRef->setBusRef(attributeMap.namedItem(QStringLiteral("busRef")).nodeValue());
        
        if (docRevision == Document::Revision::Std14)
        {
            newInterfaceRef->setIsPresent(
                singleInterfaceRefNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());
        }

        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsReader::parseVendorExtensions(singleInterfaceRefNode, newInterfaceRef);
        }

        newMultipleAbstractorInstances->getInterfaceReferences()->append(newInterfaceRef);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseAbstractorInstances()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::Details::parseAbstractorInstances(QDomNode const& multipleAbstractorsNode,
    QSharedPointer<MultipleAbstractorInstances> newMultipleAbstractorInstances, Document::Revision docRevision)
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

        if (docRevision == Document::Revision::Std22)
        {
            newAbstractorInstance->setShortDescription(
                singleAbstractorInstanceNode.firstChildElement(QStringLiteral("ipxact:shortDescription")).firstChild().nodeValue());
        }

        newAbstractorInstance->setDescription(
            singleAbstractorInstanceNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue());
        newAbstractorInstance->setViewName(
            singleAbstractorInstanceNode.firstChildElement(QStringLiteral("ipxact:viewName")).firstChild().nodeValue());

        QDomNode abstractorReferenceNode
            = singleAbstractorInstanceNode.firstChildElement(QStringLiteral("ipxact:abstractorRef"));

        QSharedPointer<ConfigurableVLNVReference> newAbstractorRef = 
            CommonItemsReader::parseConfigurableVLNVReference(abstractorReferenceNode, VLNV::ABSTRACTOR);

        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsReader::parseVendorExtensions(singleAbstractorInstanceNode, newAbstractorInstance);
        }

        newAbstractorInstance->setAbstractorRef(newAbstractorRef);

        newMultipleAbstractorInstances->getAbstractorInstances()->append(newAbstractorInstance);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::getNamedChildNodes()
//-----------------------------------------------------------------------------
QDomNodeList DesignConfigurationReader::Details::getNamedChildNodes(QDomNode const& targetNode, QString elementName)
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
void DesignConfigurationReader::Details::parseViewConfigurations(QDomDocument const& designConfigurationDocument,
    QSharedPointer<DesignConfiguration> newDesignConfiguration)
{
    QDomNodeList viewConfigurationNodes = designConfigurationDocument.elementsByTagName(QStringLiteral("ipxact:viewConfiguration"));

    auto docRevision = newDesignConfiguration->getRevision();

    for (int i = 0; i < viewConfigurationNodes.size(); ++i)
    {
        QDomNode singleViewConfigurationNode = viewConfigurationNodes.at(i);

        QSharedPointer<ViewConfiguration> newViewConfiguration (new ViewConfiguration());

        newViewConfiguration->setInstanceName(
            singleViewConfigurationNode.firstChildElement(QStringLiteral("ipxact:instanceName")).firstChild().nodeValue());

        if (docRevision == Document::Revision::Std14)
        {
            newViewConfiguration->setIsPresent(
                singleViewConfigurationNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());
        }

        QDomNode viewNode = singleViewConfigurationNode.firstChildElement(QStringLiteral("ipxact:view"));
        
        QDomNamedNodeMap attributeMap = viewNode.attributes();

        newViewConfiguration->setViewReference(attributeMap.namedItem(QStringLiteral("viewRef")).nodeValue());

        QDomNode multipleConfigurableElementsNode = viewNode.firstChildElement(QStringLiteral("ipxact:configurableElementValues"));

        QDomNodeList configurableElementNodes = multipleConfigurableElementsNode.childNodes();

        for (int j = 0; j < configurableElementNodes.size(); ++j)
        {
            QSharedPointer<ConfigurableElementValue> newConfigurableElementValue =
                CommonItemsReader::parseConfigurableElementValue(configurableElementNodes.at(j));

            newViewConfiguration->getViewConfigurableElements()->append(newConfigurableElementValue);
        }

        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsReader::parseVendorExtensions(singleViewConfigurationNode, newViewConfiguration);
        }

        newDesignConfiguration->getViewConfigurations()->append(newViewConfiguration);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseDesignConfigurationExtensions()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::Details::parseDesignConfigurationExtensions(QDomNode const& designConfigurationNode,
    QSharedPointer<DesignConfiguration> newDesignConfiguration)
{
    QDomNode extensionsNode = designConfigurationNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));

    QDomElement viewOverridesNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:viewOverrides"));
    if (!viewOverridesNode.isNull())
    {
        parseViewOverrides(viewOverridesNode, newDesignConfiguration);
    }

    DocumentReader::parseKactusAndVendorExtensions(designConfigurationNode, newDesignConfiguration);
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationReader::parseViewOverrides()
//-----------------------------------------------------------------------------
void DesignConfigurationReader::Details::parseViewOverrides(QDomElement const& viewOverridesNode,
    QSharedPointer<DesignConfiguration> newDesignConfiguration)
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
