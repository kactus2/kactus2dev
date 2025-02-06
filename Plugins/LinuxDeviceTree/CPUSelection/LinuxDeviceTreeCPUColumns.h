//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeCPUClumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.10.2021
//
// Description:
// Linux device tree declarations for editing CPU details.
//-----------------------------------------------------------------------------

#ifndef LINUXDEVICETREECPUCOLUMNS_H
#define LINUXDEVICETREECPUCOLUMNS_H

namespace LinuxDeviceTreeCPUColumns
{
    //! Defines the columns for the Linux device tree CPU table.
    enum Column
    {
        CREATEDEVICETREE = 0,
        FILE_NAME,
        CPUS,
        CONTAINING_COMPONENT,
        COLUMN_COUNT
    };
}

#endif // LINUXDEVICETREECPUCOLUMNS_H
