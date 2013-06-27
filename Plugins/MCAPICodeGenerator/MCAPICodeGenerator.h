//-----------------------------------------------------------------------------
// File: McapiCodeGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// MCAPI code generator plugin.
//-----------------------------------------------------------------------------

#ifndef MCAPICODEGENERATOR_H
#define MCAPICODEGENERATOR_H

#include <PluginSystem/IGeneratorPlugin.h>

#include <QObject>

class Component;
class CSourceWriter;
class IPluginUtility;

//-----------------------------------------------------------------------------
//! MCAPI code generator.
//-----------------------------------------------------------------------------
class MCAPICodeGenerator : public QObject, public IGeneratorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.MCAPICodeGenerator" FILE "mcapi.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(IGeneratorPlugin)

public:
    MCAPICodeGenerator();
    ~MCAPICodeGenerator();

    /*!
     *  Returns the name of the plugin.
     */
    virtual QString const& getName() const;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString const& getVersion() const;

    /*!
     *  Returns the description of the plugin.
     */
    virtual QString const& getDescription() const;

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
     *      @param [in,out] libDesConf		The optional design configuration object for the generator.
     *      @param [in,out] libDes			The optional design object.
     */
	 virtual void runGenerator(IPluginUtility* utility,
		 QSharedPointer<LibraryComponent> libComp, 
		 QSharedPointer<LibraryComponent> libDesConf = QSharedPointer<LibraryComponent const>(),
		 QSharedPointer<LibraryComponent> libDes = QSharedPointer<LibraryComponent>());

	 //! \brief Returns the external program requirements of the plugin.
	 virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

private:
    /*!
     *  Generates the MCAPI code header file.
     *
     *      @param [in] filename   The name of the header file to write.
     *      @param [in] component  The owner component.
     */
    void generateHeader(QString const& filename, QSharedPointer<Component> component);

    /*!
     *  Generates the MCAPI code source file.
     *
     *      @param [in] filename   The name of the source file to write.
     *      @param [in] component  The owner component.
     */
    void generateSource(QString const& filename, QSharedPointer<Component> component);

    /*!
     *  Generates the template source containing the program main().
     *
     *      @param [in] filename   The name of the file to write.
     *      @param [in] component  The owner component.
     */
    void generateMainTemplate(QString const& filename, QSharedPointer<Component> component);

    /*!
     *  Creates the indent string based on the application settings.
     */
    QString createIndentString();

    /*!
     *  Writes an MCAPI wait call to the source.
     *
     *      @param [in] writer       The source writer.
     *      @param [in] requestName  The name of the request variable.
     *      @param [in] sizeName     The name of the size variable.
     */
    void writeWaitCall(CSourceWriter& writer, QString const& requestName, QString const& sizeName);

    /*!
     *  Writes an MCAPI status check.
     *
     *      @param [in] writer The source writer.
     */
    void writeStatusCheck(CSourceWriter &writer);

    /*!
     *  Generates the initializeMCAPI() function based on the component.
     *
     *      @param [in] writer     The source writer.
     *      @param [in] component  The owner component.
     */
    void generateInitializeMCAPIFunc(CSourceWriter& writer, QSharedPointer<Component> component);

    /*!
     *  Generates the closeConnections() function based on the component.
     *
     *      @param [in] writer     The source writer.
     *      @param [in] component  The owner component.
     */
    void generateCloseConnectionsFunc(CSourceWriter& writer, QSharedPointer<Component> component);

    /*!
     *  Generates the createConnections() function based on the component.
     *
     *      @param [in] writer     The source writer.
     *      @param [in] component  The owner component.
     */
    void generateCreateConnectionsFunc(CSourceWriter& writer, QSharedPointer<Component> component);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The plugin utility.
    IPluginUtility* utility_;
};

#endif // MCAPICODEGENERATOR_H
