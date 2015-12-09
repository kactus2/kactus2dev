//-----------------------------------------------------------------------------
// File: RegisterReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.09.2015
//
// Description:
// Reader class for ipxact:register element.
//-----------------------------------------------------------------------------

#include "RegisterReader.h"
#include "RegisterBase.h"
#include "Register.h"
#include "RegisterDefinition.h"
#include "AlternateRegister.h"
#include "RegisterFile.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/Component/FieldReader.h>

//-----------------------------------------------------------------------------
// Function: RegisterReader::RegisterReader()
//-----------------------------------------------------------------------------
RegisterReader::RegisterReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: RegisterReader::~RegisterReader()
//-----------------------------------------------------------------------------
RegisterReader::~RegisterReader()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterReader::createRegisterfrom()
//-----------------------------------------------------------------------------
QSharedPointer<Register> RegisterReader::createRegisterfrom(QDomNode const& registerNode) const
{
    QDomElement registerElement = registerNode.toElement();

    QSharedPointer<Register> newRegister(new Register());

    parseNameGroup(registerNode, newRegister);

    parsePresence(registerNode, newRegister);

    parseRegisterDimension(registerNode, newRegister);

    newRegister->setAddressOffset(createAddressOffsetFrom(registerNode));

    parseTypeIdentifier(registerNode, newRegister);

    parseRegisterSize(registerNode, newRegister);

    parseVolatile(registerNode, newRegister);

    parseAccess(registerNode, newRegister);

    parseFields(registerElement, newRegister);

    parseAlternateRegisters(registerElement, newRegister);

    parseParameters(registerNode, newRegister);

    parseVendorExtensions(registerNode, newRegister);

    return newRegister;
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::createRegisterFileFrom()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterFile> RegisterReader::createRegisterFileFrom(QDomNode const& registerFileNode) const
{
    QSharedPointer<RegisterFile> newRegisterFile (new RegisterFile());

    parseNameGroup(registerFileNode, newRegisterFile);

    parsePresence(registerFileNode, newRegisterFile);

    parseRegisterFileDimension(registerFileNode, newRegisterFile);

    newRegisterFile->setAddressOffset(createAddressOffsetFrom(registerFileNode));

    parseTypeIdentifier(registerFileNode, newRegisterFile);

    parseRegisterFileRange(registerFileNode, newRegisterFile);

    parseRegisterFileRegisterData(registerFileNode, newRegisterFile);

    parseParameters(registerFileNode, newRegisterFile);

    parseVendorExtensions(registerFileNode, newRegisterFile);

    return newRegisterFile;
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseNameGroup()
//-----------------------------------------------------------------------------
void RegisterReader::parseNameGroup(QDomNode const& nameGroupNode, QSharedPointer<RegisterBase> registerData) const
{
    NameGroupReader nameGroupReader;
    nameGroupReader.parseNameGroup(nameGroupNode, registerData);
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseIsPresent()
//-----------------------------------------------------------------------------
void RegisterReader::parsePresence(QDomNode const& registerBaseNode, QSharedPointer<RegisterBase> registerBase)
    const
{
    QDomElement isPresentElement = registerBaseNode.firstChildElement("ipxact:isPresent");
    QString isPresent = parseIsPresent(isPresentElement);

    if (!isPresent.isEmpty())
    {
        registerBase->setIsPresent(isPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::createAddressOffset()
//-----------------------------------------------------------------------------
QString RegisterReader::createAddressOffsetFrom(QDomNode const& registerBaseNode) const
{
    QDomElement addressOffsetElement = registerBaseNode.firstChildElement("ipxact:addressOffset");
    return addressOffsetElement.firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseRegisterSize()
//-----------------------------------------------------------------------------
void RegisterReader::parseRegisterSize(QDomNode const& registerNode, QSharedPointer<Register> selectedRegister)
    const
{
    QString registerSize = registerNode.firstChildElement("ipxact:size").firstChild().nodeValue();
    selectedRegister->setSize(registerSize);
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::createRegisterDimension()
//-----------------------------------------------------------------------------
void RegisterReader::parseRegisterDimension(QDomNode const& registerNode,
    QSharedPointer<Register> selectedRegister) const
{
    QDomElement dimensionElement = registerNode.firstChildElement("ipxact:dim");
    if (!dimensionElement.isNull())
    {
        selectedRegister->setDimension(dimensionElement.firstChild().nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseTypeIdentifier()
//-----------------------------------------------------------------------------
void RegisterReader::parseTypeIdentifier(QDomNode const& registerBaseNode,
    QSharedPointer<RegisterBase> registerBase) const
{
    QDomElement typeIdentifierElement = registerBaseNode.firstChildElement("ipxact:typeIdentifier");
    if (!typeIdentifierElement.isNull())
    {
        QString typeIdentifier = typeIdentifierElement.firstChild().nodeValue();
        registerBase->setTypeIdentifier(typeIdentifier);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseDefinitionVolatile()
//-----------------------------------------------------------------------------
void RegisterReader::parseVolatile(QDomNode const& registerDefinitionNode,
    QSharedPointer<RegisterDefinition> registerDefinition) const
{
    QDomElement volatileElement = registerDefinitionNode.firstChildElement("ipxact:volatile");
    if (!volatileElement.isNull())
    {
        QString volatileString = volatileElement.firstChild().nodeValue();

        if (volatileString=="true")
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
// Function: RegisterReader::parseAccess()
//-----------------------------------------------------------------------------
void RegisterReader::parseAccess(QDomNode const& definitionNode,
    QSharedPointer<RegisterDefinition> registerDefinition) const
{
    QDomElement accessElement = definitionNode.firstChildElement("ipxact:access");
    if (!accessElement.isNull())
    {
        QString accessString = accessElement.firstChild().nodeValue();
        AccessTypes::Access access = AccessTypes::str2Access(accessString, AccessTypes::ACCESS_COUNT);
        registerDefinition->setAccess(access);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseFields()
//-----------------------------------------------------------------------------
void RegisterReader::parseFields(QDomElement const& registerDefinitionElement,
    QSharedPointer<RegisterDefinition> registerDefinition) const
{
    QDomNodeList fieldNodeList = registerDefinitionElement.childNodes();

    if (!fieldNodeList.isEmpty())
    {
        FieldReader fieldReader;

        for (int fieldIndex = 0; fieldIndex < fieldNodeList.count(); ++fieldIndex)
        {
            if (fieldNodeList.at(fieldIndex).nodeName() == "ipxact:field")
            {
                QSharedPointer<Field> newField = fieldReader.createFieldFrom(fieldNodeList.at(fieldIndex));
                registerDefinition->getFields()->append(newField);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseAlternateRegisters()
//-----------------------------------------------------------------------------
void RegisterReader::parseAlternateRegisters(QDomElement const& registerElement,
    QSharedPointer<Register> targetRegister) const
{
    QDomElement multipleAlternaterRegisterNode = registerElement.firstChildElement("ipxact:alternateRegisters");
    if (!multipleAlternaterRegisterNode.isNull())
    {
        QDomNodeList alternateNodeList =
            multipleAlternaterRegisterNode.elementsByTagName("ipxact:alternateRegister");

        for (int alternateIndex = 0; alternateIndex < alternateNodeList.count(); ++alternateIndex)
        {
            QSharedPointer<AlternateRegister> newAlternateRegister (new AlternateRegister());

            parseSingleAlternateRegister(alternateNodeList.at(alternateIndex).toElement(), newAlternateRegister);

            targetRegister->getAlternateRegisters()->append(newAlternateRegister);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseSingleAlternateRegister()
//-----------------------------------------------------------------------------
void RegisterReader::parseSingleAlternateRegister(QDomElement const& alternateRegisterElement,
    QSharedPointer<AlternateRegister> newAlternateRegister) const
{
    parseNameGroup(alternateRegisterElement, newAlternateRegister);

    parsePresence(alternateRegisterElement, newAlternateRegister);

    parseAlternateGroups(alternateRegisterElement, newAlternateRegister);

    parseTypeIdentifier(alternateRegisterElement, newAlternateRegister);

    parseVolatile(alternateRegisterElement, newAlternateRegister);

    parseAccess(alternateRegisterElement, newAlternateRegister);

    parseFields(alternateRegisterElement, newAlternateRegister);

    parseParameters(alternateRegisterElement, newAlternateRegister);

    parseVendorExtensions(alternateRegisterElement, newAlternateRegister);
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseAlternateGroups()
//-----------------------------------------------------------------------------
void RegisterReader::parseAlternateGroups(QDomElement const& alternateRegisterElement,
    QSharedPointer<AlternateRegister> newAlternateRegister) const
{
    QDomElement groupsElement = alternateRegisterElement.firstChildElement("ipxact:alternateGroups");
    if (!groupsElement.isNull())
    {
        QDomNodeList groupNodeList = groupsElement.elementsByTagName("ipxact:alternateGroup");

        for (int groupIndex = 0; groupIndex < groupNodeList.count(); ++groupIndex)
        {
            QString groupName = groupNodeList.at(groupIndex).firstChild().nodeValue();

            newAlternateRegister->getAlternateGroups()->append(groupName);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseParameters()
//-----------------------------------------------------------------------------
void RegisterReader::parseParameters(QDomNode const& registerBaseNode,
    QSharedPointer<RegisterBase> newRegisterBase) const
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters = parseAndCreateParameters(registerBaseNode);

    if (!newParameters->isEmpty())
    {
        newRegisterBase->setParameters(newParameters);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseRegisterFileRange()
//-----------------------------------------------------------------------------
void RegisterReader::parseRegisterFileRange(QDomNode const& registerFileNode,
    QSharedPointer<RegisterFile> newRegisterFile) const
{
    QDomElement rangeElement = registerFileNode.firstChildElement("ipxact:range");
    if (!rangeElement.isNull())
    {
        QString range = rangeElement.firstChild().nodeValue();
        newRegisterFile->setRange(range);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseRegisterFileDimension()
//-----------------------------------------------------------------------------
void RegisterReader::parseRegisterFileDimension(QDomNode const& registerFileNode,
    QSharedPointer<RegisterFile> newRegisterFile) const
{
    QDomElement dimensionElement = registerFileNode.firstChildElement("ipxact:dim");
    if (!dimensionElement.isNull())
    {
        QString dimension = dimensionElement.firstChild().nodeValue();
        newRegisterFile->setDimension(dimension);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterReader::parseRegisterFileRegisterData()
//-----------------------------------------------------------------------------
void RegisterReader::parseRegisterFileRegisterData(QDomNode const& registerFileNode,
    QSharedPointer<RegisterFile> newRegisterFile) const
{
    QDomNodeList childNodeList = registerFileNode.childNodes();

    for (int i = 0; i < childNodeList.count(); ++i)
    {
        if (childNodeList.at(i).nodeName() == "ipxact:register")
        {
            QSharedPointer<Register> newRegister = createRegisterfrom(childNodeList.at(i));
            newRegisterFile->getRegisterData()->append(newRegister);
        }

        else if (childNodeList.at(i).nodeName() == "ipxact:registerFile")
        {
            QSharedPointer<RegisterFile> containedRegisterFile = createRegisterFileFrom(childNodeList.at(i));
            newRegisterFile->getRegisterData()->append(containedRegisterFile);
        }
    }
}