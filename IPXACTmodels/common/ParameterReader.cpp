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

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

//-----------------------------------------------------------------------------
// Function: ParameterReader::ParameterReader()
//-----------------------------------------------------------------------------
ParameterReader::ParameterReader(QObject *parent) : QObject(parent)
{
    
}

//-----------------------------------------------------------------------------
// Function: ParameterReader::~ParameterReader()
//-----------------------------------------------------------------------------
ParameterReader::~ParameterReader()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterReader::createParameterFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ParameterReader::createParameterFrom(QDomNode const& parameterNode) const
{
    QSharedPointer<Parameter> parameter(new Parameter());
    parseAttributes(parameterNode, parameter);

    parseNameGroup(parameterNode, parameter);

    parseVectors(parameterNode, parameter);

    parseArrays(parameterNode, parameter);

    parseValue(parameterNode, parameter);

    parseVendorExtensions(parameterNode, parameter);

    return parameter;
}

//-----------------------------------------------------------------------------
// Function: ParameterReader::parseAttributes()
//-----------------------------------------------------------------------------
void ParameterReader::parseAttributes(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter) const
{
    QDomNamedNodeMap attributes = parameterNode.attributes();
    for (int j = 0; j < attributes.size(); ++j) 
    {
        QString attributeName = attributes.item(j).nodeName();
        QString attributeValue = attributes.item(j).nodeValue();

        parameter->setAttribute(attributeName, attributeValue);
    }            
}

//-----------------------------------------------------------------------------
// Function: ParameterReader::parseNameGroup()
//-----------------------------------------------------------------------------
void ParameterReader::parseNameGroup(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter) const
{
    NameGroupReader nameReader;
    nameReader.parseNameGroup(parameterNode, parameter);
}

//-----------------------------------------------------------------------------
// Function: ParameterReader::parseVectors()
//-----------------------------------------------------------------------------
void ParameterReader::parseVectors(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter) const
{
    QDomNode vectorsNode = parameterNode.firstChildElement("ipxact:vectors");

    if (vectorsNode.hasChildNodes())
    {
        QSharedPointer<QList<QSharedPointer<Vector> > > vectors = parameter->getVectors();
        
        int vectorCount = vectorsNode.childNodes().size();
        for (int i = 0; i < vectorCount; i++)
        {
            QDomNode vectorNode = vectorsNode.childNodes().at(i);

            QString left = vectorNode.firstChildElement("ipxact:left").firstChild().nodeValue();
            QString right = vectorNode.firstChildElement("ipxact:right").firstChild().nodeValue();

            vectors->append(QSharedPointer<Vector>(new Vector(left, right)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReader::parseArrays()
//-----------------------------------------------------------------------------
void ParameterReader::parseArrays(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter) const
{
    QDomNode arraysNode = parameterNode.firstChildElement("ipxact:arrays");

    if (arraysNode.hasChildNodes())
    {
        QSharedPointer<QList<QSharedPointer<Array> > > arrays = parameter->getArrays();

        int arrayCount = arraysNode.childNodes().size();
        for (int i = 0; i < arrayCount; i++)
        {
            QDomNode arrayNode = arraysNode.childNodes().at(i);

            QString left = arrayNode.firstChildElement("ipxact:left").firstChild().nodeValue();
            QString right = arrayNode.firstChildElement("ipxact:right").firstChild().nodeValue();

            arrays->append(QSharedPointer<Array>(new Array(left, right)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReader::parseValue()
//-----------------------------------------------------------------------------
void ParameterReader::parseValue(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter) const
{
    parameter->setValue(parameterNode.firstChildElement("ipxact:value").firstChild().nodeValue());

    QDomNamedNodeMap valueAttributes = parameterNode.firstChildElement("ipxact:value").attributes();
    for (int j = 0; j < valueAttributes.size(); ++j) 
    {
        QString attributeName = valueAttributes.item(j).nodeName();
        QString attributeValue = valueAttributes.item(j).nodeValue();

        parameter->setValueAttribute(attributeName, attributeValue);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReader::parseVendorExtensions()
//-----------------------------------------------------------------------------
void ParameterReader::parseVendorExtensions(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter) const
{
    QDomNode extensionsNode = parameterNode.firstChildElement("ipxact:vendorExtensions");

    if (extensionsNode.hasChildNodes())
    {
        QSharedPointer<QList<QSharedPointer<VendorExtension> > > extensions = parameter->getVendorExtensions();

        int arrayCount = extensionsNode.childNodes().size();
        for (int i = 0; i < arrayCount; i++)
        {
            QDomNode extensionNode = extensionsNode.childNodes().at(i);
            extensions->append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
        }
    }
}
