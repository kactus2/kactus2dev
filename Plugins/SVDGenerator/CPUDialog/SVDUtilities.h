//-----------------------------------------------------------------------------
// File: SVDUtilities.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.04.2023
//
// Description:
// Utility functions for SVD generator.
//-----------------------------------------------------------------------------

#ifndef SVDUTILITIES_H
#define SVDUTILITIES_H

#include <QString>

//-----------------------------------------------------------------------------
// Function: SVDUtilities::SVDConstants()
//-----------------------------------------------------------------------------
namespace SVDConstants
{
    //! File type for renode files.
    const QString SVDFILETYPE = "svd";

    //! File type for JSON files.
    const QString JSONFILETYPE = "json";

    //! Identifier for renode configuration file.
    const QString CONFIGURATIONFILEEXTENSION = "svdConfiguration";

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

    //! Identifier for IP-XACT vendor element.
    const QString VENDOR = "vendor";

    //! Identifier for IP-XACT name element.
    const QString NAME = "name";

    //! Identifier for IP-XACT version element.
    const QString VERSION = "version";

    //! Identifier for description element.
    const QString DESCRIPTION = "description";

    //! Identifier for CPU element.
    const QString CPUELEMENT = "cpu";

    //! General type for CPUs.
    const QString OTHERTYPE = "other";

    //! Revision of the CPU.
    const QString CPUREVISION = "revision";

    //! Endian element of the CPU.
    const QString CPUENDIAN = "endian";

    //! MPU presence element.
    const QString CPUMPUPRESENT = "mpuPresent";

    //! FPU presence element.
    const QString CPUFPUPRESENT = "fpuPresent";

    //! NVIC prio bits element.
    const QString CPPUNVICPRIOBITS = "nvicPrioBits";

    //! Vendor-specific system tick timer element.
    const QString CPUVENDORSYSTICKCONFIG = "vendorSystickConfig";

    //! True for booleans in string.
    const QString BOOLEANTRUE = "true";

    //! False for booleans in string.
    const QString BOOLEANFALSE = "false";

    //! Address unit bits element.
    const QString AUB = "addressUnitBits";

    //! Width element.
    const QString WIDTH = "width";

    //! Size element.
    const QString SIZE = "size";

    //! Peripherals element.
    const QString PERIPHERALSELEMENT = "peripherals";

    //! Single peripheral element.
    const QString SINGLEPERIPHERALELEMENT = "peripheral";

    //! Peripheral version element.
    const QString PERIPHERALVERSION = "version";

    //! Peripheral base address element.
    const QString PERIPHERALBASEADDRESS = "baseAddress";

    //! Address block element.
    const QString ADDRESSBLOCKELEMENT = "addressBlock";

    //! Address block offset element.
    const QString ADDRESSBLOCKOFFSET = "offset";

    //! Address block usage: buffer element.
    const QString BUFFERUSAGE = "buffer";

    //! Address block usage: registers element.
    const QString REGISTERSUSAGE = "registers";

    //! Address block usage: reserved element.
    const QString RESERVEDUSAGE = "reserved";

    //! Address block usage element.
    const QString USAGE = "usage";

    //! Registers element.
    const QString REGISTERSELEMENT = "registers";

    //! Register cluster element.
    const QString CLUSTER = "cluster";

    //! Address offset element.
    const QString ADDRESSOFFSET = "addressOffset";

    //! Single register element.
    const QString REGISTERELEMENT = "register";

    //! Register dimension element.
    const QString DIM = "dim";

    //! Register dimension incremenet element.
    const QString DIMINCREMENT = "dimIncrement";

    //! Access element.
    const QString ACCESS = "access";

    //! The used hexadecimal format.
    const QString HEXADECIMAL = "0x";

    //! Reset value element.
    const QString RESETVALUE = "resetValue";

    //! Reset mask element.
    const QString RESETMASK = "resetMask";

    //! Fields element.
    const QString FIELDSELEMENT = "fields";

    //! Single field element.
    const QString SINGLEFIELDELEMENT = "field";

    //! Field bit range element.
    const QString BITRANGE = "bitRange";

    //! Enumerated values element.
    const QString ENUMERATEDVALUESELEMENT = "enumeratedValues";

    //! Single enumerated value element.
    const QString SINGLEENUMERATEDVALUEELEMENT = "enumeratedValue";

    //! Value element.
    const QString VALUE = "value";

    //! CPUs element.
    const QString CONFIGURATIONCPUS = "CPUs";

    //! Create SVD file element.
    const QString CREATESVDFLAG = "createSVD";
};

#endif //RENODEUTILITIES_H