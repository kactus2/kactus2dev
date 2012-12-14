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

    // Split if needed.
    if (topTile->pos.y() != area.top())
    {
        StitchTile* top1 = 0;
        StitchTile* top2 = 0;
        splitV(topTile, area.top(), top1, top2);

        topTile = top2;
    }

    // Find the tile containing the bottom edge of the area.
    StitchTile* bottomTile = findTile(area.bottomLeft());

    if (bottomTile->rightBottom_ == 0 || bottomTile->rightBottom_->pos.y() != area.bottom() + 1)
    {
        StitchTile* bottom1 = 0;
        StitchTile* bottom2 = 0;
        splitV(bottomTile, area.bottom() + 1, bottom1, bottom2);

        bottomTile = bottom1;
    }

    tile->leftTop_ = topTile->leftTop_;
    //tile->rightBottom_ = bottomTile->rightBottom_;

    // Split all tiles in between.
    StitchTile* curTile = topTile;
    bool endFound = false;

    while (curTile != 0 && !endFound)
    {
        if (curTile == bottomTile)
        {
            endFound = true;
        }

        // Check if we need to split the tile.
        if (curTile->bottomRight_ == 0 || curTile->bottomRight_->pos.x() > area.right())
        {
            StitchTile* left = 0;
            StitchTile* right = 0;
            splitH(curTile, area.right() + 1, left, right);

            tile->rightBottom_ = left->rightBottom_;

            // Update the stitches.
            left->bottomRight_ = tile;
            right->topLeft_ = tile;

            if (curTile == topTile)
            {
                tile->topLeft_ = left;
            }
            
            tile->rightBottom_ = left->rightBottom_;
            tile->bottomRight_ = right;

            // Merge if possible.
            mergeV(left);
            mergeV(right);
        }
        else
        {
            // Otherwise we can just update the stitches.
            curTile->bottomRight_->topLeft_ = tile;

            if (curTile == topTile)
            {
                tile->topLeft_ = curTile;
            }
            
            tile->rightBottom_ = curTile->rightBottom_;
            tile->bottomRight_ = curTile->bottomRight_;
        }

        curTile = findTile(QPoint(area.left(), curTile->rightBottom_->pos.y()));
    }

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
void CornerStitchStructure::splitV(StitchTile* tile, int y, StitchTile*& top, StitchTile*& bottom)
{
    Q_ASSERT(y >= tile->pos.y());
    Q_ASSERT(tile->rightBottom_ == 0 || y < tile->rightBottom_->pos.y());

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

//-----------------------------------------------------------------------------
// Function: CornerStitchStructure::splitV()
//-----------------------------------------------------------------------------
void CornerStitchStructure::splitH(StitchTile* tile, int x, StitchTile*& left, StitchTile*& right)
{
    Q_ASSERT(x >= tile->pos.x());
    Q_ASSERT(tile->bottomRight_ == 0 || x < tile->bottomRight_->pos.x());

    left = tile;

    // Create a new tile for the right part.
    right = new StitchTile();
    right->type = tile->type;
    right->pos = QPoint(x, tile->pos.y());

    // Update stitches below the original tile.
    StitchTile* curBelow = tile->rightBottom_;

    while (curBelow != 0 && curBelow->leftTop_ == tile && curBelow->pos.x() >= x)
    {
        curBelow->leftTop_ = right;
        curBelow = curBelow->topLeft_;
    }

    // Update stitches above the original tile.
    StitchTile* curAbove = tile->leftTop_;

    while (curAbove != 0 && curAbove->rightBottom_ == tile)
    {
        if (curAbove->bottomRight_ == 0 || curAbove->bottomRight_->pos.x() > x)
        {
            curAbove->rightBottom_ = right;
        }

        curAbove = curAbove->bottomRight_;
    }

    // Update stitches to the right of the original tile.
    if (left->bottomRight_ != 0 )
    {
        left->bottomRight_->topLeft_ = right;
    }

    // Update the stitches of the left and right parts.
    right->leftTop_ = left->leftTop_;
    right->topLeft_ = left;
    right->bottomRight_ = left->bottomRight_;
    right->rightBottom_ = left->rightBottom_;

    left->bottomRight_ = right;
}

//-----------------------------------------------------------------------------
// Function: CornerStitchStructure::mergeH()
//-----------------------------------------------------------------------------
StitchTile* CornerStitchStructure::mergeH(StitchTile* tile)
{
    // Check if the tile can be merged with the tile on the left.
    if (tile->topLeft_ != 0 && canMergeV(tile, tile->topLeft_))
    {
        StitchTile* temp = tile->topLeft_;

        // Update stitches for all adjacent tiles on top of the top-left tile.
        StitchTile* curAbove = temp->leftTop_;

        while (curAbove != 0 && curAbove->rightBottom_ == temp)
        {
            curAbove->rightBottom_ = tile;
            curAbove = curAbove->bottomRight_;
        }

        // Update stitches for all adjacent tiles below.
        StitchTile* curBelow = temp->rightBottom_;

        while (curBelow != 0 && curBelow->leftTop_ == temp)
        {
            curBelow->leftTop_ = tile;
            curBelow = curBelow->topLeft_;
        }

        // Update stitches for the new merged tile.
        tile->pos = temp->pos;
        tile->leftTop_ = temp->leftTop_;
        tile->topLeft_ = temp->topLeft_;

        delete temp;
        return tile;
    }
    // Check if the tile can be merged with the tile below.
    else if (tile->bottomRight_ != 0 && canMergeH(tile, tile->bottomRight_))
    {
        return mergeH(tile->rightBottom_);
    }

    return tile;
}

//-----------------------------------------------------------------------------
// Function: CornerStitchStructure::mergeV()
//-----------------------------------------------------------------------------
StitchTile* CornerStitchStructure::mergeV(StitchTile* tile)
{
    // Check if the tile can be merged with the tile above.
    if (tile->leftTop_ != 0 && canMergeV(tile, tile->leftTop_))
    {
        StitchTile* temp = tile->leftTop_;
        
        // Update stitches for all adjacent tiles on the right side of the left-top tile.
        StitchTile* curRight = temp->bottomRight_;

        while (curRight != 0 && curRight->topLeft_ == temp)
        {
            curRight->topLeft_ = tile;
            curRight = curRight->leftTop_;
        }

        // Update stitches for all adjacent tiles on the left side.
        StitchTile* curLeft = temp->topLeft_;

        while (curLeft != 0 && curLeft->bottomRight_ == temp)
        {
            curLeft->bottomRight_ = tile;
            curLeft = curLeft->rightBottom_;
        }

        // Update stitches for the new merged tile.
        tile->pos = temp->pos;
        tile->leftTop_ = temp->leftTop_;
        tile->topLeft_ = temp->topLeft_;

        delete temp;
        return tile;
    }
    // Check if the tile can be merged with the tile below.
    else if (tile->rightBottom_ != 0 && canMergeV(tile, tile->rightBottom_))
    {
        return mergeV(tile->rightBottom_);
    }

    return tile;
}

//-----------------------------------------------------------------------------
// Function: CornerStitchStructure::canMergeV()
//-----------------------------------------------------------------------------
bool CornerStitchStructure::canMergeH(StitchTile const* tile1, StitchTile const* tile2)
{
    return (tile1->type == TILE_EMPTY && tile2->type == TILE_EMPTY &&
            tile1->pos.y() == tile2->pos.y() &&
            ((tile1->rightBottom_ == 0 && tile2->rightBottom_ == 0) ||
             (tile1->rightBottom_ != 0 && tile2->rightBottom_ != 0 &&
              tile1->rightBottom_->pos.y() == tile2->rightBottom_->pos.y())));
}

//-----------------------------------------------------------------------------
// Function: CornerStitchStructure::canMergeV()
//-----------------------------------------------------------------------------
bool CornerStitchStructure::canMergeV(StitchTile const* tile1, StitchTile const* tile2)
{
    return (tile1->type == TILE_EMPTY && tile2->type == TILE_EMPTY &&
            tile1->pos.x() == tile2->pos.x() &&
            ((tile1->bottomRight_ == 0 && tile2->bottomRight_ == 0) ||
             (tile1->bottomRight_ != 0 && tile2->bottomRight_ != 0 &&
              tile1->bottomRight_->pos.x() == tile2->bottomRight_->pos.x())));
}
