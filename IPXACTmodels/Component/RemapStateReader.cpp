//-----------------------------------------------------------------------------
// File: RemapStateReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Reader class for IP-XACT remap state element.
//-----------------------------------------------------------------------------

#include "RemapStateReader.h"
#include "RemapState.h"
#include "RemapPort.h"

#include <IPXACTmodels/common/NameGroupReader.h>

//-----------------------------------------------------------------------------
// Function: RemapStateReader::RemapStateReader()
//-----------------------------------------------------------------------------
RemapStateReader::RemapStateReader(QObject* parent /* = 0 */) :
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: RemapStateReader::~RemapStateReader()
//-----------------------------------------------------------------------------
RemapStateReader::~RemapStateReader()
{

}

//-----------------------------------------------------------------------------
// Function: RemapStateReader::createRemapStateFrom()
//-----------------------------------------------------------------------------
QSharedPointer<RemapState> RemapStateReader::createRemapStateFrom(QDomNode const& remapStateNode) const
{
    QSharedPointer<RemapState> newRemapState (new RemapState());

    parseNameGroup(remapStateNode, newRemapState);

    parseRemapPorts(remapStateNode, newRemapState);

    return newRemapState;
}

//-----------------------------------------------------------------------------
// Function: RemapStateReader::parseNameGroup()
//-----------------------------------------------------------------------------
void RemapStateReader::parseNameGroup(QDomNode const& remapStateNode, QSharedPointer<RemapState> newRemapState)
    const
{
    NameGroupReader nameGroupReader;
    nameGroupReader.parseNameGroup(remapStateNode, newRemapState);
}

//-----------------------------------------------------------------------------
// Function: RemapStateReader::parseRemapPorts()
//-----------------------------------------------------------------------------
void RemapStateReader::parseRemapPorts(QDomNode const& remapStateNode, QSharedPointer<RemapState> newRemapState)
    const
{
    QDomElement remapPortsElement = remapStateNode.firstChildElement("ipxact:remapPorts");
    if (!remapPortsElement.isNull())
    {
        QDomNodeList remapPortNodeList = remapPortsElement.elementsByTagName("ipxact:remapPort");

        for (int remapPortIndex = 0; remapPortIndex < remapPortNodeList.count(); ++remapPortIndex)
        {
            QDomElement remapPortElement = remapPortNodeList.at(remapPortIndex).toElement();

            QString portRef = remapPortElement.attribute("portRef");
            QSharedPointer<RemapPort> newRemapPort (new RemapPort(portRef));

            parseRemapPortIndex(remapPortElement, newRemapPort);

            parseRemapPortValue(remapPortElement, newRemapPort);

            newRemapState->getRemapPorts()->append(newRemapPort);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RemapStateReader::parseRemapPortIndex()
//-----------------------------------------------------------------------------
void RemapStateReader::parseRemapPortIndex(QDomElement const& remapPortElement,
    QSharedPointer<RemapPort> newRemapPort) const
{
    QDomElement portIndexElement = remapPortElement.firstChildElement("ipxact:portIndex");
    if (!portIndexElement.isNull())
    {
        QString portIndex = portIndexElement.firstChild().nodeValue();
        newRemapPort->setPortIndex(portIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: RemapStateReader::parseRemapPortValue()
//-----------------------------------------------------------------------------
void RemapStateReader::parseRemapPortValue(QDomElement const& remapPortElement,
    QSharedPointer<RemapPort> newRemapPort) const
{
    QString value = remapPortElement.firstChildElement("ipxact:value").firstChild().nodeValue();
    newRemapPort->setValue(value);
}