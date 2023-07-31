//-----------------------------------------------------------------------------
// File: ModuleParameterReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.09.2015
//
// Description:
// XML reader for IP-XACT module parameter element.
//-----------------------------------------------------------------------------

#include "ModuleParameterReader.h"
#include "CommonItemsReader.h"
#include "NameGroupReader.h"

//-----------------------------------------------------------------------------
// Function: ModuleParameterReader::createModuleParameterFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ModuleParameter> ModuleParameterReader::createModuleParameterFrom(
    QDomNode const& moduleParameterNode, Document::Revision docRevision)
{
    QSharedPointer<ModuleParameter> newModuleParameter (new ModuleParameter());

    ParameterReader::Details::parseAttributes(moduleParameterNode, newModuleParameter);

    NameGroupReader::parseNameGroup(moduleParameterNode, newModuleParameter);

    ParameterReader::Details::parseVectors(moduleParameterNode, newModuleParameter, docRevision);

    ParameterReader::Details::parseArrays(moduleParameterNode, newModuleParameter, docRevision);

    ParameterReader::Details::parseValue(moduleParameterNode, newModuleParameter);

    CommonItemsReader::parseVendorExtensions(moduleParameterNode, newModuleParameter);

    Details::parseIsPresent(moduleParameterNode, newModuleParameter, docRevision);

    return newModuleParameter;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterReader::parseIsPresent()
//-----------------------------------------------------------------------------
void ModuleParameterReader::Details::parseIsPresent(QDomNode const& moduleParameterNode,
    QSharedPointer<ModuleParameter> moduleParameter, Document::Revision docRevision)
{
    if (docRevision == Document::Revision::Std14)
    {
        QDomElement presenceElement = moduleParameterNode.firstChildElement(QStringLiteral("ipxact:isPresent"));
        moduleParameter->setIsPresent(CommonItemsReader::parseIsPresent(presenceElement));
    }
}