//-----------------------------------------------------------------------------
// File: DocumentUtils.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 31.01.2024
//
// Description:
// Utilities for document compatibility checking.
//-----------------------------------------------------------------------------

#include "DocumentUtils.h"

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/common/Document.h>

#include <KactusAPI/include/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: DocumentUtils::documentsHaveMatchingStdRevisions()
//-----------------------------------------------------------------------------
bool DocumentUtils::documentsHaveMatchingStdRevisions(VLNV const& firstVlnv, VLNV const& secondVlnv,
    LibraryInterface* library)
{
    if (!firstVlnv.isValid() || !secondVlnv.isValid())
    {
        return false;
    }

    auto firstDoc = library->getModelReadOnly(firstVlnv);
    auto secondDoc = library->getModelReadOnly(secondVlnv);

    if (!firstDoc || !secondDoc)
    {
        return false;
    }

    return firstDoc->getRevision() == secondDoc->getRevision();
}
