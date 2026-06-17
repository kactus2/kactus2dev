//-----------------------------------------------------------------------------
// File: ConstraintSetWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 15.06.2026
//
// Description:
// Writer for the ipxact:constraintSet element.
//-----------------------------------------------------------------------------

#include "ConstraintSetWriter.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/NameGroupWriter.h>


void ConstraintSetWriter::writeConstraintSet(QXmlStreamWriter& writer, QSharedPointer<ConstraintSet> constraintSet, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:constraintSet"));

    CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("constraintSetId"), constraintSet->getConstraintSetId());

    NameGroupWriter::writeNameGroup(writer, constraintSet, docRevision);

    CommonItemsWriter::writeVector(writer, constraintSet->getVector(), docRevision);

    Details::writeDriveConstraint(writer, constraintSet);

    Details::writeLoadConstraint(writer, constraintSet);

    Details::writeTimingConstraints(writer, constraintSet);

    writer.writeEndElement(); // ipxact:constraintSet
}

void ConstraintSetWriter::Details::writeDriveConstraint(QXmlStreamWriter& writer, QSharedPointer<ConstraintSet> constraintSet)
{
    if (auto driveConstraint = constraintSet->getDriveConstraint(); driveConstraint != nullptr)
    {
        writer.writeStartElement(QStringLiteral("ipxact:driveConstraint"));

        CommonItemsWriter::writeCellSpecification(writer, constraintSet->getDriveConstraint());

        writer.writeEndElement(); // ipxact:driveConstraint
    }
}

void ConstraintSetWriter::Details::writeLoadConstraint(QXmlStreamWriter& writer, QSharedPointer<ConstraintSet> constraintSet)
{
    if (auto loadConstraint = constraintSet->getLoadConstraint(); loadConstraint != nullptr)
    {
        writer.writeStartElement(QStringLiteral("ipxact:loadConstraint"));

        CommonItemsWriter::writeCellSpecification(writer, constraintSet->getLoadConstraint());
        CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:count"), constraintSet->getLoadConstraintCount());

        writer.writeEndElement(); // ipxact:loadConstraint
    }
}

void ConstraintSetWriter::Details::writeTimingConstraints(QXmlStreamWriter& writer, QSharedPointer<ConstraintSet> constraintSet)
{
    for (auto const& timingConstraint : *constraintSet->getTimingConstraints())
    {
        writer.writeStartElement(QStringLiteral("ipxact:timingConstraint"));

        CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("clockEdge"), timingConstraint->clockEdge_);
        CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("delayType"), timingConstraint->delayType_);
        CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("clockName"), timingConstraint->clockName_);

        writer.writeEndElement(); // ipxact:timingConstraint
    }
}
