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

#include <KactusAPI/include/IGeneratorPlugin.h>

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
    virtual QWidget* getSettingsWidget();

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel(){return NULL;}

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
     *      @param [in] component	        The component for which to check support. If design is not null, component
	 *                                      will refer to design or designConfiguration.
     *      @param [in] design	            The design, if the generator is ran for a design.
     *      @param [in] designConfiguration The design configuration for design, if it is not null.
     *
     *      @return True, if the generator may run the given component. Otherwise false.
     */
    virtual bool checkGeneratorSupport(QSharedPointer<Component const> component,
        QSharedPointer<Design const> design,
        QSharedPointer<DesignConfiguration const> designConfiguration) const;
   
    /*!
     *  Runs the generation, creating new files and/or modifying the IP-XACT metadata. The function has
	 *  also access to the parent window widget, so that it can show dialogs for the user to aid the generation.
     *
     *      @param [in] utility			    The plugin utility interface.
     *      @param [in] component	        The component for which to check support. If design is not null, component
     *                                      will refer to design or designConfiguration.
     *      @param [in] design	            The design, if the generator is ran for a design.
     *      @param [in] designConfiguration The design configuration for design, if it is not null.
     */
    virtual void runGenerator(IPluginUtility* utility, 
        QSharedPointer<Component> component,
        QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> designConfiguration);

private:
    // Disable copying.
    MemoryViewGeneratorPlugin(MemoryViewGeneratorPlugin const& rhs);
    MemoryViewGeneratorPlugin& operator=(MemoryViewGeneratorPlugin const& rhs);

    /*!
     *  Saves the generated file to top component fileset.
     *
     *      @param [in] targetFile      The generated file path.
     *      @param [in] component       The top level component.
     *      @param [in] utility         The plugin utility interface.
     *
     *      @return <Description>.
     */
    void saveToFileset(QString const& targetFile, QSharedPointer<Component> component, IPluginUtility* utility);
};

#endif // MEMORYVIEWGENERATORPLUGIN_H
