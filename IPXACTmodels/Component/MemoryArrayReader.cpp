//-----------------------------------------------------------------------------
// File: MemoryArrayReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 21.7.2023
//
// Description:
// Reader for ixpact:array memory array elements.
//-----------------------------------------------------------------------------

#include "MemoryArrayReader.h"
#include "MemoryArray.h"

//-----------------------------------------------------------------------------
// Function: MemoryArrayReader::createMemoryArrayFrom()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryArray> MemoryArrayReader::createMemoryArrayFrom(QDomNode const& arrayNode, bool isField)
{
    QSharedPointer<MemoryArray> newArray(new MemoryArray());

    Details::parseDimensions(arrayNode, newArray);

    Details::parseStride(arrayNode, newArray, isField);

    return newArray;
}

//-----------------------------------------------------------------------------
// Function: MemoryArrayReader::Details::parseDimensions()
//-----------------------------------------------------------------------------
void MemoryArrayReader::Details::parseDimensions(QDomNode const& arrayNode, QSharedPointer<MemoryArray> newArray)
{
    auto dimensionNodes = arrayNode.childNodes();

    for (int i = 0; i < dimensionNodes.size(); ++i)
    {
        QSharedPointer<MemoryArray::Dimension> newDimension(new MemoryArray::Dimension);

        auto dimensionNode = dimensionNodes.at(i);
        if (dimensionNode.nodeName() == QStringLiteral("ipxact:dim"))
        {
            newDimension->dimension_ = dimensionNode.firstChild().nodeValue();
            newDimension->indexVar_ = dimensionNode.attributes().namedItem(QStringLiteral("indexVar")).nodeValue();

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
