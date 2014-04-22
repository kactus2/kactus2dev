//-----------------------------------------------------------------------------
// File: QuartusPinImportPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.11.2013
//
// Description:
// Kactus2 plugin for Quartus II pin import.
//-----------------------------------------------------------------------------

#ifndef QUARTUSPINIMPORTPLUGIN_H
#define QUARTUSPINIMPORTPLUGIN_H

#include "QuartusPinImportPlugin_global.h"

#include <Plugins/PluginSystem/IPlugin.h>
#include <Plugins/PluginSystem/IGeneratorPlugin.h>

#include <QtPlugin>
#include <QObject>
#include <QFile>

class Component;
//-----------------------------------------------------------------------------
// Plugin for importing component ports from Quartus II pin file.
//-----------------------------------------------------------------------------
class QUARTUSPINIMPORTPLUGIN_EXPORT QuartusPinImportPlugin : public QObject, public IGeneratorPlugin
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.quartuspinimportplugin" FILE "quartuspinimportplugin.json")
    Q_INTERFACES(IPlugin)
    Q_INTERFACES(IGeneratorPlugin)

public:
    //! The constructor.
    QuartusPinImportPlugin();

    //! The destructor.
    ~QuartusPinImportPlugin();

    //! Returns the name of the plugin.
    virtual QString const& getName() const;

	//! Returns the version of the plugin.
    virtual QString const& getVersion() const;

	//! Returns the description of the plugin.
    virtual QString const& getDescription() const;

    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString const& getVendor() const;

    /*!
     *  Returns the licence of the plugin.
     */
    virtual QString const& getLicence() const;

    /*!
     *  Returns the holder of the licence of the plugin.
     */
    virtual QString const& getLicenceHolder() const;

	//! Returns the icon for the generator.
    /*!
     *  Returns the settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

    //! Returns the external program requirements of the plugin.
    virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

/*!
     *  Returns the icon for the generator.
     */
    virtual QIcon getIcon() const;

    /*!
     *  Checks whether the generator supports generation for the given library component.
     *
     *      @param [in] libComp		The library component for which to check support.
     *      @param [in] libDesConf	The optional design configuration object.
     *      @param [in] libDes		The optional design object.
     *
     *      @return True, if the generator supports the given component. Otherwise false.
     */
    virtual bool checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp,
		QSharedPointer<LibraryComponent const> libDesConf = QSharedPointer<LibraryComponent const>(),
		QSharedPointer<LibraryComponent const> libDes = QSharedPointer<LibraryComponent const>()) const;

    /*!
     *  Runs the generator.
     *
     *      @param [in]			utility			The plugin utility interface.
     *      @param [in,out]	libComp			The library component for which the generator is run.
     *      @param [in, out]	libDesConf		The optional design configuration object for the generator.
     *      @param [in, out]	libDes			The optional design object.
     */
    virtual void runGenerator(IPluginUtility* utility,
                              QSharedPointer<LibraryComponent> libComp,
							  QSharedPointer<LibraryComponent> libDesConf = QSharedPointer<LibraryComponent>(),
							  QSharedPointer<LibraryComponent> libDes = QSharedPointer<LibraryComponent>());

    void printGeneratorSummary( IPluginUtility* utility );


public slots:

    /*!
     *  Called for storing messages that must be displayed in the message console after the 
     *  generation has completed.
     *
     *      @param [in] message   The message to store.
     */
    void logMessage(QString const& message);

private:

    //! No copying
    QuartusPinImportPlugin(const QuartusPinImportPlugin& other);

    //! No assignment
    QuartusPinImportPlugin& operator=(const QuartusPinImportPlugin& other);
    
    //! List of errors produced by the generator widget.
    QStringList messageList_;
};

#endif // QUARTUSPINIMPORTPLUGIN_H
