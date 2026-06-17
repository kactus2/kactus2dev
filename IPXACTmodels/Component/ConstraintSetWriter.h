//-----------------------------------------------------------------------------
// File: ConstraintSetWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 15.06.2026
//
// Description:
// Writer for the ipxact:constraintSet element.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/ConstraintSet.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>

namespace ConstraintSetWriter
{

    IPXACTMODELS_EXPORT void writeConstraintSet(QXmlStreamWriter& writer, QSharedPointer<ConstraintSet> constraintSet, Document::Revision docRevision);

    namespace Details
    {
        void writeDriveConstraint(QXmlStreamWriter& writer, QSharedPointer<ConstraintSet> constraintSet);
        void writeLoadConstraint(QXmlStreamWriter& writer, QSharedPointer<ConstraintSet> constraintSet);
        void writeTimingConstraints(QXmlStreamWriter& writer, QSharedPointer<ConstraintSet> constraintSet);
    }
}
