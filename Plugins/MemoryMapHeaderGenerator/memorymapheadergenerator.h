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

#include <Plugins/PluginSystem/IGeneratorPlugin.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>

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
     *      @return The name of the plugin.
     */
    virtual QString getName() const;

    /*!
     *  Get the version of the plugin.
     *
     *      @return The version of the plugin.
     */
    virtual QString getVersion() const;

    /*!
     *  Get the description of the plugin.
     *
     *      @return The description of the plugin.
     */
    virtual QString getDescription() const;

    /*!
     *  Get the vendor of the plugin.
     *
     *      @return The vendor of the plugin.
     */
    virtual QString getVendor() const;

    /*!
     *  Get the licence of the plugin.
     *
     *      @return The licence of the plugin.
     */
    virtual QString getLicence() const;

    /*!
     *  Get the holder of the licence of the plugin.
     *
     *      @return The holder of the licence of the plugin.
     */
    virtual QString getLicenceHolder() const;

    /*!
     *  Get the settings widget for the plugin.
     *
     *      @return The plugin settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

    /*!
     *  Get the icon for the generator.
     *
     *      @return The icon for the generator.
     */
    virtual QIcon getIcon() const;

	 /*!
	  *  Checks whether the generator supports generation for the given library component.
	  *
	  *      @param [in] libComp        The library component for which to check support.
	  *      @param [in] libDesConf     The design configuration of a hierarchical component.
	  *      @param [in] libDes         The design of a hierarchical component.
      *
      *     @return True, if the generator supports the given component, false otherwise.
	  */
	 virtual bool checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp,
		 QSharedPointer<LibraryComponent const> libDesConf = QSharedPointer<LibraryComponent const>(),
		 QSharedPointer<LibraryComponent const> libDes = QSharedPointer<LibraryComponent const>()) const;

	 /*!
	  *  Runs the generator.
	  *
	  *      @param [in] utility        The plugin utility interface.
	  *      @param [in] libComp        The component for which the generator is run.
	  *      @param [in] libDesConf     The design configuration of a hierarchical component.
	  *      @param [in] libDes         The design of a hierarchical component.
	  */
	 virtual void runGenerator(IPluginUtility* utility, QSharedPointer<LibraryComponent> libComp,
         QSharedPointer<LibraryComponent> libDesConf = QSharedPointer<LibraryComponent>(),
         QSharedPointer<LibraryComponent> libDes = QSharedPointer<LibraryComponent>());

	 /*!
	  *  Get the external program requirements of the plugin.
	  *
      *     @return A list of external program requirements.
	  */
	 virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

     /*!
      *  Set the save options for a local writer.
      *
      *      @param [in] localMemoryMap     The local memory map of an address space.
      *      @param [in] fileInfo           The file information for the header file.
      *      @param [in] swViewName         The name of the sw view.
      */
     void setLocalSaveFileOptions(QSharedPointer<MemoryMap> localMemoryMap, QFileInfo fileInfo, QString swViewName);

     /*!
      *  Set the save options for a global writer.
      *
      *      @param [in] component              The top component of the design.
      *      @param [in] instanceName           The name of the instance containing the interface.
      *      @param [in] instanceId             The id of the instance.
      *      @param [in] masterInterfaceName    The name of the master interface to create the header for.
      *      @param [in] fileInfo               The file information for the header file.
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
