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
class RenodeCPUDetailRoutes;

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
     *      @param [in] cpuRoutes               CPU and its connected routes.
     *      @param [in] createCpuFile           Flag for creating a CPU file.
     *      @param [in] createMemoryFile        Flag for creating a memory file.
     *      @param [in] createPeripheralFile    Flag for creating a peripherals file.
     */
    void generate(QSharedPointer<Component> topComponent,
        QString const& componentPath,
        QVector<QSharedPointer<RenodeCPUDetailRoutes> > const& cpuRoutes,
        bool createCpuFile,
        bool createMemoryFile,
        bool createPeripheralFile);

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
     *      @param [in] createMemoryFile        Flag for creating a memory file.
     *      @param [in] createPeripheralFile    Flag for creating a peripherals file.
     */
    void writeFile(QSharedPointer<Component> topComponent,
        QString const& componentPath,
        QSharedPointer<RenodeCPUDetailRoutes> cpuRoute,
        QStringList& fileNames,
        bool createCpuFile,
        bool createMemoryFile,
        bool createPeripheralFile);

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
     *      @param [in] writer          The text stream.
     *      @param [in] currentCPU      The selected CPU.
     *      @param [in] cpuContainer    The CPU data container.
     */
    void writeCPU(QString const& filePath, QSharedPointer<RenodeCPUDetailRoutes> cpuContainer);

    /*!
     *  Write the peripherals of the selected CPU route container.
     *
     *      @param [in] writer              The text stream.
     *      @param [in] renodeCollection    The selected CPU route container.
     *      @param [in] renodeFilePath      The file path to the Renode platform files.
     */
    void writePeripherals(QString const& filePath, QString const& componentPath, QSharedPointer<RenodeCPUDetailRoutes> renodeCollection);

    /*!
     *  Write a single peripheral.
     *
     *      @param [in] stream          The text stream.
     *      @param [in] peripheral      The selected peripheral item.
     *      @param [in] tab             Current tab.
     *      @param [in] renodeFilePath  The file path to the Renode platform files.
     */
    void writePeripheral(QTextStream& stream, QSharedPointer<RenodeStructs::cpuPeripherals> peripheral, QString const& tab, QString const& renodeFilePath);

    /*!
     *  Write the memory of the selected CPU.
     *
     *      @param [in] filePath            File path for the Renode platform files.
     *      @param [in] renodeCollection    The selected CPU route container.
     */
    void writeMemories(QString const& filePath, QSharedPointer<RenodeCPUDetailRoutes> renodeCollection);

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
