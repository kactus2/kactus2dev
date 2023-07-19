//-----------------------------------------------------------------------------
// File: InstantiationsReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Reader for IP-XACT instantiations.
//-----------------------------------------------------------------------------

#include "InstantiationsReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/FileBuilderReader.h>

#include <IPXACTmodels/common/ModuleParameterReader.h>

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::createDesignInstantiationFrom()
//-----------------------------------------------------------------------------
QSharedPointer<DesignInstantiation> InstantiationsReader::createDesignInstantiationFrom(
    QDomNode const& instantiationNode)
{
    QSharedPointer<DesignInstantiation> newInstantiation (new DesignInstantiation());

    NameGroupReader::parseNameGroup(instantiationNode, newInstantiation);

    QDomNode designReferenceNode = instantiationNode.firstChildElement(QStringLiteral("ipxact:designRef"));
    newInstantiation->setDesignReference(CommonItemsReader::parseConfigurableVLNVReference(designReferenceNode, VLNV::DESIGN));

    CommonItemsReader::parseVendorExtensions(instantiationNode, newInstantiation);

    return newInstantiation;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::createDesignConfigurationInstantiationFrom()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfigurationInstantiation> InstantiationsReader::createDesignConfigurationInstantiationFrom(
    QDomNode const& instantiationNode, Document::Revision docRevision)
{
    QSharedPointer<DesignConfigurationInstantiation> newInstantiation(new DesignConfigurationInstantiation());

    NameGroupReader::parseNameGroup(instantiationNode, newInstantiation);

    QDomNode configurationReferenceNode = instantiationNode.firstChildElement(
        QStringLiteral("ipxact:designConfigurationRef"));
    QSharedPointer<ConfigurableVLNVReference> configurationReference =
        CommonItemsReader::parseConfigurableVLNVReference(configurationReferenceNode, VLNV::DESIGNCONFIGURATION);
    newInstantiation->setDesignConfigurationReference(configurationReference);

    newInstantiation->setLanguage(Details::parseLanguageFrom(instantiationNode));
    newInstantiation->setLanguageStrict(Details::parseLanguageStrictnessFrom(instantiationNode));

    newInstantiation->setParameters(CommonItemsReader::parseAndCreateParameters(instantiationNode, docRevision));

    CommonItemsReader::parseVendorExtensions(instantiationNode, newInstantiation);

    return newInstantiation;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::Details::getLanguageFrom()
//-----------------------------------------------------------------------------
QString InstantiationsReader::Details::parseLanguageFrom(QDomNode const& instantiationNode)
{
    QDomElement languageNode = instantiationNode.firstChildElement(QStringLiteral("ipxact:language"));
    return languageNode.firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::Details::getLanguageStrictnessFrom()
//-----------------------------------------------------------------------------
bool InstantiationsReader::Details::parseLanguageStrictnessFrom(QDomNode const& instantiationNode)
{
    QDomElement languageNode = instantiationNode.firstChildElement(QStringLiteral("ipxact:language"));
    if (!languageNode.attribute(QStringLiteral("strict")).isNull())
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
    QDomNode const& instantiationNode, Document::Revision docRevision)
{
    QSharedPointer<ComponentInstantiation> newInstantiation (new ComponentInstantiation());

    NameGroupReader::parseNameGroup(instantiationNode, newInstantiation);

    if (!instantiationNode.firstChildElement(QStringLiteral("ipxact:isVirtual")).isNull())
    {
        newInstantiation->setVirtual(true);
    }

    newInstantiation->setLanguage(Details::parseLanguageFrom(instantiationNode));
    newInstantiation->setLanguageStrictness(Details::parseLanguageStrictnessFrom(instantiationNode));

    Details::parseNameReferences(instantiationNode, newInstantiation);

    Details::parseModuleParameters(instantiationNode, newInstantiation, docRevision);

    Details::parseDefaultFileBuilders(instantiationNode, newInstantiation, docRevision);

    Details::parseFileSetReferences(instantiationNode, newInstantiation);

    newInstantiation->setParameters(CommonItemsReader::parseAndCreateParameters(instantiationNode, docRevision));

    CommonItemsReader::parseVendorExtensions(instantiationNode, newInstantiation);

    return newInstantiation;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::Details::parseNameReferences()
//-----------------------------------------------------------------------------
void InstantiationsReader::Details::parseNameReferences(QDomNode const& instantiationNode,
    QSharedPointer<ComponentInstantiation> instantiation)
{
    QDomElement libraryElement = instantiationNode.firstChildElement(QStringLiteral("ipxact:libraryName"));
    if (!libraryElement.isNull())
    {
        instantiation->setLibraryName(libraryElement.firstChild().nodeValue());
    }

    QDomElement packageElement = instantiationNode.firstChildElement(QStringLiteral("ipxact:packageName"));
    if (!libraryElement.isNull())
    {
        instantiation->setPackageName(packageElement.firstChild().nodeValue());
    }

    QDomElement moduleElement = instantiationNode.firstChildElement(QStringLiteral("ipxact:moduleName"));
    if (!moduleElement.isNull())
    {
        instantiation->setModuleName(moduleElement.firstChild().nodeValue());
    }

    QDomElement architectureElement = instantiationNode.firstChildElement(QStringLiteral("ipxact:architectureName"));
    if (!architectureElement.isNull())
    {
        instantiation->setArchitectureName(architectureElement.firstChild().nodeValue());
    }

    QDomElement configurationElement = instantiationNode.firstChildElement(QStringLiteral("ipxact:configurationName"));
    if (!configurationElement.isNull())
    {
        instantiation->setConfigurationName(configurationElement.firstChild().nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::Details::parseModuleParameters()
//-----------------------------------------------------------------------------
void InstantiationsReader::Details::parseModuleParameters(QDomNode const& instantiationNode,
    QSharedPointer<ComponentInstantiation> instantiation, Document::Revision docRevision)
{
    QDomElement moduleParametersElement = instantiationNode.firstChildElement(QStringLiteral("ipxact:moduleParameters"));
    if (!moduleParametersElement.isNull())
    {
        QDomNodeList moduleParameterNodes = moduleParametersElement.elementsByTagName(QStringLiteral("ipxact:moduleParameter"));

        for (int parameterIndex = 0; parameterIndex < moduleParameterNodes.count(); ++parameterIndex)
        {
            QSharedPointer<ModuleParameter> newModuleParameter =
                ModuleParameterReader::createModuleParameterFrom(moduleParameterNodes.at(parameterIndex), docRevision);

            instantiation->getModuleParameters()->append(newModuleParameter);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::Details::parseDefaultFileBuilders()
//-----------------------------------------------------------------------------
void InstantiationsReader::Details::parseDefaultFileBuilders(QDomNode const& instantiationNode,
    QSharedPointer<ComponentInstantiation> instantiation,
    Document::Revision docRevision)
{
    QDomElement instantiationElement = instantiationNode.toElement();

    QDomNodeList defaultFileBuilderNodeList = instantiationElement.elementsByTagName(QStringLiteral("ipxact:defaultFileBuilder"));

    if (!defaultFileBuilderNodeList.isEmpty())
    {
        for (int i = 0; i < defaultFileBuilderNodeList.count(); ++i)
        {
            QSharedPointer<FileBuilder> defaultFileBuilder =
                FileBuilderReader::createFileBuilderFrom(defaultFileBuilderNodeList.at(i), docRevision);
            instantiation->getDefaultFileBuilders()->append(defaultFileBuilder);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::Details::parseFileSetReferences()
//-----------------------------------------------------------------------------
void InstantiationsReader::Details::parseFileSetReferences(QDomNode const& instantiationNode,
    QSharedPointer<ComponentInstantiation> instantiation)
{
    QDomElement instantiationElement = instantiationNode.toElement();
    
    if (!instantiationElement.isNull())
    {
        QDomNodeList fileSetReferenceNodeList = instantiationElement.elementsByTagName(QStringLiteral("ipxact:fileSetRef"));

        for (int i = 0; i < fileSetReferenceNodeList.count(); ++i)
        {
            QDomNode fileSetNode = fileSetReferenceNodeList.at(i);
            QString referenceName = fileSetNode.firstChildElement(QStringLiteral("ipxact:localName")).firstChild().nodeValue();

            instantiation->getFileSetReferences()->append(referenceName);
        }
    }
}