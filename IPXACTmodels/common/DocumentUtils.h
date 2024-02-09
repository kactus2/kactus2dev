//-----------------------------------------------------------------------------
// File: DocumentUtils.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 31.01.2024
//
// Description:
// Utilities for document compatibility checking.
//-----------------------------------------------------------------------------

#ifndef DOCUMENTUTILS_H
#define DOCUMENTUTILS_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

class LibraryInterface;
class VLNV;

namespace DocumentUtils
{

    /*!
     *	Checks if two documents of same type are using the same IP-XACT standard revision.
     *  
     *      @param [in] firstVlnv        The VLNV of the first document to check.
     *      @param [in] secondVlnv       The VLNV of the second document to check.
     *      @param [in] library          The library handler.
     *	    
     * 	    @return True, if the revisions are the same, false if not or if either document couldn't be found.
     */
    IPXACTMODELS_EXPORT bool documentsHaveMatchingStdRevisions(VLNV const& firstVlnv, VLNV const& secondVlnv, 
        LibraryInterface* library);

}

#endif // DOCUMENTUTILS_H
