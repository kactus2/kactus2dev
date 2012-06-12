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
#include <QSharedPointer>

class VLNV;
class Component;
class Design;
class DesignConfiguration;
class LibraryInterface;

//-----------------------------------------------------------------------------

/*!
 *  Creates a flattened system design based on the given component. This function
 *  searches for programmable elements, creates corresponding SW components and
 *  generates a system design.
 *
 *      @param [in]  lh         The library interface.
 *      @param [in]  component  The component.
 *      @param [out] sysDesign  The resulted system design.
 */
void generateSystemDesignV2(LibraryInterface* lh, VLNV const& designVLNV, Design& sysDesign);

/*!
 *  Updates the system design based on the given component. This function reflects
 *  the changes in the HW component and its software mappings to the given system design.
 *
 *      @param [in]    lh         The library interface.
 *      @param [in,out] sysDesign  The system design to update.
 */
void updateSystemDesignV2(LibraryInterface* lh, VLNV const& designVLNV, Design& sysDesign,
                          QSharedPointer<DesignConfiguration> designConf);

//-----------------------------------------------------------------------------

#endif // LIBRARYUTILS_H
