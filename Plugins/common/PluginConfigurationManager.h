//-----------------------------------------------------------------------------
// File: PluginConfigurationManager.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.04.2023
//
// Description:
// Manager for storing and restoring plugin editor configurations.
//-----------------------------------------------------------------------------

#ifndef PLUGINCONFIGURATIONMANAGER_H
#define PLUGINCONFIGURATIONMANAGER_H

#include <KactusAPI/include/IPluginUtility.h>

#include <Plugins/common/ConfigurationManager.h>

//-----------------------------------------------------------------------------
//! Manager for storing and restoring plugin editor configurations.
//-----------------------------------------------------------------------------
class PluginConfigurationManager : public ConfigurationManager
{

public:
    
    /*!
     *  The constructor.
     */
    PluginConfigurationManager(IPluginUtility* utility);
    
    /*!
     * The destructor.
     */
    ~PluginConfigurationManager() = default;

private:

    /*!
     *  Print the selected error.
     *
     *      @param [in] errorMessage    The selected error message.
     */
    virtual void printError(QString const& errorMessage) const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Utility for plugins.
    IPluginUtility* utility_;
};

#endif // CONFIGURATIONMANAGER_H
