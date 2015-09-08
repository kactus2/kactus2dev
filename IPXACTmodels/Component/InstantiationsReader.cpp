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

    newInstantiation->setLanguage(getLanguageFrom(instantiationNode));
    newInstantiation->setLanguageStrict(getLanguageStrictnessFrom(instantiationNode));

    newInstantiation->setParameters(parseAndCreateParameters(instantiationNode));

    parseVendorExtensions(instantiationNode, newInstantiation);

    return newInstantiation;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::getLanguageFrom()
//-----------------------------------------------------------------------------
QString InstantiationsReader::getLanguageFrom(QDomNode const& instantiationNode) const
{
    QDomElement languageNode = instantiationNode.firstChildElement("ipxact:language");
    return languageNode.firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: InstantiationsReader::getLanguageStrictnessFrom()
//-----------------------------------------------------------------------------
bool InstantiationsReader::getLanguageStrictnessFrom(QDomNode const& instantiationNode) const
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