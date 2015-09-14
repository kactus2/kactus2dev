//-----------------------------------------------------------------------------
// File: TLMWGeneratorPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 13.7.2015
//
// Description:
// TLMW generator plugin.
//-----------------------------------------------------------------------------

#ifndef TLMWGeneratorPlugin_H
#define TLMWGeneratorPlugin_H

#include <Plugins/PluginSystem/IGeneratorPlugin.h>
#include <QObject>

#include <TLMWParser.h>

class Component;
class CSourceWriter;
class IPluginUtility;

//-----------------------------------------------------------------------------
//! TLMW code generator.
//-----------------------------------------------------------------------------
class TLMWGeneratorPlugin : public QObject, public IGeneratorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.TLMWGenerator" FILE "tlmw.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(IGeneratorPlugin)

public:
    TLMWGeneratorPlugin();
    ~TLMWGeneratorPlugin();

    /*!
     *  Returns the name of the plugin.
     */
    virtual QString getName() const;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString getVersion() const;

    /*!
     *  Returns the description of the plugin.
     */
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

    /*!
     *  Returns the settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

    /*!
     *  Returns the icon for the generator.
     */
    virtual QIcon getIcon() const;

    /*!
     *  Checks whether the generator supports generation for the given library component.
     *
     *      @param [in] libComp The library component for which to check support.
     *      @param [in] libDesConf The optional design configuration object.
     *      @param [in] libDes The optional design object.
     *
     *      @return True, if the generator supports the given component. Otherwise false.
     */
     virtual bool checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp, 
         QSharedPointer<LibraryComponent const> libDesConf = QSharedPointer<LibraryComponent const>(), 
         QSharedPointer<LibraryComponent const> libDes = QSharedPointer<LibraryComponent const>()) const;

    /*!
     *  Runs the generator.
     *
     *
     *      @param [in]     utility       The plugin utility interface.
     *      @param [in,out] libComp       The component for which the generator is run.
     *      @param [in,out] libDesConf        The optional design configuration object for the generator.
     *      @param [in,out] libDes            The optional design object.
     */
     virtual void runGenerator(IPluginUtility* utility,
         QSharedPointer<LibraryComponent> libComp, 
         QSharedPointer<LibraryComponent> libDesConf = QSharedPointer<LibraryComponent const>(),
         QSharedPointer<LibraryComponent> libDes = QSharedPointer<LibraryComponent>());

     //! \brief Returns the external program requirements of the plugin.
     virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();
};

#endif // TLMWGeneratorPlugin_H
