//-----------------------------------------------------------------------------
// File: ClearboxElementReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 10.06.2026
//
// Description:
// Reader for the ipxact:clearboxElement/whiteboxElement elements.
//-----------------------------------------------------------------------------

#pragma once

#include <IPXACTmodels/Component/ClearboxElement.h>
#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>

namespace ClearboxElementReader
{
    IPXACTMODELS_EXPORT QSharedPointer<ClearboxElement> createClearboxElementFrom(QDomNode const& clearboxElementNode, Document::Revision docRevision);

    namespace Details
    {
        void parseClearboxType(QDomNode const& clearboxElementNode, QSharedPointer<ClearboxElement> newClearboxElement, Document::Revision docRevision);

        void parseDriveable(QDomNode const& clearboxElementNode, QSharedPointer<ClearboxElement> newClearboxElement);
    }
}