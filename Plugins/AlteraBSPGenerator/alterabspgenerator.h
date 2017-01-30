/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * File name:	alterabspgenerator.h
 * Project:	Kactus 2
*/

#ifndef ALTERABSPGENERATOR_H
#define ALTERABSPGENERATOR_H

#include "alterabspgenerator_global.h"

#include <Plugins/PluginSystem/GeneratorPlugin/IGeneratorPlugin.h>

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <QFileInfo>

class IPluginUtility;

/*! \brief The plugin to generate C-headers of component memory maps.
 *
 */
class ALTERABSPGENERATOR_EXPORT AlteraBSPGenerator : public QObject, public IGeneratorPlugin {

	Q_OBJECT
	Q_PLUGIN_METADATA(IID "kactus2.plugins.AlteraBSPGenerator" FILE "alteraBSPGenerator.json")

	Q_INTERFACES(IPlugin)
	Q_INTERFACES(IGeneratorPlugin)

public:

	/*! \brief The constructor
	 *
	 * Method: 		AlteraBSPGenerator
	 * Full name:	AlteraBSPGenerator::AlteraBSPGenerator
	 * Access:		public 
	 *
	*/
	AlteraBSPGenerator();
	
	//! \brief The destructor
	virtual ~AlteraBSPGenerator();

	//! \brief Returns the name of the plugin.
    virtual QString getName() const;

	//! \brief Returns the version of the plugin.
    virtual QString getVersion() const;

	//! \brief Returns the description of the plugin.
    virtual QString getDescription() const;

    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString getVendor() const;

    /*!
     *  Returns the licence of the plugin.
     */
    virtual QString getLicence() const;

    /*!
     *  Returns the holder of the licence of the plugin.
     */
    virtual QString getLicenceHolder() const;

	//! \brief Returns the icon for the generator.
    /*!
     *  Returns the settings widget.
     */
    virtual QWidget* getSettingsWidget();

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel(){return NULL;}

    /*!
     *  Returns the icon for the generator.
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

	 //! \brief Returns the external program requirements of the plugin.
	 virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

private:

	//! \brief No copying
	AlteraBSPGenerator(const AlteraBSPGenerator& other);

	//! \brief No assignment
	AlteraBSPGenerator& operator=(const AlteraBSPGenerator& other);

	/*! \brief Parse the entries in the file system and add them to specified file set.
	 *
	 * Method: 		addEntry
	 * Full name:	AlteraBSPGenerator::addEntry
	 * Access:		private 
	 *
	 * \param entry The entry to add.
	 * \param xmlPath Path to the top component xml file.
	 * \param fileSet Pointer to the file set to add the entries to.
	 * \param settings Contains the file type settings.
	 *
	*/
	void addEntry(const QFileInfo &entry, 
		const QString& xmlPath,
		QSharedPointer<FileSet> fileSet,
		QSettings& settings);

	//! The plugin utility.
	IPluginUtility* utility_;
};

#endif // ALTERABSPGENERATOR_H
