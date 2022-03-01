//-----------------------------------------------------------------------------
// File: SegmentComboBox.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.02.2022
//
// Description:
// The combo box to select one of the defined segments.
//-----------------------------------------------------------------------------

#include "SegmentComboBox.h"

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Segment.h>

//-----------------------------------------------------------------------------
// Function: SegmentComboBox::SegmentComboBox()
//-----------------------------------------------------------------------------
SegmentComboBox::SegmentComboBox(QWidget *parent):
QComboBox(parent)
{
    setEditable(false);
}

//-----------------------------------------------------------------------------
// Function: SegmentComboBox::setupSelection()
//-----------------------------------------------------------------------------
void SegmentComboBox::setupSelection(QSharedPointer<AddressSpace> containingSpace, QString const& segmentReference)
{
    clear();

    addItem("");

    if (containingSpace)
    {
        for (auto segment : *containingSpace->getSegments())
        {
            addItem(segment->name());
        }
    }

    if (findText(segmentReference) < 0)
    {
        addItem(segmentReference);
    }

    setCurrentText(segmentReference);
}
