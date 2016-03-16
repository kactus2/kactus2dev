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

//-----------------------------------------------------------------------------
// Function: ModuleParameterReader::ModuleParameterReader()
//-----------------------------------------------------------------------------
ModuleParameterReader::ModuleParameterReader(QObject* parent /* = 0 */) :
ParameterReader(parent)
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

    parseVendorExtensions(moduleParameterNode, newModuleParameter);

    parseIsPresent(moduleParameterNode, newModuleParameter);

    return newModuleParameter;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterReader::parseIsPresent()
//-----------------------------------------------------------------------------
void ModuleParameterReader::parseIsPresent(QDomNode const& moduleParameterNode,
    QSharedPointer<ModuleParameter> moduleParameter) const
{
    QDomElement presenceElement = moduleParameterNode.firstChildElement("ipxact:isPresent");

    if (!presenceElement.isNull())
    {
        moduleParameter->setIsPresent(presenceElement.firstChild().nodeValue());
    }
}