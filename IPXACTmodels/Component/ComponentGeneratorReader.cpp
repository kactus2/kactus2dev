//-----------------------------------------------------------------------------
// File: ComponentGeneratorReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Reader for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------

#include "ComponentGeneratorReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/ParameterReader.h>

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::createComponentGeneratorFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentGenerator> ComponentGeneratorReader::createComponentGeneratorFrom(
    QDomNode const& componentGeneratorNode, Document::Revision docRevision)
{
	QSharedPointer<ComponentGenerator> newComponentGenerator (new ComponentGenerator());

	NameGroupReader::parseNameGroup(componentGeneratorNode, newComponentGenerator);

	Details::readAttributes(componentGeneratorNode, newComponentGenerator);

    if (auto const& phaseElement = componentGeneratorNode.firstChildElement(QStringLiteral("ipxact:phase")); 
        !phaseElement.isNull())
    {
        newComponentGenerator->setPhase(phaseElement.firstChild().nodeValue());
    }

    Details::readParameters(componentGeneratorNode.firstChildElement(QStringLiteral("ipxact:parameters")), newComponentGenerator, docRevision);
        
    Details::readApiType(componentGeneratorNode.firstChildElement(QStringLiteral("ipxact:apiType")), newComponentGenerator);
   
    if (docRevision == Document::Revision::Std22)
    {
        Details::readApiService(componentGeneratorNode.firstChildElement(QStringLiteral("ipxact:apiService")), newComponentGenerator);
    }

    Details::readTransportMethods(componentGeneratorNode.firstChildElement(QStringLiteral("ipxact:transportMethods")), newComponentGenerator);

    if (auto const& exeElement = componentGeneratorNode.firstChildElement(QStringLiteral("ipxact:generatorExe")); 
        !exeElement.isNull())
    {
        newComponentGenerator->setGeneratorExe(exeElement.firstChild().nodeValue());
    }

    CommonItemsReader::parseVendorExtensions(componentGeneratorNode, newComponentGenerator);

    Details::parseGroups(componentGeneratorNode, newComponentGenerator);

    return newComponentGenerator;
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::Details::readAttributes()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::Details::readAttributes(QDomNode const& componentGeneratorNode,
	QSharedPointer<ComponentGenerator> newComponentGenerator)
{
	QDomNamedNodeMap attributeMap = componentGeneratorNode.attributes();

	QString scope = attributeMap.namedItem(QStringLiteral("scope")).nodeValue();
	if (scope == QLatin1String("entity"))
	{
		newComponentGenerator->setScope(ComponentGenerator::Scope::ENTITY);
	}
	else if (scope == QLatin1String("instance"))
	{
		newComponentGenerator->setScope(ComponentGenerator::Scope::INSTANCE);
	}

    QString hidden = attributeMap.namedItem(QStringLiteral("hidden")).firstChild().nodeValue();
    if (hidden == QLatin1String("true"))
    {
        newComponentGenerator->setHidden(true);
    }
    else if (hidden == QLatin1String("false"))
    {
        newComponentGenerator->setHidden(false);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::Details::readParameters()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::Details::readParameters(QDomNode const& parametersNode, 
    QSharedPointer<ComponentGenerator> componentGenerator, 
    Document::Revision docRevision)
{
    int parameterCount = parametersNode.childNodes().count();
	for (int i = 0; i < parameterCount; i++)
	{
		QDomNode parameterNode = parametersNode.childNodes().at(i);
		componentGenerator->getParameters()->append(ParameterReader::createParameterFrom(parameterNode, docRevision));
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::Details::readApiType()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::Details::readApiType(QDomNode const& apiTypeNode,
	QSharedPointer<ComponentGenerator> newComponentGenerator)
{
	QString apiType = apiTypeNode.firstChild().nodeValue();
	if (apiType == QLatin1String("TGI_2014_BASE"))
	{
		newComponentGenerator->setApiType(ComponentGenerator::ApiType::TGI_2014_BASE);
	}
	else if (apiType == QLatin1String("TGI_2014_EXTENDED"))
	{
		newComponentGenerator->setApiType(ComponentGenerator::ApiType::TGI_2014_EXTENDED);
	}
	else if (apiType == QLatin1String("TGI_2009"))
	{
		newComponentGenerator->setApiType(ComponentGenerator::ApiType::TGI_2009);
	}
    else if (apiType == QLatin1String("none"))
    {
        newComponentGenerator->setApiType(ComponentGenerator::ApiType::NONE);
    }
    else if (apiType == QLatin1String("TGI_2022_BASE"))
    {
        newComponentGenerator->setApiType(ComponentGenerator::ApiType::TGI_2022_BASE);
    }
    else if (apiType == QLatin1String("TBGI_2022_EXTENDED"))
    {
        newComponentGenerator->setApiType(ComponentGenerator::ApiType::TBGI_2022_EXTENDED);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::Details::readApiService()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::Details::readApiService(QDomNode const& apiServiceNode, 
    QSharedPointer<ComponentGenerator> newComponentGenerator)
{
    if (!apiServiceNode.isNull())
    {
        newComponentGenerator->setApiService(apiServiceNode.firstChild().nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::Details::readTransportMethods()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::Details::readTransportMethods(QDomElement const& methodsNode, 
    QSharedPointer<ComponentGenerator> newComponentGenerator)
{
    if (methodsNode.isNull())
    {
        return;
    }

    QStringList transportMethods;
    int methodsCount = methodsNode.childNodes().count();
    for (int i = 0; i < methodsCount; i++)
    {
        QDomNode methodNode = methodsNode.childNodes().at(i);
        transportMethods.append(methodNode.firstChild().nodeValue());
    }

    newComponentGenerator->setTransportMethods(transportMethods);
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::Details::parseGroups()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::Details::parseGroups(QDomNode const& componentGeneratorNode, 
    QSharedPointer<ComponentGenerator> newComponentGenerator)
{
    QDomNodeList groupNodes = componentGeneratorNode.toElement().elementsByTagName(QStringLiteral("ipxact:group"));

    QStringList groupNames;
    int groupCount = groupNodes.count();
    for (int i = 0; i < groupCount; i++)
    {
        groupNames.append(groupNodes.at(i).firstChild().nodeValue());
    }

    newComponentGenerator->setGroups(groupNames);
}
