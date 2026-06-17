//-----------------------------------------------------------------------------
// File: ConstraintSetReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 15.06.2026
//
// Description:
// Reader for the ipxact:constraintSet element.
//-----------------------------------------------------------------------------

#include "ConstraintSetReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/CommonItemsReader.h>

QSharedPointer<ConstraintSet> ConstraintSetReader::createConstraintSetFrom(QDomNode const& constraintSetNode, Document::Revision docRevision)
{
    QSharedPointer<ConstraintSet> newConstraintSet(new ConstraintSet());

    Details::parseConstraintSetId(constraintSetNode, newConstraintSet);

    NameGroupReader::parseNameGroup(constraintSetNode, newConstraintSet);

    if (auto vectorNode = constraintSetNode.firstChildElement(QStringLiteral("ipxact:vector"));
        vectorNode.isNull() == false)
    {
        newConstraintSet->setVector(CommonItemsReader::parseVector(vectorNode, docRevision));
    }

    Details::parseDriveConstraint(constraintSetNode, newConstraintSet);

    Details::parseLoadConstraint(constraintSetNode, newConstraintSet);

    Details::parseTimingConstraints(constraintSetNode, newConstraintSet);

    return newConstraintSet;
}

void ConstraintSetReader::Details::parseConstraintSetId(QDomNode const& constraintSetNode, QSharedPointer<ConstraintSet> newConstraintSet)
{
    newConstraintSet->setConstraintSetId(constraintSetNode.toElement().attribute(QStringLiteral("constraintSetId")));
}

void ConstraintSetReader::Details::parseDriveConstraint(QDomNode const& constraintSetNode, QSharedPointer<ConstraintSet> newConstraintSet)
{
    auto driveConstraintNode = constraintSetNode.firstChildElement(QStringLiteral("ipxact:driveConstraint"));
    auto cellSpecificationNode = driveConstraintNode.firstChildElement(QStringLiteral("ipxact:cellSpecification"));
    if (cellSpecificationNode.isNull() == false)
    {
        newConstraintSet->setDriveConstraint(CommonItemsReader::parseCellSpecification(cellSpecificationNode));
    }
}

void ConstraintSetReader::Details::parseLoadConstraint(QDomNode const& constraintSetNode, QSharedPointer<ConstraintSet> newConstraintSet)
{
    auto loadConstraintNode = constraintSetNode.firstChildElement(QStringLiteral("ipxact:loadConstraint"));
    auto cellSpecificationNode = loadConstraintNode.firstChildElement(QStringLiteral("ipxact:cellSpecification"));
    if (cellSpecificationNode.isNull() == false)
    {
        newConstraintSet->setLoadConstraint(CommonItemsReader::parseCellSpecification(cellSpecificationNode));

        auto countNode = loadConstraintNode.firstChildElement(QStringLiteral("ipxact:count"));
        if (countNode.isNull() == false)
        {
            newConstraintSet->setLoadConstraintCount(countNode.firstChild().nodeValue());
        }
    }
}

void ConstraintSetReader::Details::parseTimingConstraints(QDomNode const& constraintSetNode, QSharedPointer<ConstraintSet> newConstraintSet)
{
    auto timingConstraintNodes = constraintSetNode.toElement().elementsByTagName(QStringLiteral("ipxact:timingConstraint"));

    for (int i = 0; i < timingConstraintNodes.size(); ++i)
    {
        auto newTimingConstraint = QSharedPointer<ConstraintSet::TimingConstraint>(new ConstraintSet::TimingConstraint());
        auto currentNode = timingConstraintNodes.at(i).toElement();

        newTimingConstraint->clockEdge_ = currentNode.attribute(QStringLiteral("clockEdge"));
        newTimingConstraint->delayType_ = currentNode.attribute(QStringLiteral("delayType"));
        newTimingConstraint->clockName_ = currentNode.attribute(QStringLiteral("clockName"));

        newConstraintSet->getTimingConstraints()->append(newTimingConstraint);
    }
}
