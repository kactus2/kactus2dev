//-----------------------------------------------------------------------------
// File: FieldReferenceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 1.8.2023
//
// Description:
// Reads a field reference group from IP-XACT XML.
//-----------------------------------------------------------------------------

#include "FieldReferenceReader.h"

//-----------------------------------------------------------------------------
// Function: FieldReferenceReader::createFieldReferenceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<FieldReference> FieldReferenceReader::createFieldReferenceFrom(QDomNode const& groupNode)
{
    QSharedPointer<FieldReference> newFieldReference(new FieldReference());

    auto referenceElementNodes = groupNode.childNodes();

    for (int i = 0; i < referenceElementNodes.size(); ++i)
    {
        auto currentNode = referenceElementNodes.at(i);

        Details::readSingleReferenceElement(referenceElementNodes.at(i), newFieldReference);
    }

    return newFieldReference;
}

//-----------------------------------------------------------------------------
// Function: FieldReferenceReader::Details::readSingleReferenceElement()
//-----------------------------------------------------------------------------
void FieldReferenceReader::Details::readSingleReferenceElement(QDomNode const& currentNode, QSharedPointer<FieldReference> newFieldReference)
{
    auto nodeName = currentNode.nodeName().split(QStringLiteral(":")).back();
    auto refType = FieldReference::str2Type(nodeName);
    auto refValue = currentNode.attributes().namedItem(nodeName).nodeValue();

    QSharedPointer<FieldReference::IndexedReference> newFieldRefElement(new FieldReference::IndexedReference());

    if (refValue.isEmpty())
    {
        return;
    }

    newFieldRefElement->reference_ = refValue;

    // Read possible indices.
    if (currentNode.hasChildNodes())
    {
        Details::readReferenceIndices(currentNode, newFieldRefElement);
    }

    newFieldReference->setReference(newFieldRefElement, refType);
}

//-----------------------------------------------------------------------------
// Function: FieldReferenceReader::Details::readReferenceIndices()
//-----------------------------------------------------------------------------
void FieldReferenceReader::Details::readReferenceIndices(QDomNode const& referenceNode, QSharedPointer<FieldReference::IndexedReference> newFieldRefElement)
{
    auto indicesNode = referenceNode.firstChildElement(QStringLiteral("ipxact:indices"));
    auto const& indexNodes = indicesNode.childNodes();

    for (auto i = 0; i < indexNodes.size(); ++i)
    {
        auto currentIndexNode = indexNodes.at(i);

        if (currentIndexNode.nodeName() == QStringLiteral("ipxact:index"))
        {
            newFieldRefElement->indices_.append(currentIndexNode.firstChild().nodeValue());
        }
    }
}
