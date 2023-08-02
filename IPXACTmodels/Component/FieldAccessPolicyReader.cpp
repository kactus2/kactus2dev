//-----------------------------------------------------------------------------
// File: FieldAccessPolicyReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 31.7.2023
//
// Description:
// Reader for ipxact:fieldAccessPolicy.
//-----------------------------------------------------------------------------

#include "FieldAccessPolicyReader.h"
#include "FieldReferenceReader.h"

#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyReader::createFieldAccessPolicyFrom()
//-----------------------------------------------------------------------------
QSharedPointer<FieldAccessPolicy> FieldAccessPolicyReader::createFieldAccessPolicyFrom(
    QDomNode const& fieldAccessPolicyNode)
{
    QSharedPointer<FieldAccessPolicy> fieldAccessPolicy(new FieldAccessPolicy());

    Details::parseModeRefs(fieldAccessPolicyNode, fieldAccessPolicy->getModeReferences());

    Details::parseDefinitionRef(fieldAccessPolicyNode, fieldAccessPolicy);

    Details::parseAccess(fieldAccessPolicyNode, fieldAccessPolicy);

    Details::parseModifiedWriteValue(fieldAccessPolicyNode, fieldAccessPolicy);

    Details::parseWriteValueConstraint(fieldAccessPolicyNode, fieldAccessPolicy);

    Details::parseReadAction(fieldAccessPolicyNode, fieldAccessPolicy);

    Details::parseReadResponse(fieldAccessPolicyNode, fieldAccessPolicy);

    Details::parseBroadcasts(fieldAccessPolicyNode, fieldAccessPolicy);

    Details::parseAccessRestrictions(fieldAccessPolicyNode, fieldAccessPolicy);

    Details::parseTestable(fieldAccessPolicyNode, fieldAccessPolicy);

    Details::parseReserved(fieldAccessPolicyNode, fieldAccessPolicy);

    CommonItemsReader::parseVendorExtensions(fieldAccessPolicyNode, fieldAccessPolicy);

    return fieldAccessPolicy;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyReader::Details::parseModeRefs()
//-----------------------------------------------------------------------------
void FieldAccessPolicyReader::Details::parseModeRefs(QDomNode const& rootNode, 
    QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefList)
{
    auto modeRefNodes = rootNode.childNodes();

    for (int i = 0; i < modeRefNodes.size(); ++i)
    {
        if (auto const& modeRefNode = modeRefNodes.at(i);
            modeRefNode.nodeName() == QStringLiteral("ipxact:modeRef"))
        {
            QSharedPointer<ModeReference> newModeRef(new ModeReference());

            newModeRef->setReference(modeRefNode.firstChild().nodeValue());
            
            if (auto const& priority = modeRefNode.attributes().namedItem(QStringLiteral("priority")).nodeValue();
                !priority.isEmpty())
            {
                newModeRef->setPriority(priority);
            }

            modeRefList->append(newModeRef);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyReader::Details::parseDefinitionRef()
//-----------------------------------------------------------------------------
void FieldAccessPolicyReader::Details::parseDefinitionRef(QDomNode const& accessPolicyNode,
    QSharedPointer<FieldAccessPolicy> accessPolicy)
{
    auto definitionNode = accessPolicyNode.firstChildElement(
        QStringLiteral("ipxact:fieldAccessPolicyDefinitionRef"));

    accessPolicy->setFieldAccessPolicyDefinitionRef(definitionNode.firstChild().nodeValue());

    if (definitionNode.hasAttribute(QStringLiteral("typeDefinitions")))
    {
        accessPolicy->setFieldAccessPolicyTypeDefinitionRef(
            definitionNode.attribute(QStringLiteral("typeDefinitions")));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyReader::Details::parseAccess()
//-----------------------------------------------------------------------------
void FieldAccessPolicyReader::Details::parseAccess(QDomNode const& accessPolicyNode,
    QSharedPointer<FieldAccessPolicy> accessPolicy)
{
    auto accessElement = accessPolicyNode.firstChildElement(QStringLiteral("ipxact:access"));
    if (!accessElement.isNull())
    {
        auto accessString = accessElement.firstChild().nodeValue();
        AccessTypes::Access access = AccessTypes::str2Access(accessString, AccessTypes::ACCESS_COUNT);
        accessPolicy->setAccess(access);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyReader::Details::parseModifiedWriteValue()
//-----------------------------------------------------------------------------
void FieldAccessPolicyReader::Details::parseModifiedWriteValue(QDomNode const& accessPolicyNode,
    QSharedPointer<FieldAccessPolicy> accessPolicy)
{
    auto modifiedWriteElement = accessPolicyNode.firstChildElement(QStringLiteral("ipxact:modifiedWriteValue"));
    if (!modifiedWriteElement.isNull())
    {
        QString modifiedWriteValueString = modifiedWriteElement.firstChild().nodeValue();
        General::ModifiedWrite modifiedWriteValue = General::str2ModifiedWrite(modifiedWriteValueString);
        accessPolicy->setModifiedWrite(modifiedWriteValue);

        if (modifiedWriteElement.hasAttribute(QStringLiteral("modify")))
        {
            accessPolicy->setModifiedWriteModify(modifiedWriteElement.attribute(QStringLiteral("modify")));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyReader::Details::parseWriteValueConstraint()
//-----------------------------------------------------------------------------
void FieldAccessPolicyReader::Details::parseWriteValueConstraint(QDomNode const& accessPolicyNode, 
    QSharedPointer<FieldAccessPolicy> accessPolicy)
{
    QDomElement writeConstraintElement = accessPolicyNode.firstChildElement(
        QStringLiteral("ipxact:writeValueConstraint"));
    
    if (!writeConstraintElement.isNull())
    {
        QSharedPointer<WriteValueConstraint> writeConstraint(new WriteValueConstraint());

        if (auto writeAsReadElement = writeConstraintElement.firstChildElement(
            QStringLiteral("ipxact:writeAsRead")); !writeAsReadElement.isNull())
        {
            writeConstraint->setType(WriteValueConstraint::WRITE_AS_READ);
        }
        if (auto useEnumerationElement = writeConstraintElement.firstChildElement(
            QStringLiteral("ipxact:useEnumeratedValues")); !useEnumerationElement.isNull())
        {
            writeConstraint->setType(WriteValueConstraint::USE_ENUM);
        }
        if (auto minimumElement = writeConstraintElement.firstChildElement(QStringLiteral("ipxact:minimum"));
            !minimumElement.isNull())
        {
            writeConstraint->setMinimum(minimumElement.firstChild().nodeValue());
        }
        if (auto maximumElement = writeConstraintElement.firstChildElement(QStringLiteral("ipxact:maximum"));
            !maximumElement.isNull())
        {
            writeConstraint->setMaximum(maximumElement.firstChild().nodeValue());
        }

        accessPolicy->setWriteValueConstraint(writeConstraint);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyReader::Details::parseReadAction()
//-----------------------------------------------------------------------------
void FieldAccessPolicyReader::Details::parseReadAction(QDomNode const& accessPolicyNode,
    QSharedPointer<FieldAccessPolicy> accessPolicy)
{
    auto readActionElement = accessPolicyNode.firstChildElement(QStringLiteral("ipxact:readAction"));
    if (!readActionElement.isNull())
    {
        QString readActionString = readActionElement.firstChild().nodeValue();
        General::ReadAction readAction = General::str2ReadAction(readActionString);

        accessPolicy->setReadAction(readAction);

        if (readActionElement.hasAttribute(QStringLiteral("modify")))
        {
            accessPolicy->setReadActionModify(readActionElement.attribute(QStringLiteral("modify")));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyReader::Details::parseReadResponse()
//-----------------------------------------------------------------------------
void FieldAccessPolicyReader::Details::parseReadResponse(QDomNode const& accessPolicyNode, 
    QSharedPointer<FieldAccessPolicy> accessPolicy)
{
    auto readResponseElement = accessPolicyNode.firstChildElement(QStringLiteral("ipxact:readResponse"));

    if (!readResponseElement.isNull())
    {
        accessPolicy->setReadResponse(readResponseElement.firstChild().nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyReader::Details::parseBroadcasts()
//-----------------------------------------------------------------------------
void FieldAccessPolicyReader::Details::parseBroadcasts(QDomNode const& accessPolicyNode,
    QSharedPointer<FieldAccessPolicy> accessPolicy)
{
    auto broadcastsElement = accessPolicyNode.firstChildElement(QStringLiteral("ipxact:broadcasts"));

    if (broadcastsElement.isNull())
    {
        return;
    }
    
    auto broadcastElements = broadcastsElement.childNodes();

    for (int i = 0; i < broadcastElements.size(); ++i)
    {
        auto singleBroadcast = broadcastElements.at(i);

        if (singleBroadcast.nodeName() == QStringLiteral("ipxact:broadcastTo"))
        {
            auto fieldRef = FieldReferenceReader::createFieldReferenceFrom(singleBroadcast);

            accessPolicy->getBroadcasts()->append(fieldRef);
        }
    }
    
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyReader::Details::parseAccessRestrictions()
//-----------------------------------------------------------------------------
void FieldAccessPolicyReader::Details::parseAccessRestrictions(QDomNode const& accessPolicyNode,
    QSharedPointer<FieldAccessPolicy> accessPolicy)
{
    auto accessRestrictionsElement = accessPolicyNode.firstChildElement(
        QStringLiteral("ipxact:accessRestrictions"));

    if (accessRestrictionsElement.isNull())
    {
        return;
    }

    auto accessRestrictions = accessRestrictionsElement.childNodes();

    for (int i = 0; i < accessRestrictions.size(); ++i)
    {
        QSharedPointer<FieldAccessPolicy::AccessRestriction> accessRestriction(
            new FieldAccessPolicy::AccessRestriction());

        auto accessRestrictionNode = accessRestrictions.at(i);

        if (accessRestrictionNode.nodeName() == QStringLiteral("ipxact:accessRestriction"))
        {
            parseModeRefs(accessRestrictionNode, accessRestriction->modeRefs_);
        }

        accessRestriction->readAccessMask_ = accessRestrictionNode.firstChildElement(
            QStringLiteral("ipxact:readAccessMask")).firstChild().nodeValue();

        accessRestriction->writeAccessMask_ = accessRestrictionNode.firstChildElement(
            QStringLiteral("ipxact:writeAccessMask")).firstChild().nodeValue();

        accessPolicy->getAccessRestrictions()->append(accessRestriction);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyReader::Details::parseTestable()
//-----------------------------------------------------------------------------
void FieldAccessPolicyReader::Details::parseTestable(QDomNode const& accessPolicyNode,
    QSharedPointer<FieldAccessPolicy> accessPolicy)
{
    auto testableElement = accessPolicyNode.firstChildElement(QStringLiteral("ipxact:testable"));
    if (!testableElement.isNull())
    {
        accessPolicy->setTestable(General::str2Bool(testableElement.firstChild().nodeValue(), false));

        if (testableElement.hasAttribute(QStringLiteral("testConstraint")))
        {
            QString testConstraintString = testableElement.attribute(QStringLiteral("testConstraint"));
            General::TestConstraint testConstraint = General::str2TestConstraint(testConstraintString);
            accessPolicy->setTestConstraint(testConstraint);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyReader::Details::parseReserved()
//-----------------------------------------------------------------------------
void FieldAccessPolicyReader::Details::parseReserved(QDomNode const& accessPolicyNode, QSharedPointer<FieldAccessPolicy> accessPolicy)
{
    auto reservedElement = accessPolicyNode.firstChildElement(QStringLiteral("ipxact:reserved"));
    if (!reservedElement.isNull())
    {
        accessPolicy->setReserved(reservedElement.firstChild().nodeValue());
    }
}
