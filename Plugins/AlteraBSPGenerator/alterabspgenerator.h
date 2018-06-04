//-----------------------------------------------------------------------------
// File: alterabspgenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 15.2.2013
//
// Description:
// Generator plugin for Altera Board Support Package (BSP) files.
//-----------------------------------------------------------------------------


#ifndef ALTERABSPGENERATOR_H
#define ALTERABSPGENERATOR_H

#include "alterabspgenerator_global.h"

#include <Plugins/PluginSystem/GeneratorPlugin/IGeneratorPlugin.h>

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <QFileInfo>

class IPluginUtility;

//-----------------------------------------------------------------------------
//! Generator plugin for Altera Board Support Package (BSP) files.
//-----------------------------------------------------------------------------
class ALTERABSPGENERATOR_EXPORT AlteraBSPGenerator : public QObject, public IGeneratorPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "kactus2.plugins.AlteraBSPGenerator" FILE "alteraBSPGenerator.json")

	Q_INTERFACES(IPlugin)
	Q_INTERFACES(IGeneratorPlugin)

public:

	/*!  The constructor
	 *
	 * Method: 		AlteraBSPGenerator
	 * Full name:	AlteraBSPGenerator::AlteraBSPGenerator
	 * Access:		public 
	 *
	*/
	AlteraBSPGenerator();

    //!  No copying
    AlteraBSPGenerator(const AlteraBSPGenerator& other) = delete;

    //!  No assignment
    AlteraBSPGenerator& operator=(const AlteraBSPGenerator& other) = delete;

	//!  The destructor
	virtual ~AlteraBSPGenerator() = default;

	//!  Returns the name of the plugin.
    virtual QString getName() const override final;

	//!  Returns the version of the plugin.
    virtual QString getVersion() const override final;

	//!  Returns the description of the plugin.
    virtual QString getDescription() const override final;

    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString getVendor() const override final;

    /*!
     *  Returns the licence of the plugin.
     */
    virtual QString getLicence() const override final;

    /*!
     *  Returns the holder of the licence of the plugin.
     */
    virtual QString getLicenceHolder() const override final;

	//!  Returns the icon for the generator.
    /*!
     *  Returns the settings widget.
     */
    virtual QWidget* getSettingsWidget() override final;

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel() override final {return nullptr;}

    /*!
     *  Returns the icon for the generator.
     */
    virtual QIcon getIcon() const override final;

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
        QSharedPointer<DesignConfiguration const> designConfiguration) const override final;

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
        QSharedPointer<DesignConfiguration> designConfiguration) override final;

	 //!  Returns the external program requirements of the plugin.
	 virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements() override final;

private:

	/*!
	 * Parse the entries in the file system and add them to specified file set.
	 *
	 *     @param [in] entry    The entry to add.
	 *     @param [in] xmlPath  Path to the top component xml file.
	 *     @param [in] fileSet  The file set to add the entries to.
	 *     @param [in] settings  Contains the file type settings.
	 */
	 void addEntry(const QFileInfo &entry, const QString& xmlPath, QSharedPointer<FileSet> fileSet,
		QSettings& settings);

	//! The plugin utility.
	IPluginUtility* utility_;
};

#endif // ALTERABSPGENERATOR_H
