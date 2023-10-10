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
// Function: RemapStateReader::createRemapStateFrom()
//-----------------------------------------------------------------------------
QSharedPointer<RemapState> RemapStateReader::createRemapStateFrom(QDomNode const& remapStateNode)
{
    QSharedPointer<RemapState> newRemapState (new RemapState());

    Details::parseNameGroup(remapStateNode, newRemapState);

    Details::parseRemapPorts(remapStateNode, newRemapState);

    return newRemapState;
}

//-----------------------------------------------------------------------------
// Function: RemapStateReader::Details::parseNameGroup()
//-----------------------------------------------------------------------------
void RemapStateReader::Details::parseNameGroup(QDomNode const& remapStateNode, 
    QSharedPointer<RemapState> newRemapState)
{
    NameGroupReader::parseNameGroup(remapStateNode, newRemapState);
}

//-----------------------------------------------------------------------------
// Function: RemapStateReader::Details::parseRemapPorts()
//-----------------------------------------------------------------------------
void RemapStateReader::Details::parseRemapPorts(QDomNode const& remapStateNode,
    QSharedPointer<RemapState> newRemapState)
{
    QDomElement remapPortsElement = remapStateNode.firstChildElement(QStringLiteral("ipxact:remapPorts"));
    if (!remapPortsElement.isNull())
    {
        QDomNodeList remapPortNodeList = remapPortsElement.elementsByTagName(QStringLiteral("ipxact:remapPort"));

        for (int remapPortIndex = 0; remapPortIndex < remapPortNodeList.count(); ++remapPortIndex)
        {
            QDomElement remapPortElement = remapPortNodeList.at(remapPortIndex).toElement();

            QString portRef = remapPortElement.attribute(QStringLiteral("portRef"));
            QSharedPointer<RemapPort> newRemapPort (new RemapPort(portRef));

            parseRemapPortIndex(remapPortElement, newRemapPort);

            parseRemapPortValue(remapPortElement, newRemapPort);

            newRemapState->getRemapPorts()->append(newRemapPort);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RemapStateReader::Details::parseRemapPortIndex()
//-----------------------------------------------------------------------------
void RemapStateReader::Details::parseRemapPortIndex(QDomElement const& remapPortElement,
    QSharedPointer<RemapPort> newRemapPort)
{
    QDomElement portIndexElement = remapPortElement.firstChildElement(QStringLiteral("ipxact:portIndex"));
    if (!portIndexElement.isNull())
    {
        QString portIndex = portIndexElement.firstChild().nodeValue();
        newRemapPort->setPortIndex(portIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: RemapStateReader::Details::parseRemapPortValue()
//-----------------------------------------------------------------------------
void RemapStateReader::Details::parseRemapPortValue(QDomElement const& remapPortElement,
    QSharedPointer<RemapPort> newRemapPort)
{
    QString value = remapPortElement.firstChildElement(QStringLiteral("ipxact:value")).firstChild().nodeValue();
    newRemapPort->setValue(value);
}