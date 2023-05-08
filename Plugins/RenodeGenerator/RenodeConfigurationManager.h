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

class RenodeCPUDetailRoutes;

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
     *      @param [in] library             Interface for accessing the library.
     *      @param [in] topComponent        Top level component.
     */
    void createConfigureFile(QVector<QSharedPointer<RenodeCPUDetailRoutes> > renodeData,
        QString const& selectedView,
        bool saveToFileSet,
        QString const& selectedFileSet,
        QString const& folderPath,
        bool writeCPU,
        bool writeMemory,
        bool writePeripherals,
        QSharedPointer<Component> topComponent);

private:

    /*!
     *  Create a JSON document for storing the generator configurations.
     *
     *      @param [in] renodeData          Container for renode generator data.
     *      @param [in] selectedView        Name of the view to use in the generator.
     *      @param [in] saveToFileSetFlag   Flag for save to file set.
     *      @param [in] selectedFileSet     Name of the target file set.
     *      @param [in] folderPath          Path for the renode files.
     *      @param [in] writeCPU            Flag for writing a CPU file.
     *      @param [in] writeMemory         Flag for writing a memory file.
     *      @param [in] writePeripherals    Flag for writing a peripherals file.
     *
     *      @return The JSON configuration document.
     */
    QJsonDocument createJsonDocument(QVector<QSharedPointer<RenodeCPUDetailRoutes> > renodeData,
        QString const& selectedView,
        bool saveToFileSetFlag,
        QString const& selectedFileSet,
        QString const& folderPath,
        bool writeCPU,
        bool writeMemory,
        bool writePeripherals);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
};

#endif // RENODECONFIGURATIONMANAGER_H
