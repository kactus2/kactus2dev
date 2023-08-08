//-----------------------------------------------------------------------------
// File: ModeReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.08.2023
//
// Description:
// Reader class for IP-XACT mode element.
//-----------------------------------------------------------------------------

#include "ModeReader.h"
#include "Mode.h"
#include "RemapPort.h"

#include <IPXACTmodels/common/NameGroupReader.h>

//-----------------------------------------------------------------------------
// Function: ModeReader::createModeFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Mode> ModeReader::createModeFrom(QDomNode const& ModeNode)
{
    QSharedPointer<Mode> newMode (new Mode());

    NameGroupReader::parseNameGroup(ModeNode, newMode);

    Details::parseRemapPorts(ModeNode, newMode);

    return newMode;
}

//-----------------------------------------------------------------------------
// Function: ModeReader::parseRemapPorts()
//-----------------------------------------------------------------------------
void ModeReader::Details::parseRemapPorts(QDomNode const& ModeNode, QSharedPointer<Mode> newMode)
{
    QDomElement remapPortsElement = ModeNode.firstChildElement(QStringLiteral("ipxact:remapPorts"));
    if (!remapPortsElement.isNull())
    {
        QDomNodeList remapPortNodeList = remapPortsElement.elementsByTagName(QStringLiteral("ipxact:remapPort"));

        for (int remapPortIndex = 0; remapPortIndex < remapPortNodeList.count(); ++remapPortIndex)
        {
            QDomElement remapPortElement = remapPortNodeList.at(remapPortIndex).toElement();

            QString portRef = remapPortElement.attribute(QStringLiteral("portRef"));

        }
    }
}
