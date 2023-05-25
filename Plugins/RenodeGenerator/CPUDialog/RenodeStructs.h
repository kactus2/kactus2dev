//-----------------------------------------------------------------------------
// File: RenodeStructs.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.05.2023
//
// Description:
// Utility structs for Renode generator.
//-----------------------------------------------------------------------------

#ifndef RENODESTRUCTS_H
#define RENODESTRUCTS_H

#include <Plugins/RenodeGenerator/CPUDialog/RenodeConstants.h>

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
        QString className_ = RenodeConstants::PYTHONPERIPHERAL;

        //! Base address.
        QString baseAddress_ = "";

        //! Size.
        QString size_ = "";

        //! Flag for if the peripheral can be initialized and executes code from the isInit section.
        bool initable_ = true;

        //! Relative path to the python file for this peripheral.
        QString filePath_ = RenodeConstants::PYTHONPERIPHERALFILEPATH;
    };

    //! Container for renode memory data connected to a CPU.
    struct cpuMemories
    {
        //! Name.
        QString memoryName_ = "";

        //! ID.
        QString memoryID_ = "";

        //! Class name.
        QString className_ = RenodeConstants::MAPPEDMEMORY;

        //! Base address.
        QString baseAddress_ = "";

        //! Size.
        QString size_ = "";
    };
};

#endif // RENODESTRUCTS_H