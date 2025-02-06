//-----------------------------------------------------------------------------
// File: RenodeConstants.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.05.2023
//
// Description:
// Utility constants for Renode generator.
//-----------------------------------------------------------------------------

#ifndef RENODECONSTANTS_H
#define RENODECONSTANTS_H

#include <QString>

//-----------------------------------------------------------------------------
//! Utility constants for Renode generator.
//-----------------------------------------------------------------------------
namespace RenodeConstants
{
    //! File type for renode files.
    const QString RENODEFILETYPE = "repl";

    //! File type for python files.
    const QString PYTHONFILETYPE = "py";

    //! File type for JSON files.
    const QString JSONFILETYPE = "json";

    //! File name for CPU.
    const QString RENODECPUFILENAME = "cpus";

    //! Identifier for CPU file name.
    const QString RENODECPUFILEIDENTIFIER = "cpuFileName";

    //! File name for memory.
    const QString RENODEMEMORYFILENAME = "memory";

    //! Identifier for memory file name.
    const QString RENODEMEMORYFILEIDENTIFIER = "memoryFileName";

    //! File name for peripherals.
    const QString RENODEPERIPHERALFILENAME = "peripherals";

    //! Identifier for peripherals file name.
    const QString RENODEPERIPHERALFILEIDENTIFIER = "peripheralsFileName";

    //! Identifier for multiple CPUs.
    const QString CPUS = "CPUs";

    //! Identifier for CPU configuration.
    const QString SINGLECPU = "CPU";

    //! Identifier for CPU name.
    const QString CPUNAME = "name";

    //! Identifier for CPU class configuration.
    const QString CPUCLASS = "class";

    //! Identifier for CPU type configuration.
    const QString CPUTYPE = "type";

    //! Identifier for CPU time provider configuration.
    const QString CPUTIME = "timeProvider";

    //! Identifier for memory configuration.
    const QString MEMORY = "memory";

    //! Identifier for memory name configuration.
    const QString MEMORYNAME = "name";

    //! Identifier for memory class configuration.
    const QString MEMORYCLASS = "class";

    //! Identifier for peripherals configuration.
    const QString PERIPHERALS = "peripherals";

    //! Identifier for peripheral name configuration.
    const QString PERIPHERALNAME = "name";

    //! Identifier for peripheral class configuration.
    const QString PERIPHERALCLASS = "class";

    //! Identifier for peripheral initable configuration.
    const QString PERIPHERALINITABLE = "initable";

    //! Identifier for peripheral file path configuration.
    const QString PERIPHERALPATH = "filePath";

    //! Identifier for peripheral template configuration.
    const QString PERIPHERALTEMPLATE = "template";

    //! Identifier for view selection.
    const QString VIEW = "view";

    //! Identifier for save to fileset flag.
    const QString SAVETOFILESET = "saveToFileSet";

    //! Identifier for file set name.
    const QString FILESET = "fileSet";

    //! Identifier for generation destination.
    const QString FOLDERPATH = "destinationFolder";

    //! Identifier for write flags.
    const QString WRITEFILES = "writeFiles";

    //! Identifier for renode configuration file.
    const QString CONFIGURATIONFILEEXTENSION = "renodeConfiguration";

    //! Python peripheral class for renode peripherals.
    const QString PYTHONPERIPHERAL = "Python.PythonPeripheral";

    //! Default file path for python peripherals.
    const QString PYTHONPERIPHERALFILEPATH = "python_peripherals";

    //! Default memory class.
    const QString MAPPEDMEMORY = "Memory.MappedMemory";
};

//-----------------------------------------------------------------------------
//! Utility constants for Renode generator peripheral templates.
//-----------------------------------------------------------------------------
namespace RenodeTemplateConstants
{
    //! Identifier for template array.
    const QString TEMPLATEARRAYKEY = "Templates";

    //! Identifier for template name.
    const QString TEMPLATENAME = "name";

    //! Identifier for template path.
    const QString TEMPLATEFILE = "template";

    //! Identifier for template target folder.
    const QString TARGETPATH = "targetPath";
};

#endif // RENODECONSTANTS_H