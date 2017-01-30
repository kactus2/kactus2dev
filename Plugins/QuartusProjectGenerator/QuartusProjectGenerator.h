//-----------------------------------------------------------------------------
// File: QuartusProjectGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.06.2015
//
// Description:
// The plugin for generating quartus projects containing quartus project files and quartus settings files.
//-----------------------------------------------------------------------------

#ifndef QUARTUSPROJECTGENERATOR_H
#define QUARTUSPROJECTGENERATOR_H

#include "QuartusProjectGenerator_global.h"

#include <Plugins/PluginSystem/GeneratorPlugin/IGeneratorPlugin.h>

#include <IPXACTmodels/Component/Component.h>

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QIcon>
#include <QFileInfo>
#include <QSettings>

class IPluginUtility;
class Design;
class DesignConfiguration;

//-----------------------------------------------------------------------------
// !The plugin for generating quartus projects containing quartus project files and quartus settings files.
//-----------------------------------------------------------------------------
class QUARTUSPROJECTGENERATOR_EXPORT QuartusProjectGenerator : public QObject, public IGeneratorPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "kactus2.plugins.QuartusProjectGenerator" FILE "QuartusProjectGenerator.json")

	Q_INTERFACES(IPlugin)
	Q_INTERFACES(IGeneratorPlugin)

public:

    /*!
	 *  The constructor.
	 */
	QuartusProjectGenerator();
	
	/*!
	 *  The destructor.
	 */
	virtual ~QuartusProjectGenerator();

    /*!
     *  Gets the name of the plugin.
     *
     *      @return The name of the plugin.
     */
    virtual QString getName() const;

    /*!
     *  Gets the version of the plugin.
     *
     *      @return The version of the plugin
     */
    virtual QString getVersion() const;

    /*!
     *  Gets the description of the plugin.
     *
     *      @return The description of the plugin.
     */
    virtual QString getDescription() const;

    /*!
     *  Gets the vendor of the plugin.
     *
     *      @return The vendor of the plugin.
     */
    virtual QString getVendor() const;

    /*!
     *  Gets the license of the plugin.
     *
     *      @return The license of the plugin.
     */
    virtual QString getLicence() const;

    /*!
     *  Returns the holder of the license of the plugin.
     *
     *      @return The holder of the license of the plugin.
     */
    virtual QString getLicenceHolder() const;

    /*!
     *  Returns the settings widget.
     *
     *      @return The settings widget.
     */
    virtual QWidget* getSettingsWidget();

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel(){return NULL;}

    /*!
     *  Returns the icon for the generator.
     *
     *      @return The icon for the generator.
     */
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

    /*!
     *  Get a list of external program requirements of the plugin.
     *
     *      @return A list of external program requirements of the plugin.
     */
    virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

private slots:

    /*!
     *  Handles the error messages of the quartus generator.
     *
     *      @param [in] message     The error message.
     */
    void onErrorMessage(QString const& message);

    /*!
     *  Handles the notification messages of the quartus generator.
     *
     *      @param [in] message     The notification message.
     */
    void onNoticeMessage(QString const& message);

private:

	//! No copying
	QuartusProjectGenerator(const QuartusProjectGenerator& other);

	//! No assignment
	QuartusProjectGenerator& operator=(const QuartusProjectGenerator& other);

    /*!
     *  Gets the view of the current design or design configuration.
	 *
	 *      @param [in] component   The component for which this plugin is run for.
     *      @param [in] desConf		The design configuration object of a hierarchical component.
     *      @param [in] design      The design object of a hierarchical component.
     *
     *      @return The view referencing to the open design.
     */
	QSharedPointer<View> getOpenView(QSharedPointer<Component> component,
		QSharedPointer<DesignConfiguration> desConf, QSharedPointer<Design> design);

	//! The plugin utility.
	IPluginUtility* utility_;
};

#endif // QUARTUSPROJECTGENERATOR_H
