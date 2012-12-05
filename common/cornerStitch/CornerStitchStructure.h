//-----------------------------------------------------------------------------
// File: CornerStitchStructure.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.11.2012
//
// Description:
// Corner stitch data structure.
//-----------------------------------------------------------------------------

#ifndef CORNERSTITCHSTRUCTURE_H
#define CORNERSTITCHSTRUCTURE_H

#include "StitchTile.h"

#include <QRect>

//-----------------------------------------------------------------------------
//! Corner stitch data structure.
//-----------------------------------------------------------------------------
class CornerStitchStructure
{
public:
    /*!
     *  Constructor.
     */
    CornerStitchStructure();

    /*!
     *  Destructor.
     */
    ~CornerStitchStructure();

    /*!
     *  Adds a new solid tile.
     *
     *      @param [in] area The tile area.
     */
    StitchTile* addSolidTile(QRect const& area);

    /*!
     *  Removes the given tile.
     *
     *      @param [in] tile The tile to remove.
     */
    void removeTile(StitchTile* tile);

    /*!
     *  Retrieves the tile that contains the given point.
     */
    StitchTile* findTile(QPoint const& pt);

private:
    // Disable copying.
    CornerStitchStructure(CornerStitchStructure const& rhs);
    CornerStitchStructure& operator=(CornerStitchStructure const& rhs);

    /*!
     *  Splits the given tile horizontally into two parts.
     */
    void splitH(StitchTile* tile, int y, StitchTile* top, StitchTile* bottom);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The root tile.
    StitchTile* root_;
};


#endif // CORNERSTITCHSTRUCTURE_H
