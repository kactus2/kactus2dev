/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator.h
 * 	Project:	Kactus 2
*/

#ifndef MEMORYMAPHEADERGENERATOR_H
#define MEMORYMAPHEADERGENERATOR_H

#include "memorymapheadergenerator_global.h"

#include "localheadersavemodel.h"
#include "globalheadersavemodel.h"
#include "systemheadersavemodel.h"

#include <KactusAPI/include/IGeneratorPlugin.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <QObject>
#include <QSharedPointer>

class IPluginUtility;
class ExpressionParser;
class ExpressionFormatter;

//-----------------------------------------------------------------------------
//! The plugin to generate C-headers of component memory maps.
//-----------------------------------------------------------------------------
class MEMORYMAPHEADERGENERATOR_EXPORT MemoryMapHeaderGenerator : public QObject, public IGeneratorPlugin {

	Q_OBJECT
	Q_PLUGIN_METADATA(IID "kactus2.plugins.MemoryMapHeaderGenerator" FILE "headerGenerator.json")

	Q_INTERFACES(IPlugin)
	Q_INTERFACES(IGeneratorPlugin)

public:

    /*!
	 *  The constructor.
	 */
	MemoryMapHeaderGenerator();
	
	/*!
	 *  The destructor.
	 */
	virtual ~MemoryMapHeaderGenerator();

    /*!
     *  Get the name of the plugin.
     *
     *    @return The name of the plugin.
     */
    virtual QString getName() const;

    /*!
     *  Get the version of the plugin.
     *
     *    @return The version of the plugin.
     */
    virtual QString getVersion() const;

    /*!
     *  Get the description of the plugin.
     *
     *    @return The description of the plugin.
     */
    virtual QString getDescription() const;

    /*!
     *  Get the vendor of the plugin.
     *
     *    @return The vendor of the plugin.
     */
    virtual QString getVendor() const;

    /*!
     *  Get the license of the plugin.
     *
     *    @return The license of the plugin.
     */
    virtual QString getLicense() const;

    /*!
     *  Get the holder of the license of the plugin.
     *
     *    @return The holder of the license of the plugin.
     */
    virtual QString getLicenseHolder() const;

    /*!
     *  Get the settings widget for the plugin.
     *
     *    @return The plugin settings widget.
     */
    virtual QWidget* getSettingsWidget();

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel(){return NULL;}

    /*!
     *  Get the icon for the generator.
     *
     *    @return The icon for the generator.
     */
    virtual QIcon getIcon() const;

	 /*!
     *  Checks whether the generator may run for the given component or design. 
     *
     *    @param [in] component	        The component for which to check support. If design is not null, component
	 *                                      will refer to design or designConfiguration.
     *    @param [in] design	            The design, if the generator is ran for a design.
     *    @param [in] designConfiguration The design configuration for design, if it is not null.
     *
     *    @return True, if the generator may run the given component. Otherwise false.
     */
    virtual bool checkGeneratorSupport(QSharedPointer<Component const> component,
        QSharedPointer<Design const> design,
        QSharedPointer<DesignConfiguration const> designConfiguration) const;

	 /*!
     *  Runs the generation, creating new files and/or modifying the IP-XACT metadata. The function has
	 *  also access to the parent window widget, so that it can show dialogs for the user to aid the generation.
     *
     *    @param [in] utility			    The plugin utility interface.
     *    @param [in] component	        The component for which to check support. If design is not null, component
     *                                      will refer to design or designConfiguration.
     *    @param [in] design	            The design, if the generator is ran for a design.
     *    @param [in] designConfiguration The design configuration for design, if it is not null.
     */
    virtual void runGenerator(IPluginUtility* utility, 
        QSharedPointer<Component> component,
        QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> designConfiguration);

	 /*!
	  *  Get the external program requirements of the plugin.
	  *
      *    @return A list of external program requirements.
	  */
	 virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

     /*!
      *  Set the save options for a local writer.
      *
      *    @param [in] localMemoryMap     The local memory map of an address space.
      *    @param [in] fileInfo           The file information for the header file.
      *    @param [in] swViewName         The name of the sw view.
      */
     void setLocalSaveFileOptions(QSharedPointer<MemoryMap> localMemoryMap, QFileInfo fileInfo, QString swViewName);

     /*!
      *  Set the save options for a global writer.
      *
      *    @param [in] component              The top component of the design.
      *    @param [in] instanceName           The name of the instance containing the interface.
      *    @param [in] instanceId             The id of the instance.
      *    @param [in] masterInterfaceName    The name of the master interface to create the header for.
      *    @param [in] fileInfo               The file information for the header file.
      */
     void setGlobalSaveFileOptions(QSharedPointer<Component> component, QString const& instanceName,
         QString const& instanceId, QString const& masterInterfaceName, QFileInfo fileInfo);

     /*!
      *  Set the save options for a system design writer.
      */
     void setSystemSaveFileOptions();

private:

	//! No copying
	MemoryMapHeaderGenerator(const MemoryMapHeaderGenerator& other);

	//! No assignment
	MemoryMapHeaderGenerator& operator=(const MemoryMapHeaderGenerator& other);

	//! The plugin utility.
	IPluginUtility* utility_;

	//! Pointer to the design being operated when creating global headers.
	QSharedPointer<Design> design_;

    //! Save file options for a local header writer.
    QList<LocalHeaderSaveModel::SaveFileOptions*> localSaveOptions_;

    //! Save file options for a global header writer
    QList<GlobalHeaderSaveModel::SaveFileOptions*> globalSaveOptions_;

};

#endif // MEMORYMAPHEADERGENERATOR_H
