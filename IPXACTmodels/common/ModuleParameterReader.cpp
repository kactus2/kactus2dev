//-----------------------------------------------------------------------------
// File: ModuleParameterReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.09.2015
//
// Description:
// XML reader class for IP-XACT module parameter element.
//-----------------------------------------------------------------------------

#include "ModuleParameterReader.h"
#include "CommonItemsReader.h"

//-----------------------------------------------------------------------------
// Function: ModuleParameterReader::ModuleParameterReader()
//-----------------------------------------------------------------------------
ModuleParameterReader::ModuleParameterReader() :
ParameterReader()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterReader::~ModuleParameterReader()
//-----------------------------------------------------------------------------
ModuleParameterReader::~ModuleParameterReader()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterReader::createModuleParameterFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ModuleParameter> ModuleParameterReader::createModuleParameterFrom(
    QDomNode const& moduleParameterNode) const
{
    QSharedPointer<ModuleParameter> newModuleParameter (new ModuleParameter());

    parseAttributes(moduleParameterNode, newModuleParameter);

    parseNameGroup(moduleParameterNode, newModuleParameter);

    parseVectors(moduleParameterNode, newModuleParameter);

    parseArrays(moduleParameterNode, newModuleParameter);

    parseValue(moduleParameterNode, newModuleParameter);

    CommonItemsReader::parseVendorExtensions(moduleParameterNode, newModuleParameter);

    parseIsPresent(moduleParameterNode, newModuleParameter);

    return newModuleParameter;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterReader::parseIsPresent()
//-----------------------------------------------------------------------------
void ModuleParameterReader::parseIsPresent(QDomNode const& moduleParameterNode,
    QSharedPointer<ModuleParameter> moduleParameter) const
{
    QDomElement presenceElement = moduleParameterNode.firstChildElement(QStringLiteral("ipxact:isPresent"));

    moduleParameter->setIsPresent(CommonItemsReader::parseIsPresent(presenceElement));
}