//-----------------------------------------------------------------------------
// File: RenodeConfigurationManager.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.04.2023
//
// Description:
// Manager for storing and restoring renode editor configurations.
//-----------------------------------------------------------------------------

#ifndef RENODECONFIGURATIONMANAGER_H
#define RENODECONFIGURATIONMANAGER_H

#include <Plugins/common/PluginConfigurationManager.h>

class RenodeCpuRoutesContainer;

#include <QJsonArray>

//-----------------------------------------------------------------------------
//! Manager for storing and restoring renode editor configurations.
//-----------------------------------------------------------------------------
class RenodeConfigurationManager : public PluginConfigurationManager
{

public:
    
    /*!
     *  The constructor.
     */
    RenodeConfigurationManager(IPluginUtility* utility);
    
    /*!
     * The destructor.
     */
    ~RenodeConfigurationManager() = default;
    
    /*!
     *  Get the configuration object of a single CPU.
     *
     *      @param [in] component   The containing component.
     *      @param [in] utility     Utilities for enabling plugin execution.
     *
     *      @return The configuration object.
     */
    QJsonObject getConfigurationObject(QSharedPointer<Component> component);
    
    /*!
     *  Create a configuration file.
     *
     *      @param [in] renodeData          Container for renode generator data.
     *      @param [in] selectedView        Name of the view to use in the generator.
     *      @param [in] saveToFileSet       Flag for save to file set.
     *      @param [in] selectedFileSet     Name of the target file set.
     *      @param [in] folderPath          Path for the renode files.
     *      @param [in] writeCPU            Flag for writing a CPU file.
     *      @param [in] writeMemory         Flag for writing a memory file.
     *      @param [in] writePeripherals    Flag for writing a peripherals file.
     *      @param [in] selectedCpu         Name of the active CPU.
     *      @param [in] topComponent        Top level component.
     */
    void createConfigureFile(QSharedPointer<RenodeCpuRoutesContainer> renodeData,
        QString const& selectedView,
        bool saveToFileSet,
        QString const& selectedFileSet,
        QString const& folderPath,
        bool writeCPU,
        bool writeMemory,
        bool writePeripherals,
        QString const& selectedCpu,
        QSharedPointer<Component> topComponent);

private:

    /*!
     *  Create a JSON document for storing the generator configurations.
     *
     *      @param [in] renodeCpu           Container for renode generator data.
     *      @param [in] selectedView        Name of the view to use in the generator.
     *      @param [in] saveToFileSetFlag   Flag for save to file set.
     *      @param [in] selectedFileSet     Name of the target file set.
     *      @param [in] folderPath          Path for the renode files.
     *      @param [in] writeCPU            Flag for writing a CPU file.
     *      @param [in] writeMemory         Flag for writing a memory file.
     *      @param [in] writePeripherals    Flag for writing a peripherals file.
     *      @param [in] selectedCpu         Name of the active CPU.
     *
     *      @return The JSON configuration document.
     */
    QJsonDocument createJsonDocument(QSharedPointer<RenodeCpuRoutesContainer> renodeCpu,
        QString const& selectedView,
        bool saveToFileSetFlag,
        QString const& selectedFileSet,
        QString const& folderPath,
        bool writeCPU,
        bool writeMemory,
        bool writePeripherals,
        QString const& selectedCpu);

    /*!
     *  Remove the selected CPU container from the CPU configuration array.
     *
     *      @param [in] cpuContainer    The selected CPU container.
     */
    void removeCpuContainerFromConfigurationArray(QSharedPointer<RenodeCpuRoutesContainer> cpuContainer);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Configuration array for CPUs.
    QJsonArray cpuArray_;
};

#endif // RENODECONFIGURATIONMANAGER_H
