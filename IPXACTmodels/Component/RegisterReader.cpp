//-----------------------------------------------------------------------------
// File: RegisterReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.09.2015
//
// Description:
// Reader for ipxact:register element.
//-----------------------------------------------------------------------------

#include "RegisterReader.h"
#include "RegisterBase.h"
#include "Register.h"
#include "RegisterDefinition.h"
#include "AlternateRegister.h"
#include "RegisterFile.h"
#include "FieldReader.h"
#include "MemoryArrayReader.h"
#include "AccessPolicyReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>

//-----------------------------------------------------------------------------
// Function: RegisterReader::createRegisterfrom()
//-----------------------------------------------------------------------------
QSharedPointer<Register> RegisterReader::createRegisterfrom(QDomNode const& registerNode, Document::Revision docRevision)
{
    QDomElement registerElement = registerNode.toElement();

    QSharedPointer<Register> newRegister(new Register());

    NameGroupReader::parseNameGroup(registerNode, newRegister);

    if (docRevision == Document::Revision::Std14)
    {
        Details::parsePresence(registerNode, newRegister);
        
        Details::parseRegisterDimension(registerNode, newRegister);

        Details::parseAccess(registerNode, newRegister);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        Details::parseRegisterDefinitionReference(registerNode, newRegister);

        Details::parseRegisterMemoryArray(registerNode, newRegister);
    
        Details::parseAccessPolicies(registerNode, newRegister);
    }

    newRegister->setAddressOffset(Details::createAddressOffsetFrom(registerNode));

    Details::parseTypeIdentifier(registerNode, newRegister);

    Details::parseRegisterSize(registerNode, newRegister);

    Details::parseVolatile(registerNode, newRegister);

    Details::parseFields(registerElement, newRegister, docRevision);

    Details::parseAlternateRegisters(registerElement, newRegister, docRevision);

    Details::parseParameters(registerNode, newRegister);

    CommonItemsReader::parseVendorExtensions(registerNode, newRegister);

    return newRegister;
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::createRegisterFileFrom()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterFile> RegisterReader::createRegisterFileFrom(QDomNode const& registerFileNode, Document::Revision docRevision)
{
    QSharedPointer<RegisterFile> newRegisterFile (new RegisterFile());

    NameGroupReader::parseNameGroup(registerFileNode, newRegisterFile);

    if (docRevision == Document::Revision::Std14)
    {
        Details::parsePresence(registerFileNode, newRegisterFile);

        Details::parseRegisterFileDimension(registerFileNode, newRegisterFile);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        Details::parseRegisterFileMemoryArray(registerFileNode, newRegisterFile);

        Details::parseRegisterFileDefinitionReference(registerFileNode, newRegisterFile);

        Details::parseAccessPolicies(registerFileNode, newRegisterFile);
    }

    newRegisterFile->setAddressOffset(Details::createAddressOffsetFrom(registerFileNode));

    Details::parseTypeIdentifier(registerFileNode, newRegisterFile);

    Details::parseRegisterFileRange(registerFileNode, newRegisterFile);

    Details::parseRegisterFileRegisterData(registerFileNode, newRegisterFile, docRevision);

    Details::parseParameters(registerFileNode, newRegisterFile);

    CommonItemsReader::parseVendorExtensions(registerFileNode, newRegisterFile);

    return newRegisterFile;
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseIsPresent()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parsePresence(QDomNode const& registerBaseNode, QSharedPointer<RegisterBase> registerBase)
   
{
    QDomElement isPresentElement = registerBaseNode.firstChildElement(QStringLiteral("ipxact:isPresent"));
    QString isPresent = CommonItemsReader::parseIsPresent(isPresentElement);

    if (!isPresent.isEmpty())
    {
        registerBase->setIsPresent(isPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::createAddressOffset()
//-----------------------------------------------------------------------------
QString RegisterReader::Details::createAddressOffsetFrom(QDomNode const& registerBaseNode)
{
    QDomElement addressOffsetElement = registerBaseNode.firstChildElement(QStringLiteral("ipxact:addressOffset"));
    return addressOffsetElement.firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseRegisterSize()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseRegisterSize(QDomNode const& registerNode, QSharedPointer<Register> selectedRegister)
   
{
    QString registerSize = registerNode.firstChildElement(QStringLiteral("ipxact:size")).firstChild().nodeValue();
    selectedRegister->setSize(registerSize);
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::createRegisterDimension()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseRegisterDimension(QDomNode const& registerNode,
    QSharedPointer<Register> selectedRegister)
{
    auto newMemoryArray = MemoryArrayReader::createMemoryArrayFrom(registerNode, Document::Revision::Std14, false);

    if (!newMemoryArray->getDimensions()->isEmpty())
    {
        selectedRegister->setMemoryArray(newMemoryArray);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseTypeIdentifier()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseTypeIdentifier(QDomNode const& registerBaseNode,
    QSharedPointer<RegisterBase> registerBase)
{
    QDomElement typeIdentifierElement = registerBaseNode.firstChildElement(QStringLiteral("ipxact:typeIdentifier"));
    if (!typeIdentifierElement.isNull())
    {
        QString typeIdentifier = typeIdentifierElement.firstChild().nodeValue();
        registerBase->setTypeIdentifier(typeIdentifier);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseDefinitionVolatile()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseVolatile(QDomNode const& registerDefinitionNode,
    QSharedPointer<RegisterDefinition> registerDefinition)
{
    QDomElement volatileElement = registerDefinitionNode.firstChildElement(QStringLiteral("ipxact:volatile"));
    if (!volatileElement.isNull())
    {
        QString volatileString = volatileElement.firstChild().nodeValue();

        if (volatileString == QLatin1String("true"))
        {
            registerDefinition->setVolatile(true);
        }
        else
        {
            registerDefinition->setVolatile(false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseAccess()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseAccess(QDomNode const& definitionNode,
    QSharedPointer<RegisterDefinition> registerDefinition)
{
    QDomElement accessElement = definitionNode.firstChildElement(QStringLiteral("ipxact:access"));
    if (!accessElement.isNull())
    {
        QString accessString = accessElement.firstChild().nodeValue();
        AccessTypes::Access access = AccessTypes::str2Access(accessString, AccessTypes::ACCESS_COUNT);
        registerDefinition->setAccess(access);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseFields()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseFields(QDomElement const& registerDefinitionElement,
    QSharedPointer<RegisterDefinition> registerDefinition,
    Document::Revision docRevision)
{
    QDomNodeList fieldNodeList = registerDefinitionElement.childNodes();

    if (!fieldNodeList.isEmpty())
    {
        for (int fieldIndex = 0; fieldIndex < fieldNodeList.count(); ++fieldIndex)
        {
            if (fieldNodeList.at(fieldIndex).nodeName() == QLatin1String("ipxact:field"))
            {
                QSharedPointer<Field> newField = FieldReader::createFieldFrom(fieldNodeList.at(fieldIndex), docRevision);
                registerDefinition->getFields()->append(newField);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseAlternateRegisters()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseAlternateRegisters(QDomElement const& registerElement,
    QSharedPointer<Register> targetRegister,
    Document::Revision docRevision)
{
    QDomElement multipleAlternaterRegisterNode = registerElement.firstChildElement(QStringLiteral("ipxact:alternateRegisters"));
    if (!multipleAlternaterRegisterNode.isNull())
    {
        QDomNodeList alternateNodeList =
            multipleAlternaterRegisterNode.elementsByTagName(QStringLiteral("ipxact:alternateRegister"));

        for (int alternateIndex = 0; alternateIndex < alternateNodeList.count(); ++alternateIndex)
        {
            QSharedPointer<AlternateRegister> newAlternateRegister (new AlternateRegister());

            parseSingleAlternateRegister(alternateNodeList.at(alternateIndex).toElement(), newAlternateRegister, docRevision);

            targetRegister->getAlternateRegisters()->append(newAlternateRegister);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseSingleAlternateRegister()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseSingleAlternateRegister(QDomElement const& alternateRegisterElement,
    QSharedPointer<AlternateRegister> newAlternateRegister,
    Document::Revision docRevision)
{
    NameGroupReader::parseNameGroup(alternateRegisterElement, newAlternateRegister);

    if (docRevision == Document::Revision::Std14)
    {
        parsePresence(alternateRegisterElement, newAlternateRegister);

        parseAlternateGroups(alternateRegisterElement, newAlternateRegister);
    
        parseAccess(alternateRegisterElement, newAlternateRegister);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        parseAlternateRegisterModeRefs(alternateRegisterElement, newAlternateRegister);

        parseAccessPolicies(alternateRegisterElement, newAlternateRegister);
    }

    parseTypeIdentifier(alternateRegisterElement, newAlternateRegister);

    parseVolatile(alternateRegisterElement, newAlternateRegister);

    parseFields(alternateRegisterElement, newAlternateRegister, docRevision);

    parseParameters(alternateRegisterElement, newAlternateRegister);

    CommonItemsReader::parseVendorExtensions(alternateRegisterElement, newAlternateRegister);
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseAlternateGroups()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseAlternateGroups(QDomElement const& alternateRegisterElement,
    QSharedPointer<AlternateRegister> newAlternateRegister)
{
    QDomElement groupsElement = alternateRegisterElement.firstChildElement(QStringLiteral("ipxact:alternateGroups"));
    if (!groupsElement.isNull())
    {
        QDomNodeList groupNodeList = groupsElement.elementsByTagName(QStringLiteral("ipxact:alternateGroup"));

        for (int groupIndex = 0; groupIndex < groupNodeList.count(); ++groupIndex)
        {
            QString groupName = groupNodeList.at(groupIndex).firstChild().nodeValue();

            newAlternateRegister->getAlternateGroups()->append(groupName);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseParameters()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseParameters(QDomNode const& registerBaseNode,
    QSharedPointer<RegisterBase> newRegisterBase)
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters = 
        CommonItemsReader::parseAndCreateParameters(registerBaseNode);

    if (!newParameters->isEmpty())
    {
        newRegisterBase->setParameters(newParameters);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseRegisterFileRange()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseRegisterFileRange(QDomNode const& registerFileNode,
    QSharedPointer<RegisterFile> newRegisterFile)
{
    QDomElement rangeElement = registerFileNode.firstChildElement(QStringLiteral("ipxact:range"));
    if (!rangeElement.isNull())
    {
        QString range = rangeElement.firstChild().nodeValue();
        newRegisterFile->setRange(range);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseRegisterFileDimension()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseRegisterFileDimension(QDomNode const& registerFileNode,
    QSharedPointer<RegisterFile> newRegisterFile)
{
    QDomElement dimensionElement = registerFileNode.firstChildElement(QStringLiteral("ipxact:dim"));
    if (!dimensionElement.isNull())
    {
        QString dimension = dimensionElement.firstChild().nodeValue();
        newRegisterFile->setDimension(dimension);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseRegisterFileRegisterData()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseRegisterFileRegisterData(QDomNode const& registerFileNode,
    QSharedPointer<RegisterFile> newRegisterFile,
    Document::Revision docRevision)
{
    QDomNodeList childNodeList = registerFileNode.childNodes();

    for (int i = 0; i < childNodeList.count(); ++i)
    {
        if (childNodeList.at(i).nodeName() == QStringLiteral("ipxact:register"))
        {
            QSharedPointer<Register> newRegister = createRegisterfrom(childNodeList.at(i), docRevision);
            newRegisterFile->getRegisterData()->append(newRegister);
        }

        else if (childNodeList.at(i).nodeName() == QStringLiteral("ipxact:registerFile"))
        {
            QSharedPointer<RegisterFile> containedRegisterFile = createRegisterFileFrom(childNodeList.at(i), docRevision);
            newRegisterFile->getRegisterData()->append(containedRegisterFile);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseRegisterDefinitionReference()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseRegisterDefinitionReference(QDomNode const& registerNode,
    QSharedPointer<Register> newRegister)
{
    auto registerDefinitionReferenceElement = registerNode.firstChildElement(QStringLiteral("ipxact:registerDefinitionRef"));

    if (!registerDefinitionReferenceElement.isNull())
    {
        newRegister->setRegisterDefinitionReference(registerDefinitionReferenceElement.firstChild().nodeValue());

        newRegister->setTypeDefinitionsReference(
            registerDefinitionReferenceElement.attribute(QStringLiteral("typeDefinitions")));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseRegisterMemoryArray()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseRegisterMemoryArray(QDomNode const& registerNode, QSharedPointer<Register> newRegister)
{
    auto memArrayElement = registerNode.firstChildElement(QStringLiteral("ipxact:array"));

    if (!memArrayElement.isNull())
    {
        auto newMemoryArray = MemoryArrayReader::createMemoryArrayFrom(memArrayElement, Document::Revision::Std22, false);

        newRegister->setMemoryArray(newMemoryArray);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseRegisterFileMemoryArray()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseRegisterFileMemoryArray(QDomNode const& registerFileNode, QSharedPointer<RegisterFile> newRegisterFile)
{
    auto memArrayElement = registerFileNode.firstChildElement(QStringLiteral("ipxact:array"));

    if (!memArrayElement.isNull())
    {
        auto newMemoryArray = MemoryArrayReader::createMemoryArrayFrom(memArrayElement, Document::Revision::Std22, false);

        newRegisterFile->setMemoryArray(newMemoryArray);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseRegisterFileDefinitionReference()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseRegisterFileDefinitionReference(QDomNode const& registerFileNode, QSharedPointer<RegisterFile> newRegisterFile)
{
    auto registerDefinitionReferenceElement = registerFileNode.firstChildElement(QStringLiteral("ipxact:registerFileDefinitionRef"));

    if (!registerDefinitionReferenceElement.isNull())
    {
        newRegisterFile->setRegisterFileDefinitionReference(registerDefinitionReferenceElement.firstChild().nodeValue());

        newRegisterFile->setTypeDefinitionsReference(
            registerDefinitionReferenceElement.attribute(QStringLiteral("typeDefinitions")));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseAccessPolicies()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseAccessPolicies(QDomNode const& registerBaseNode, QSharedPointer<RegisterBase> newRegisterBase)
{
    auto accessPoliciesElement = registerBaseNode.firstChildElement(QStringLiteral("ipxact:accessPolicies"));

    if (accessPoliciesElement.isNull())
    {
        return;
    }

    auto accessPolicies = accessPoliciesElement.childNodes();

    for (int i = 0; i < accessPolicies.size(); ++i)
    {
        if (accessPolicies.at(i).nodeName() == QStringLiteral("ipxact:accessPolicy"))
        {
            auto newAccessPolicy = AccessPolicyReader::createAccessPolicyFrom(accessPolicies.at(i));

            newRegisterBase->getAccessPolicies()->append(newAccessPolicy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::Details::parseAlternateRegisterModeRefs()
//-----------------------------------------------------------------------------
void RegisterReader::Details::parseAlternateRegisterModeRefs(QDomNode const& alternateRegisterNode, QSharedPointer<AlternateRegister> newAlternateRegister)
{
    newAlternateRegister->setModeReferences(CommonItemsReader::parseModeReferences(alternateRegisterNode));
}
