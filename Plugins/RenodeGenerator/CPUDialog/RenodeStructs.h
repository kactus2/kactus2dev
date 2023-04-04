//-----------------------------------------------------------------------------
// File: RenodeStructs.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.03.2023
//
// Description:
// Utility structs for Renode generator.
//-----------------------------------------------------------------------------

#ifndef RENODESTRUCTS_H
#define RENODESTRUCTS_H

#include <QString>

//-----------------------------------------------------------------------------
//! Columns for Renode peripherals.
//-----------------------------------------------------------------------------
namespace PeripheralColumns
{
    //! The editable columns in the peripheral editor.
    enum columns
    {
        NAME = 0,
        BASEADDRESS,
        SIZE,
        CLASS,
        INITABLE,
        FILEPATH,
        COLUMN_COUNT
    };
}

//-----------------------------------------------------------------------------
//! Columns for Renode memories.
//-----------------------------------------------------------------------------
namespace MemoryColumns
{
    //! The editable columns in the memory editor.
    enum columns
    {
        NAME = 0,
        CLASS,
        BASEADDRESS,
        SIZE,
        COLUMN_COUNT
    };

};

//-----------------------------------------------------------------------------
//! Utility structs for Renode generator.
//-----------------------------------------------------------------------------
namespace RenodeStructs
{
    //! Container for renode peripheral data connected to a CPU.
    struct cpuPeripherals
    {
        //! Name.
        QString peripheralName_ = "";

        //! ID.
        QString peripheralID_ = "";

        //! Type name.
        QString className_ = "Python.PythonPeripheral";

        //! Base address.
        QString baseAddress_ = "";

        //! Size.
        QString size_ = "";

        //! Flag for if the peripheral can be initialized and executes code from the isInit section.
        bool initable_ = true;

        //! Relative path to the python file for this peripheral.
        QString filePath_ = "python_peripherals";
    };

    //! Container for renode memory data connected to a CPU.
    struct cpuMemories
    {
        //! Name.
        QString memoryName_ = "";

        //! ID.
        QString memoryID_ = "";

        //! Class name.
        QString className_ = "Memory.MappedMemory";

        //! Base address.
        QString baseAddress_ = "";

        //! Size.
        QString size_ = "";
    };
};

#endif // RENODESTRUCTS_H