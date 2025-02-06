//-----------------------------------------------------------------------------
// File: SVDCPUColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 18.05.2021
//
// Description:
// SVD declarations for editing CPU details.
//-----------------------------------------------------------------------------

#ifndef SVDCPUCOLUMNS_H
#define SVDCPUCOLUMNS_H

namespace SVDCPUColumns
{
    //! Defines the columns for the SVD CPU table.
    enum Column
    {
        CREATESVD = 0,
        NAME,
        REVISION,
        ENDIAN,
        NVICPRIOBITS,
        MPUPRESENT,
        FPUPRESENT,
        VENDORSYSTICKCONFIG,
        COLUMN_COUNT
    };
}

#endif // SVDCPUCOLUMNS_H
