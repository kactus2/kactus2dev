//-----------------------------------------------------------------------------
// File: ParameterReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2015
//
// Description:
// XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------

#include "ParameterReader.h"

#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: ParameterReader::createParameterFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ParameterReader::createParameterFrom(QDomNode const& parameterNode,
    Document::Revision revision)
{
    QSharedPointer<Parameter> parameter(new Parameter());
    Details::parseAttributes(parameterNode, parameter);

    NameGroupReader::parseNameGroup(parameterNode, parameter);

    Details::parseVectors(parameterNode, parameter, revision);

    Details::parseArrays(parameterNode, parameter);

    Details::parseValue(parameterNode, parameter);

    CommonItemsReader::parseVendorExtensions(parameterNode, parameter);

    return parameter;
}

//-----------------------------------------------------------------------------
// Function: ParameterReader::Details::parseAttributes()
//-----------------------------------------------------------------------------
void ParameterReader::Details::parseAttributes(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter)
{
    QDomNamedNodeMap attributes = parameterNode.attributes();
    for (int j = 0; j < attributes.size(); ++j) 
    {
        QString attributeName = attributes.item(j).nodeName();
        QString attributeValue = attributes.item(j).nodeValue();

        //! Add missing prefix to Kactus2-specific attribute.
        if (attributeName.compare(QLatin1String("usageCount")) == 0)
        {
            attributeName.prepend(QStringLiteral("kactus2:"));
        }

        parameter->setAttribute(attributeName, attributeValue);
    }            
}

//-----------------------------------------------------------------------------
// Function: ParameterReader::Details::parseVectors()
//-----------------------------------------------------------------------------
void ParameterReader::Details::parseVectors(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter,
    Document::Revision revision)
{
    QDomNode vectorsNode = parameterNode.firstChildElement(QStringLiteral("ipxact:vectors"));

    if (vectorsNode.hasChildNodes())
    {
        QSharedPointer<QList<QSharedPointer<Vector> > > vectors = parameter->getVectors();
        
        int vectorCount = vectorsNode.childNodes().size();
        for (int i = 0; i < vectorCount; i++)
        {
            QDomNode vectorNode = vectorsNode.childNodes().at(i);

            QString left = vectorNode.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue();
            QString right = vectorNode.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue();

            auto parsedVector = QSharedPointer<Vector>(new Vector(left, right));
            
            if (revision == Document::Revision::Std22)
            {
                auto vectorId = vectorNode.attributes().namedItem(QStringLiteral("vectorId")).nodeValue();
                parsedVector->setId(vectorId);
            }

            vectors->append(parsedVector);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReader::Details::parseArrays()
//-----------------------------------------------------------------------------
void ParameterReader::Details::parseArrays(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter,
    Document::Revision revision)
{
    QDomNode arraysNode = parameterNode.firstChildElement(QStringLiteral("ipxact:arrays"));

    if (arraysNode.hasChildNodes())
    {
        QSharedPointer<QList<QSharedPointer<Array> > > arrays = parameter->getArrays();

        int arrayCount = arraysNode.childNodes().size();
        for (int i = 0; i < arrayCount; i++)
        {
            QDomNode arrayNode = arraysNode.childNodes().at(i);

            QString left = arrayNode.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue();
            QString right = arrayNode.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue();

            auto newArray = QSharedPointer<Array>(new Array(left, right));

            if (revision == Document::Revision::Std22)
            {
                newArray->setId(arrayNode.attributes().namedItem(QStringLiteral("arrayId")).firstChild().nodeValue());
            }

            arrays->append(newArray);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReader::Details::parseValue()
//-----------------------------------------------------------------------------
void ParameterReader::Details::parseValue(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter)
{
    parameter->setValue(parameterNode.firstChildElement(QStringLiteral("ipxact:value")).firstChild().nodeValue());

    QDomNamedNodeMap valueAttributes = parameterNode.firstChildElement(QStringLiteral("ipxact:value")).attributes();
    for (int j = 0; j < valueAttributes.size(); ++j) 
    {
        QString attributeName = valueAttributes.item(j).nodeName();
        QString attributeValue = valueAttributes.item(j).nodeValue();

        parameter->setValueAttribute(attributeName, attributeValue);
    }
}
