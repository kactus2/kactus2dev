//-----------------------------------------------------------------------------
// File: LibraryUtils.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.6.2011
//
// Description:
// Library utility functions.
//-----------------------------------------------------------------------------

#ifndef LIBRARYUTILS_H
#define LIBRARYUTILS_H

#include <QString>

class Component;
class Design;
class LibraryInterface;

//-----------------------------------------------------------------------------

/*!
 *  Creates a flattened system design based on the given component. This function
 *  searches for programmable elements, creates corresponding SW components and
 *  generates a system design.
 *
 *      @param [in]  lh         The library interface.
 *      @param [in]  directory  The directory for the files.
 *      @param [in]  component  The component.
 *      @param [out] sysDesign  The resulted system design.
 */
void generateSystemDesign(LibraryInterface* lh, QString const& directory,
                          Component& component, Design& sysDesign);

//-----------------------------------------------------------------------------

#endif // LIBRARYUTILS_H
