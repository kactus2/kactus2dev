//-----------------------------------------------------------------------------
// File: StitchTile.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 26.11.2012
//
// Description:
// Tile class for corner stitching data structure.
//-----------------------------------------------------------------------------

#ifndef STITCHTILE_H
#define STITCHTILE_H

#include <QPoint>

//-----------------------------------------------------------------------------

enum TileType
{
    TILE_EMPTY = 0,     //!< Specified an empty tile.
    TILE_SOLID,         //!< Specifies a used tile.
    TILE_LINE           //!< Special tile for lines.
};

//-----------------------------------------------------------------------------
//! Tile struct for corner stitching data structure.
//-----------------------------------------------------------------------------
struct StitchTile
{
    TileType type;              //!< The tile type.
    QPoint pos;                 //!< The position of the top-left corner.
    StitchTile* topLeft_;       //!< Top-left stitch.
    StitchTile* leftTop_;       //!< Right-top stitch.
    StitchTile* bottomRight_;   //!< Bottom-right stitch.
    StitchTile* rightBottom_;   //!< Right-bottom stitch.
    void* data;                 //!< Custom data pointer

    /*! 
     *  Default constructor.
     */
    StitchTile() : topLeft_(0), leftTop_(0), bottomRight_(0), rightBottom_(0),
                   pos(), data(0), type(TILE_EMPTY)
    {
    }

    /*!
     *  Returns true if the tile contains the given point.
     *
     *      @param [in] pt The point to test.
     */
    bool contains(QPoint const& pt) const
    {
        return (pt.x() >= pos.x() && pt.y() >= pos.y() &&
                (bottomRight_ == 0 || pt.x() < bottomRight_->pos.x()) &&
                (rightBottom_ == 0 || pt.y() < rightBottom_->pos.y()));
    }

    /*!
     *  Returns true if the tile contains the given x coordinate.
     *
     *      @param [in] x The x coordinate.
     */
    bool containsX(int x) const
    {
        return (x >= pos.x() && (bottomRight_ == 0 || x < bottomRight_->pos.x()));
    }

    /*!
     *  Returns true if the tile contains the given y coordinate.
     *
     *      @param [in] y The y coordinate.
     */
    bool containsY(int y) const
    {
        return (y >= pos.y() && (rightBottom_ == 0 || y < rightBottom_->pos.y()));
    }
};

//-----------------------------------------------------------------------------

#endif // STITCHTILE_H
