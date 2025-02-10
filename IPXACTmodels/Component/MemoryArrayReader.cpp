//-----------------------------------------------------------------------------
// File: MemoryArrayReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 21.7.2023
//
// Description:
// Reader for ixpact:array memory array elements and 2014 standard dim elements.
//-----------------------------------------------------------------------------

#include "MemoryArrayReader.h"
#include "MemoryArray.h"

//-----------------------------------------------------------------------------
// Function: MemoryArrayReader::createMemoryArrayFrom()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryArray> MemoryArrayReader::createMemoryArrayFrom(QDomNode const& arrayNode,
    Document::Revision docRevision, bool isField)
{
    QSharedPointer<MemoryArray> newArray(new MemoryArray());

    Details::parseDimensions(arrayNode, newArray, docRevision);

    if (docRevision == Document::Revision::Std22)
    {
        Details::parseStride(arrayNode, newArray, isField);
    }

    return newArray;
}

//-----------------------------------------------------------------------------
// Function: MemoryArrayReader::Details::parseDimensions()
//-----------------------------------------------------------------------------
void MemoryArrayReader::Details::parseDimensions(QDomNode const& arrayNode, QSharedPointer<MemoryArray> newArray, Document::Revision docRevision)
{
    auto dimensionNodes = arrayNode.childNodes();

    for (int i = 0; i < dimensionNodes.size(); ++i)
    {
        QSharedPointer<MemoryArray::Dimension> newDimension(new MemoryArray::Dimension);

        auto dimensionNode = dimensionNodes.at(i);
        if (dimensionNode.nodeName() == QStringLiteral("ipxact:dim"))
        {
            newDimension->value_ = dimensionNode.firstChild().nodeValue();

            if (docRevision == Document::Revision::Std22)
            {
                newDimension->indexVar_ = dimensionNode.attributes().namedItem(QStringLiteral("indexVar")).nodeValue();
            }

            newArray->getDimensions()->append(newDimension);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryArrayReader::Details::parseStride()
//-----------------------------------------------------------------------------
void MemoryArrayReader::Details::parseStride(QDomNode const& arrayNode, QSharedPointer<MemoryArray> newArray, bool isField)
{
    QString nodeName = isField ? QStringLiteral("ipxact:bitStride") : QStringLiteral("ipxact:stride");
    
    auto strideNode = arrayNode.firstChildElement(nodeName);
    newArray->setStride(strideNode.firstChild().nodeValue());
}
