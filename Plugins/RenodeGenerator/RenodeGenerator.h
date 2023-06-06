//-----------------------------------------------------------------------------
// File: RenodeGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.02.2023
//
// Description:
// Creates Renode files for CPU, memory and peripherals.
//-----------------------------------------------------------------------------

#ifndef RENODEGENERATOR_H
#define RENODEGENERATOR_H

#include "renodegeneratorplugin_global.h"

#include <Plugins/common/ConnectivityGraphUtilities.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

class Component;
class LibraryInterface;
class RenodeCpuRoutesContainer;

#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QTextStream>

//-----------------------------------------------------------------------------
//! Creates Renode files for CPU, memory and peripherals.
//-----------------------------------------------------------------------------
class RENODEGENERATORPLUGIN_EXPORT RenodeGenerator
{
public:
    
    /*!
     *  The constructor.
     *
     *      @param [in] library     Interface for accessing libraries.
     */
    explicit RenodeGenerator(LibraryInterface* library);

    /*!
     *  The destructor.
     */
    ~RenodeGenerator() = default;

    // Disable copying.
    RenodeGenerator(RenodeGenerator const& rhs) = delete;
    RenodeGenerator& operator=(RenodeGenerator const& rhs) = delete;

    /*!
     *  Generates slave memory listing for the given component.
     *
     *      @param [in] topComponent            The top component in the hierarchy to generate listing for.
     *      @param [in] componentPath           Path to the component folder.
     *      @param [in] cpuContainer            CPU and its connected routes.
     *      @param [in] createCpuFile           Flag for creating a CPU file.
     *      @param [in] cpuFileName             Name of the CPU file.
     *      @param [in] createMemoryFile        Flag for creating a memory file.
     *      @param [in] memoryFileName          Name of the memory file.
     *      @param [in] createPeripheralFile    Flag for creating a peripherals file.
     *      @param [in] peripheralFileName      Name of the peripherals file.
     */
    void generate(QSharedPointer<Component> topComponent,
        QString const& componentPath,
        QSharedPointer<RenodeCpuRoutesContainer> const& cpuContainer,
        bool createCpuFile,
        QString const& cpuFileName,
        bool createMemoryFile,
        QString const& memoryFileName,
        bool createPeripheralFile,
        QString const& peripheralFileName);

    /*!
     *  Get the generated files.
     *
     *      @return The generated files.
     */
    QStringList getGeneratedFiles() const;

private:

    /*!
     *  Writes the CPU listing into Renode platform files.
     *
     *      @param [in] topComponent            Top component of the design.
     *      @param [in] componentPath           Path to the component folder.
     *      @param [in] cpuRoute                The selected CPU route container.
     *      @param [in] fileNames               Names of the generated Renode files.
     *      @param [in] createCpuFile           Flag for creating a CPU file.
     *      @param [in] cpuFileName             Name of the CPU file.
     *      @param [in] createMemoryFile        Flag for creating a memory file.
     *      @param [in] memoryFileName          Name of the memory file.
     *      @param [in] createPeripheralFile    Flag for creating a peripherals file.
     *      @param [in] peripheralFileName      Name of the peripherals file.
     */
    void writeFiles(QSharedPointer<Component> topComponent,
        QString const& componentPath,
        QSharedPointer<RenodeCpuRoutesContainer> cpuRoute,
        QStringList& fileNames,
        bool createCpuFile,
        QString const& cpuFileName,
        bool createMemoryFile,
        QString const& memoryFileName,
        bool createPeripheralFile,
        QString const& peripheralFileName);

    /*!
     *  Get a unique file name using the selected file name.
     *
     *      @param [in] fileNames           List of used file names.
     *      @param [in] currentFileName     The selected file name.
     *
     *      @return Unique file name.
     */
    QString getUniqueFileName(QStringList& fileNames, QString const& currentFileName);

    /*!
     *  Get the number of files containing the selected name.
     *
     *      @param [in] fileNames   List of the generated files.
     *      @param [in] fileName    The selected file name.
     *
     *      @return Number of files containing the same name.
     */
    int getFileNumberExtension(QStringList const& fileNames, QString const& fileName) const;

    /*!
     *  Write the selected CPU.
     *
     *      @param [in] filePath        File path for the renode files.
     *      @param [in] cpuContainer    The CPU data container.
     *      @param [in] fileName        Name for the CPU file.
     */
    void writeCPU(QString const& filePath,
        QSharedPointer<RenodeCpuRoutesContainer> cpuContainer,
        QString const& fileName);

    /*!
     *  Write the peripherals of the selected CPU route container.
     *
     *      @param [in] filePath            File path for the renode files.
     *      @param [in] peripheralPath      Path for the renode platform files.
     *      @param [in] renodeCollection    The selected CPU route container.
     *      @param [in] fileName            Name for the peripherals file.
     */
    void writePeripherals(QString const& filePath,
        QString const& peripheralPath,
        QSharedPointer<RenodeCpuRoutesContainer> renodeCollection,
        QString const& fileName);

    /*!
     *  Write a single peripheral.
     *
     *      @param [in] stream          The text stream.
     *      @param [in] peripheral      The selected peripheral item.
     *      @param [in] tab             Current tab.
     *      @param [in] renodeFilePath  The file path to the Renode platform files.
     */
    void writePeripheral(QTextStream& stream,
        QSharedPointer<RenodeStructs::cpuPeripheral> peripheral,
        QString const& tab,
        QString const& renodeFilePath);

    /*!
     *  Write the memory of the selected CPU.
     *
     *      @param [in] filePath            File path for the Renode platform files.
     *      @param [in] renodeCollection    The selected CPU route container.
     *      @param [in] fileName            Name of the memory file.
     */
    void writeMemories(QString const& filePath,
        QSharedPointer<RenodeCpuRoutesContainer> renodeCollection,
        QString const& fileName);

    /*!
     *  Write a single memory item.
     *
     *      @param [in] stream      The text stream.
     *      @param [in] memory      The selected memory item.
     *      @param [in] tab         Current tab.
     */
    void writeMemory(QTextStream& stream, QSharedPointer<RenodeStructs::cpuMemories> memory, QString const& tab);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The available IP-XACT library.
    LibraryInterface* library_;

    //! Paths to the generated files.
    QStringList generatedFiles_;
};

#endif // RENODEGENERATOR_H
