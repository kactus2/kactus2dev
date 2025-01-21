//-----------------------------------------------------------------------------
// File: PadsPartGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.11.2013
//
// Description:
// Generator for PADS parts.
//-----------------------------------------------------------------------------

#ifndef PADSPARTGENERATOR_H
#define PADSPARTGENERATOR_H

#include "PadsPartGeneratorDialog.h"
#include <KactusAPI/include/IGeneratorPlugin.h>
#include <KactusAPI/include/IPlugin.h>

#include <QtPlugin>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QIcon>
#include <QFileInfo>
#include <QSettings>

class Component;
class Document;
class FileSet;
class IPluginUtility;
class PluginSettingsWidget;

//-----------------------------------------------------------------------------
// class PadsPartGenerator.
//-----------------------------------------------------------------------------
class PadsPartGenerator : public QObject, public IGeneratorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.PadsPartGenerator" FILE "padspartgenerator.json")
    Q_INTERFACES(IPlugin)
    Q_INTERFACES(IGeneratorPlugin)

public:

	/*!
	 *  The constructor.
	 */
	PadsPartGenerator();

    // Disable copying.
    PadsPartGenerator(PadsPartGenerator const& rhs) = delete;

    // Disable assignment.
    PadsPartGenerator& operator=(PadsPartGenerator& rhs) = delete;

	/*!
	 *  The destructor.
	*/
	virtual ~PadsPartGenerator() = default;

    //! Returns the name of the plugin.
    virtual QString getName() const override final;

	//! Returns the version of the plugin.
    virtual QString getVersion() const override final;

	//! Returns the description of the plugin.
    virtual QString getDescription() const override final;

    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString getVendor() const override final;

    /*!
     *  Returns the license of the plugin.
     */
    virtual QString getLicense() const override final;

    /*!
     *  Returns the holder of the license of the plugin.
     */
    virtual QString getLicenseHolder() const override final;

	//! Returns the icon for the generator.
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
    *    @param [in] component	        The component for which to check support. If design is not null, component
    *                                      will refer to design or designConfiguration.
    *    @param [in] design	            The design, if the generator is ran for a design.
    *    @param [in] designConfiguration The design configuration for design, if it is not null.
    *
    *    @return True, if the generator may run the given component. Otherwise false.
    */
    virtual bool checkGeneratorSupport(QSharedPointer<Component const> component,
        QSharedPointer<Design const> design,
        QSharedPointer<DesignConfiguration const> designConfiguration) const override final;

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
        QSharedPointer<DesignConfiguration> designConfiguration) override final;

	 //! Returns the external program requirements of the plugin.
	 virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements() override final;

private:

    /*!
     *  Writes the PADS part file.
     *
     *    @param [in] file            The file to write to.
     *    @param [in] partDescription The part description to write.
     *
     *    @return True, if part file was successfully written, otherwise false.
     */
    bool generatePartFile(QFile* file, QString const& partDescription);

    /*!
     *  Writes the PADS CAE decal file.
     *
     *    @param [in] file            The file to write to.
     *    @param [in] partTitle       The part title displayed on all gates.
     *    @param [in] partDescription The PADS part description.
     *
     *    @return True, if CAE file was successfully written, otherwise false.
     */
    bool generateCAEFile(QFile* file, QString const& partTitle, QString const& partDescription);

    /*!
     *  Generates the CAE decal terminals and prints them into an output stream.
     *
     *    @param [in] output      The output stream for generation.
     *    @param [in] drawSymbol  The rectangle used as a drawing symbol for decal.
     *    @param [in] pinCount    The number of terminals in the decal.
     */
    void insertCAETerminals( QTextStream& output, QRect const& drawSymbol, int pinCount );  

    /*!
     *  Generates the CAE decal and prints it into an output stream.
     *
     *    @param [in] output          The output stream for generation.
     *    @param [in] drawSymbol      The rectangle used as a drawing symbol for decal.
     *    @param [in] gateTitle       The title of the gate.
     *    @param [in] gateName        The name of the gate used as a decal name.      
     */
    void insertCAEDecal(QTextStream& output, QRect const& drawSymbol,
        QString const& gateTitle, QString const& gateName); 
    

    /*!
     *  Generates the CAE header lines and prints them into an output stream.
     *
     *    @param [in] output      The output stream for generation.
     *    @param [in] timestamp   Header line with timestamp to insert.
     *    @param [in] gateName    The name of the gate used as a decal name.
     *    @param [in] pinCount    The number of terminals in the decal.
     */
    void insertCAEHeader( QTextStream& output, QString const& timestamp, QString const& gateName, int pinCount );
   

    // void generateCAETerminals( QTextStream& output, gateGenerationType generationType );

    /*!
     *  Adds a file to a file set with defined file types.
     *
     *    @param [in] fileSet         The file set.
     *    @param [in] filePath        The path to file which is added.
     *    @param [in] basePath        The base path of the component xlm file.
     *    @param [in] fileTypes       The filetypes of the added file.
     *
     *    @return True, if the file was successfully added to file set, otherwise false.
     */
    bool addFileToFileset(QSharedPointer<FileSet> fileSet, QString const& filePath, QString const& basePath,
        const QStringList& fileTypes);

};
#endif // PADSPARTGENERATOR_H
