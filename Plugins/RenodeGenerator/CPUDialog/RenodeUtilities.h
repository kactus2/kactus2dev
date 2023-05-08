//-----------------------------------------------------------------------------
// File: RenodeUtilities.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.03.2023
//
// Description:
// Utility functions for Renode generator.
//-----------------------------------------------------------------------------

#ifndef RENODEUTILITIES_H
#define RENODEUTILITIES_H

#include <Plugins/RenodeGenerator/CPUDialog/RenodeCPUDetailRoutes.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QString>
#include <QJsonObject>

//-----------------------------------------------------------------------------
//! Utility functions for Renode generator.
//-----------------------------------------------------------------------------
namespace RenodeUtilities
{
    /*!
     *  Get CPU detail routes for Renode generator.
     *
     *      @param [in] configurationObject     JSON object containing the editor configuration.
     *      @param [in] library                 Interface for accessing the library.
     *      @param [in] component               Top component of the design.
     *      @param [in] viewName                The currently active view.
     *
     *      @return List of cpu detail routes for Renode generator.
     */
    QVector<QSharedPointer<RenodeCPUDetailRoutes> > getRenodeCpuRoutes(
        QJsonObject const& configurationObject,
        LibraryInterface* library,
        QSharedPointer<Component> component,
        QString const& viewName);
};

//-----------------------------------------------------------------------------
// Function: RenodeUtilities::RenodeConstants()
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

    //! File name for memory.
    const QString RENODEMEMORYFILENAME = "memory";

    //! File name for peripherals.
    const QString RENODEPERIPHERALFILENAME = "peripherals";

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
};

#endif //RENODEUTILITIES_H