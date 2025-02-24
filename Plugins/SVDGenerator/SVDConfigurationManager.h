//-----------------------------------------------------------------------------
// File: SVDConfigurationManager.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.04.2023
//
// Description:
// Manager for storing and restoring SVD editor configurations.
//-----------------------------------------------------------------------------

#ifndef SVDCONFIGURATIONMANAGER_H
#define SVDCONFIGURATIONMANAGER_H

#include <Plugins/common/PluginConfigurationManager.h>

class SVDCpuRoutesContainer;

//-----------------------------------------------------------------------------
//! Manager for storing and restoring SVD editor configurations.
//-----------------------------------------------------------------------------
class SVDConfigurationManager : public PluginConfigurationManager
{

public:
    
    /*!
     *  The constructor.
     */
    SVDConfigurationManager(IPluginUtility* utility);
    
    /*!
     * The destructor.
     */
    ~SVDConfigurationManager() = default;
    
    /*!
     *  Get the configuration object of a single CPU.
     *
     *    @param [in] component   The containing component.
     *
     *    @return The configuration object.
     */
    QJsonObject getConfigurationObject(QSharedPointer<Component> component);
    
    /*!
     *  Create a configuration file.
     *
     *    @param [in] svdData             Container for SVD generator data.
     *    @param [in] topComponent        Top level component.
     *    @param [in] selectedView        Name of the view to use in the generator.
     *    @param [in] saveToFileSet       Flag for save to file set.
     *    @param [in] selectedFileSet     Name of the target file set.
     *    @param [in] folderPath          Path for the renode files.
     */
    void createConfigureFile(QVector<QSharedPointer<SVDCpuRoutesContainer> > svdData,
        QSharedPointer<Component> topComponent,
        QString const& selectedView,
        bool saveToFileSet,
        QString const& selectedFileSet,
        QString const& folderPath);

private:

    /*!
     *  Create a JSON document for storing the generator configurations.
     *
     *    @param [in] svdData             Container for SVD generator data.
     *    @param [in] selectedView        Name of the view to use in the generator.
     *    @param [in] saveToFileSetFlag   Flag for save to file set.
     *    @param [in] selectedFileSet     Name of the target file set.
     *    @param [in] folderPath          Path for the renode files.
     *
     *    @return The JSON configuration document.
     */
    QJsonDocument createJsonDocument(QVector<QSharedPointer<SVDCpuRoutesContainer> > svdData,
        QString const& selectedView,
        bool saveToFileSetFlag,
        QString const& selectedFileSet,
        QString const& folderPath);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
};

#endif // SVDCONFIGURATIONMANAGER_H
