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

#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/PartSelect.h>
#include <IPXACTmodels/Component/FieldReferenceReader.h>

//-----------------------------------------------------------------------------
// Function: ModeReader::createModeFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Mode> ModeReader::createModeFrom(QDomNode const& modeNode)
{
    QSharedPointer<Mode> newMode (new Mode());

    NameGroupReader::parseNameGroup(modeNode, newMode);

    Details::parseCondition(modeNode, newMode);

    Details::parsePortSlices(modeNode, newMode);

    Details::parseFieldSlices(modeNode, newMode);

    return newMode;
}

//-----------------------------------------------------------------------------
// Function: ModeReader::Details::parseCondition()
//-----------------------------------------------------------------------------
void ModeReader::Details::parseCondition(QDomNode const& modeNode, QSharedPointer<Mode> newMode)
{
    auto conditionElement = modeNode.firstChildElement(QStringLiteral("ipxact:condition"));
    newMode->setCondition(conditionElement.firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: ModeReader::parsePortSlices()
//-----------------------------------------------------------------------------
void ModeReader::Details::parsePortSlices(QDomNode const& modeNode, QSharedPointer<Mode> newMode)
{
    auto sliceNodeList = modeNode.toElement().elementsByTagName(QStringLiteral("ipxact:portSlice"));

    const int SLICE_COUNT = sliceNodeList.count();
    for (int i = 0; i < SLICE_COUNT; ++i)
    {
        auto portSliceElement = sliceNodeList.at(i).toElement();

        QSharedPointer<PortSlice> newSlice(new PortSlice());

        NameGroupReader::parseNameGroup(portSliceElement, newSlice);

        auto portRefElement = portSliceElement.firstChildElement(QStringLiteral("ipxact:portRef"));
        auto portRef = portRefElement.attribute(QStringLiteral("portRef"));
        newSlice->setPortRef(portRef);

        
        if (QDomNode partSelectNode = portSliceElement.firstChildElement(QStringLiteral("ipxact:partSelect")); 
            !partSelectNode.isNull())
        {
            QSharedPointer<PartSelect> newPartSelect = CommonItemsReader::parsePartSelect(partSelectNode);
            newSlice->setPartSelect(newPartSelect);
        }

        newMode->getPortSlices()->append(newSlice);
    }
}

//-----------------------------------------------------------------------------
// Function: ModeReader::parseFieldSlices()
//-----------------------------------------------------------------------------
void ModeReader::Details::parseFieldSlices(QDomNode const& modeNode, QSharedPointer<Mode> newMode)
{
    auto sliceNodeList = modeNode.toElement().elementsByTagName(QStringLiteral("ipxact:fieldSlice"));

    const int SLICE_COUNT = sliceNodeList.count();
    for (int i = 0; i < SLICE_COUNT; ++i)
    {
        auto fieldSliceElement = sliceNodeList.at(i).toElement();

        QSharedPointer<FieldSlice> newSlice(new FieldSlice());

        NameGroupReader::parseNameGroup(fieldSliceElement, newSlice);

        auto ref = FieldReferenceReader::createFieldReferenceFrom(fieldSliceElement);
        newSlice->FieldReference::operator=(*ref);

        Details::parseRange(fieldSliceElement, newSlice);

        newMode->getFieldSlices()->append(newSlice);
    }
}

//-----------------------------------------------------------------------------
// Function: ModeReader::Details::parseRange()
//-----------------------------------------------------------------------------
void ModeReader::Details::parseRange(QDomElement const& element, QSharedPointer<Range> range)
{
    auto rangeElement = element.firstChildElement(QStringLiteral("ipxact:range"));
    if (!rangeElement.isNull())
    {
        auto leftRange = rangeElement.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue();
        auto rightRange = rangeElement.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue();

        range->setLeft(leftRange);
        range->setRight(rightRange);
    }
}
