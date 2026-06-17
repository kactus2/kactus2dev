//-----------------------------------------------------------------------------
// File: ConstraintSetReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 15.06.2026
//
// Description:
// Reader for the ipxact:constraintSet element.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/ConstraintSet.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>

namespace ConstraintSetReader
{

    IPXACTMODELS_EXPORT QSharedPointer<ConstraintSet> createConstraintSetFrom(QDomNode const& constraintSetNode, Document::Revision docRevision);

    namespace Details
    {
        void parseConstraintSetId(QDomNode const& constraintSetNode, QSharedPointer<ConstraintSet> newConstraintSet);

        void parseDriveConstraint(QDomNode const& constraintSetNode, QSharedPointer<ConstraintSet> newConstraintSet);

        void parseLoadConstraint(QDomNode const& constraintSetNode, QSharedPointer<ConstraintSet> newConstraintSet);

        void parseTimingConstraints(QDomNode const& constraintSetNode, QSharedPointer<ConstraintSet> newConstraintSet);
    }
}
