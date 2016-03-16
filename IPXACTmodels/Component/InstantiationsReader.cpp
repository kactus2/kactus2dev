//-----------------------------------------------------------------------------
// File: InstantiationsReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Reader class for IP-XACT instantiations.
//-----------------------------------------------------------------------------

#include "InstantiationsReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/FileBuilderReader.h>

#include <IPXACTmodels/common/ModuleParameterReader.h>

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::InstantiationsReader()
//-----------------------------------------------------------------------------
InstantiationsReader::InstantiationsReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::~InstantiationsReader()
//-----------------------------------------------------------------------------
InstantiationsReader::~InstantiationsReader()
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::createDesignInstantiationFrom()
//-----------------------------------------------------------------------------
QSharedPointer<DesignInstantiation> InstantiationsReader::createDesignInstantiationFrom(
    QDomNode const& instantiationNode) const
{
    QSharedPointer<DesignInstantiation> newInstantiation (new DesignInstantiation());

    NameGroupReader nameReader;
    nameReader.parseNameGroup(instantiationNode, newInstantiation);

    QDomNode designReferenceNode = instantiationNode.firstChildElement("ipxact:designRef");
    newInstantiation->setDesignReference(parseConfigurableVLNVReference(designReferenceNode, VLNV::DESIGN));

    parseVendorExtensions(instantiationNode, newInstantiation);

    return newInstantiation;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::createDesignConfigurationInstantiationFrom()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfigurationInstantiation> InstantiationsReader::createDesignConfigurationInstantiationFrom(
    QDomNode const& instantiationNode) const
{
    QSharedPointer<DesignConfigurationInstantiation> newInstantiation(new DesignConfigurationInstantiation());

    NameGroupReader nameReader;
    nameReader.parseNameGroup(instantiationNode, newInstantiation);

    QDomNode configurationReferenceNode = instantiationNode.firstChildElement("ipxact:designConfigurationRef");
    QSharedPointer<ConfigurableVLNVReference> configurationReference =
        parseConfigurableVLNVReference(configurationReferenceNode, VLNV::DESIGNCONFIGURATION);
    newInstantiation->setDesignConfigurationReference(configurationReference);

    newInstantiation->setLanguage(parseLanguageFrom(instantiationNode));
    newInstantiation->setLanguageStrict(parseLanguageStrictnessFrom(instantiationNode));

    newInstantiation->setParameters(parseAndCreateParameters(instantiationNode));

    parseVendorExtensions(instantiationNode, newInstantiation);

    return newInstantiation;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::getLanguageFrom()
//-----------------------------------------------------------------------------
QString InstantiationsReader::parseLanguageFrom(QDomNode const& instantiationNode) const
{
    QDomElement languageNode = instantiationNode.firstChildElement("ipxact:language");
    return languageNode.firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::getLanguageStrictnessFrom()
//-----------------------------------------------------------------------------
bool InstantiationsReader::parseLanguageStrictnessFrom(QDomNode const& instantiationNode) const
{
    QDomElement languageNode = instantiationNode.firstChildElement("ipxact:language");
    if (!languageNode.attribute("strict").isNull())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::createComponentInstantiationFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> InstantiationsReader::createComponentInstantiationFrom(
    QDomNode const& instantiationNode) const
{
    QSharedPointer<ComponentInstantiation> newInstantiation (new ComponentInstantiation());

    NameGroupReader nameReader;
    nameReader.parseNameGroup(instantiationNode, newInstantiation);

    if (!instantiationNode.firstChildElement("ipxact:isVirtual").isNull())
    {
        newInstantiation->setVirtual(true);
    }

    newInstantiation->setLanguage(parseLanguageFrom(instantiationNode));
    newInstantiation->setLanguageStrictness(parseLanguageStrictnessFrom(instantiationNode));

    parseNameReferences(instantiationNode, newInstantiation);

    parseModuleParameters(instantiationNode, newInstantiation);

    parseDefaultFileBuilders(instantiationNode, newInstantiation);

    parseFileSetReferences(instantiationNode, newInstantiation);

    newInstantiation->setParameters(parseAndCreateParameters(instantiationNode));

    parseVendorExtensions(instantiationNode, newInstantiation);

    return newInstantiation;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::parseNameReferences()
//-----------------------------------------------------------------------------
void InstantiationsReader::parseNameReferences(QDomNode const& instantiationNode,
    QSharedPointer<ComponentInstantiation> instantiation) const
{
    QDomElement libraryElement = instantiationNode.firstChildElement("ipxact:libraryName");
    if (!libraryElement.isNull())
    {
        instantiation->setLibraryName(libraryElement.firstChild().nodeValue());
    }

    QDomElement packageElement = instantiationNode.firstChildElement("ipxact:packageName");
    if (!libraryElement.isNull())
    {
        instantiation->setPackageName(packageElement.firstChild().nodeValue());
    }

    QDomElement moduleElement = instantiationNode.firstChildElement("ipxact:moduleName");
    if (!moduleElement.isNull())
    {
        instantiation->setModuleName(moduleElement.firstChild().nodeValue());
    }

    QDomElement architectureElement = instantiationNode.firstChildElement("ipxact:architectureName");
    if (!architectureElement.isNull())
    {
        instantiation->setArchitectureName(architectureElement.firstChild().nodeValue());
    }

    QDomElement configurationElement = instantiationNode.firstChildElement("ipxact:configurationName");
    if (!configurationElement.isNull())
    {
        instantiation->setConfigurationName(configurationElement.firstChild().nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::parseModuleParameters()
//-----------------------------------------------------------------------------
void InstantiationsReader::parseModuleParameters(QDomNode const& instantiationNode,
    QSharedPointer<ComponentInstantiation> instantiation) const
{
    QDomElement moduleParametersElement = instantiationNode.firstChildElement("ipxact:moduleParameters");
    if (!moduleParametersElement.isNull())
    {
        QDomNodeList moduleParameterNodes = moduleParametersElement.elementsByTagName("ipxact:moduleParameter");

        ModuleParameterReader moduleParameterReader;

        for (int parameterIndex = 0; parameterIndex < moduleParameterNodes.count(); ++parameterIndex)
        {
            QSharedPointer<ModuleParameter> newModuleParameter =
                moduleParameterReader.createModuleParameterFrom(moduleParameterNodes.at(parameterIndex));

            instantiation->getModuleParameters()->append(newModuleParameter);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::parseDefaultFileBuilders()
//-----------------------------------------------------------------------------
void InstantiationsReader::parseDefaultFileBuilders(QDomNode const& instantiationNode,
    QSharedPointer<ComponentInstantiation> instantiation) const
{
    QDomElement instantiationElement = instantiationNode.toElement();

    QDomNodeList defaultFileBuilderNodeList = instantiationElement.elementsByTagName("ipxact:defaultFileBuilder");

    if (!defaultFileBuilderNodeList.isEmpty())
    {
        FileBuilderReader defaultFileBuilderReader;

        for (int i = 0; i < defaultFileBuilderNodeList.count(); ++i)
        {
            QSharedPointer<FileBuilder> defaultFileBuilder =
                defaultFileBuilderReader.createDefaultFileBuilderFrom(defaultFileBuilderNodeList.at(i));
            instantiation->getDefaultFileBuilders()->append(defaultFileBuilder);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::parseFileSetReferences()
//-----------------------------------------------------------------------------
void InstantiationsReader::parseFileSetReferences(QDomNode const& instantiationNode,
    QSharedPointer<ComponentInstantiation> instantiation) const
{
    QDomElement instantiationElement = instantiationNode.toElement();
    
    if (!instantiationElement.isNull())
    {
        QDomNodeList fileSetReferenceNodeList = instantiationElement.elementsByTagName("ipxact:fileSetRef");

        for (int i = 0; i < fileSetReferenceNodeList.count(); ++i)
        {
            QDomNode fileSetNode = fileSetReferenceNodeList.at(i);
            QString referenceName = fileSetNode.firstChildElement("ipxact:localName").firstChild().nodeValue();

            instantiation->getFileSetReferences()->append(referenceName);
        }
    }
}