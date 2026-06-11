#include "ClearboxElementReader.h"

#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/common/ParameterReader.h>
#include <IPXACTmodels/common/NameGroupReader.h>

QSharedPointer<ClearboxElement> ClearboxElementReader::createClearboxElementFrom(QDomNode const& clearBoxElementNode, Document::Revision docRevision)
{
    QSharedPointer<ClearboxElement> newClearBoxElement(new ClearboxElement());

    NameGroupReader::parseNameGroup(clearBoxElementNode, newClearBoxElement);

    if (docRevision == Document::Revision::Std14)
    {
        auto isPresentValue = CommonItemsReader::parseIsPresent(clearBoxElementNode.firstChildElement(QStringLiteral("ipxact:isPresent")));
        newClearBoxElement->setIsPresent(isPresentValue);
    }

    Details::parseClearboxType(clearBoxElementNode, newClearBoxElement, docRevision);

    Details::parseDriveable(clearBoxElementNode, newClearBoxElement);

    newClearBoxElement->setParameters(CommonItemsReader::parseAndCreateParameters(clearBoxElementNode, docRevision));

    CommonItemsReader::parseVendorExtensions(clearBoxElementNode, newClearBoxElement);

    return newClearBoxElement;
}

void ClearboxElementReader::Details::parseClearboxType(QDomNode const& clearBoxElementNode, QSharedPointer<ClearboxElement> newClearBoxElement, Document::Revision docRevision)
{
    if (docRevision == Document::Revision::Std22)
    {
        auto typeNode = clearBoxElementNode.firstChildElement(QStringLiteral("ipxact:clearboxType"));
        newClearBoxElement->setType(ClearboxElement::stringToType(typeNode.firstChild().nodeValue()));
    }
    else
    {
        auto typeNode = clearBoxElementNode.firstChildElement(QStringLiteral("ipxact:whiteboxType"));
        newClearBoxElement->setType(ClearboxElement::stringToType(typeNode.firstChild().nodeValue()));
    }
}

void ClearboxElementReader::Details::parseDriveable(QDomNode const& clearBoxElementNode, QSharedPointer<ClearboxElement> newClearBoxElement)
{
    auto driveableNodeValue = clearBoxElementNode.firstChildElement(QStringLiteral("ipxact:driveable")).firstChild().nodeValue();
    if (driveableNodeValue.compare(QStringLiteral("true")) == 0)
    {
        newClearBoxElement->setDriveable(true);
    }
    else if (driveableNodeValue.compare(QStringLiteral("false")) == 0)
    {
        newClearBoxElement->setDriveable(false);
    }
}
