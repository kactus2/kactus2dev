//-----------------------------------------------------------------------------
// File: MemoryDesignerConstants.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2016
//
// Description:
// Common declarations for memory designer.
//-----------------------------------------------------------------------------

#include <designEditors/common/diagramgrid.h>

#include <QString>

// Common declarations for memory designer.
namespace MemoryDesignerConstants
{
    //! Interval of one bit in a memory address.
    const qreal RANGEINTERVAL = GridSize * 1.5;

    const QString MEMORYMAP_TYPE = "memoryMap";
    const QString ADDRESSBLOCK_TYPE = "addressBlock";
    const QString REGISTER_TYPE = "register";
    const QString ADDRESSSPACE_TYPE = "addressSpace";
    const QString ADDRESSSEGMENT_TYPE = "segment";
}
