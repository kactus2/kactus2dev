//-----------------------------------------------------------------------------
// File: CornerStitchStructure.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Määttä
// Date: 27.11.2012
//
// Description:
// Corner stitching data structure.
//-----------------------------------------------------------------------------

#include "CornerStitchStructure.h"

//-----------------------------------------------------------------------------
// Function: CornerStitchStructure::CornerStitchStructure()
//-----------------------------------------------------------------------------
CornerStitchStructure::CornerStitchStructure()
    : root_(0)
{
    // Create an empty tile occupying the whole max area.
    StitchTile* tile = new StitchTile();
    tile->type = TILE_EMPTY;
    tile->pos = QPoint(0, 0);

    root_ = tile;
}

//-----------------------------------------------------------------------------
// Function: CornerStitchStructure::~CornerStitchStructure()
//-----------------------------------------------------------------------------
CornerStitchStructure::~CornerStitchStructure()
{

}

//-----------------------------------------------------------------------------
// Function: CornerStitchStructure::addSolidTile()
//-----------------------------------------------------------------------------
StitchTile* CornerStitchStructure::addSolidTile(QRect const& area)
{
    StitchTile* tile = new StitchTile();
    tile->type = TILE_SOLID;
    tile->pos = area.topLeft();

    // Find the tile containing the top edge of the area.
    StitchTile* topTile = findTile(area.topLeft());

    StitchTile* top1 = 0;
    StitchTile* top2 = 0;
    splitH(topTile, area.top(), top1, top2);

    // Find the tile containing the bottom edge of the area.
    StitchTile* bottomTile = findTile(area.bottomLeft());

    StitchTile* bottom1 = 0;
    StitchTile* bottom2 = 0;
    splitH(bottomTile, area.bottom(), bottom1, bottom2);

    return tile;
}

//-----------------------------------------------------------------------------
// Function: CornerStitchStructure::removeTile()
//-----------------------------------------------------------------------------
void CornerStitchStructure::removeTile(StitchTile* tile)
{
    // TODO:
}

//-----------------------------------------------------------------------------
// Function: CornerStitchStructure::findTile()
//-----------------------------------------------------------------------------
StitchTile* CornerStitchStructure::findTile(QPoint const& pt)
{
    StitchTile* curTile = root_;

    while (curTile != 0 && !curTile->contains(pt))
    {
        while (curTile != 0 && !curTile->containsY(pt.y()))
        {
            if (pt.y() > curTile->pos.y())
            {
                curTile = curTile->rightBottom_;
            }
            else
            {
                curTile = curTile->leftTop_;
            }
        }

        while (curTile != 0 && !curTile->containsX(pt.x()))
        {
            if (pt.x() > curTile->pos.x())
            {
                curTile = curTile->bottomRight_;
            }
            else
            {
                curTile = curTile->topLeft_;
            }
        }
    }

    return curTile;
}

//-----------------------------------------------------------------------------
// Function: CornerStitchStructure::splitH()
//-----------------------------------------------------------------------------
void CornerStitchStructure::splitH(StitchTile* tile, int y, StitchTile* top, StitchTile* bottom)
{
    Q_ASSERT(y >= tile->pos.y());
    Q_ASSERT(y < tile->rightBottom_->pos.y());

    top = tile;

    // Create a new tile for the bottom part.
    bottom = new StitchTile();
    bottom->type = tile->type;
    bottom->pos = QPoint(tile->pos.x(), y);

    // Update all left-top stitches below the original tile.
    StitchTile* curBelow = tile->rightBottom_;

    while (curBelow != 0 && curBelow->leftTop_ == tile)
    {
        curBelow->leftTop_ = bottom;
        curBelow = curBelow->topLeft_;
    }

    // Update the stitches of the top and bottom parts.
    bottom->leftTop_ = top;
    bottom->topLeft_ = top->topLeft_;
    bottom->bottomRight_ = top->bottomRight_;
    bottom->rightBottom_ = top->rightBottom_;

    top->rightBottom_ = bottom;
}
