//-----------------------------------------------------------------------------
// File: MemoryViewGeneratorPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.04.2016
//
// Description:
// Plugin for creating a CSV listing of all slave memories within a design hierarchy.
//-----------------------------------------------------------------------------

#ifndef MEMORYVIEWGENERATORPLUGIN_H
#define MEMORYVIEWGENERATORPLUGIN_H

#include "memoryviewgenerator_global.h"

#include <Plugins/PluginSystem/IPlugin.h>
#include <Plugins/PluginSystem/GeneratorPlugin/IGeneratorPlugin.h>

//-----------------------------------------------------------------------------
//! Plugin for creating a CSV listing of all slave memories within a design hierarchy.
//-----------------------------------------------------------------------------
class MEMORYVIEWGENERATOR_EXPORT MemoryViewGeneratorPlugin : public QObject, public IGeneratorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.MemoryViewGeneratorPlugin" FILE "MemoryViewGenerator.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(IGeneratorPlugin)

public:
    
    //! The constructor.
    MemoryViewGeneratorPlugin();

    //! The destructor.
    ~MemoryViewGeneratorPlugin();

    //! Returns the name of the plugin.
    virtual QString getName() const;

    //! Returns the version of the plugin.
    virtual QString getVersion() const;

    //! Returns the description of the plugin.
    virtual QString getDescription() const;

    //! Returns the vendor of the plugin.     
    virtual QString getVendor() const;

    //! Returns the licence of the plugin.
    virtual QString getLicence() const;

    //! Returns the license holder i.e. who is the plugin licensed to.
    virtual QString getLicenceHolder() const;

    /*!
     *  This can be used to show custom plugin-specific settings UI in the Kactus2 settings dialog.
	 *  If the plugin has no custom settings, a null pointer should be returned.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

    /*!
     *  This can be used to indicate what external programs the plugin needs in order to operate correctly.
	 *  This allows Kactus2 to show settings to set paths where those external program are located.
     */
	 virtual QList<ExternalProgramRequirement> getProgramRequirements();
    
    //! Returns the icon for the generator.
    virtual QIcon getIcon() const;
    
    /*!
     *  Checks whether the generator may run for the given component or design. 
     *
     *      @param [in] libComp		The component for which to check support. If libDes is not null, libComp
	 *                              must refer to libDes or libDesConf.
     *      @param [in] libDesConf	The design configuration for design libDes, if it is not null.
     *      @param [in] libDes	    The design, if the generator is ran for a design.
     *
     *      @return True, if the generator may run the given component. Otherwise false.
     */
    virtual bool checkGeneratorSupport(QSharedPointer<Document const> libComp,
        QSharedPointer<Document const> libDesConf = QSharedPointer<Document const>(), 
        QSharedPointer<Document const> libDes = QSharedPointer<Document const>()) const;
   
    /*!
     *  Runs the generation, creating new files.
     *
     *      @param [in]		utility			The plugin utility interface.
     *      @param [in]     libComp			The component for which the generator is run.
     *      @param [in]	    libDesConf		The design configuration for design libDes, if it is not null.
     *      @param [in]	    libDes			The design, if the generator is ran for a design.
     */
    virtual void runGenerator(IPluginUtility* utility, QSharedPointer<Document> libComp,
        QSharedPointer<Document> libDesConf = QSharedPointer<Document>(),
        QSharedPointer<Document> libDes = QSharedPointer<Document>());

private:
    // Disable copying.
    MemoryViewGeneratorPlugin(MemoryViewGeneratorPlugin const& rhs);
    MemoryViewGeneratorPlugin& operator=(MemoryViewGeneratorPlugin const& rhs);

};

#endif // MEMORYVIEWGENERATORPLUGIN_H
