//-----------------------------------------------------------------------------
// File: ClearboxElementWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 10.06.2026
//
// Description:
// Writer for the ipxact:clearboxElement/whiteboxElement elements.
//-----------------------------------------------------------------------------

#pragma once

#include <IPXACTmodels/Component/ClearboxElement.h>
#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>

namespace ClearboxElementWriter
{
    IPXACTMODELS_EXPORT void writeClearboxElement(QXmlStreamWriter& writer, QSharedPointer<ClearboxElement> clearBoxElement, Document::Revision docRevision);
}