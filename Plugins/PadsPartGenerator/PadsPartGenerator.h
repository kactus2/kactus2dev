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
#include <Plugins/PluginSystem/IGeneratorPlugin.h>
#include <Plugins/PluginSystem/IPlugin.h>

#include <QtPlugin>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QIcon>
#include <QFileInfo>
#include <QSettings>

class Component;
class IPluginUtility;
class PluginSettingsWidget;
class LibraryComponent;

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

	/*!
	 *  The destructor.
	*/
	~PadsPartGenerator();

    //! Returns the name of the plugin.
    virtual QString getName() const;

	//! Returns the version of the plugin.
    virtual QString getVersion() const;

	//! Returns the description of the plugin.
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

	//! Returns the icon for the generator.
    /*!
     *  Returns the settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

    /*!
     *  Returns the icon for the generator.
     */
    virtual QIcon getIcon() const;

    /*! Checks whether the generator supports generation for the given library component.
     *
     * @param libComp       The library component for which to check support.
     * @param libDesConf    The design configuration object if the generator is run on a hierarchical component.
     * @param libDes        The design object if the generator is run on a hierarchical component.
     *
     * @return True, if the generator supports the given component. Otherwise false.
    */
	 virtual bool checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp,
		 QSharedPointer<LibraryComponent const> libDesConf = QSharedPointer<LibraryComponent const>(),
		 QSharedPointer<LibraryComponent const> libDes = QSharedPointer<LibraryComponent const>()) const;

    /*! Runs the generator.
     *
     * @param utility       The plugin utility interface.
     * @param libComp       The component for which the generator is run.
     * @param libDesConf    The design configuration object if the generator is run on a hierarchical component.
     * @param libDes        The design object if the generator is run on a hierarchical component.
     *
    */
	 virtual void runGenerator(IPluginUtility* utility, 
		 QSharedPointer<LibraryComponent> libComp, 
		 QSharedPointer<LibraryComponent> libDesConf = QSharedPointer<LibraryComponent>(), 
		 QSharedPointer<LibraryComponent> libDes = QSharedPointer<LibraryComponent>());

	 //! Returns the external program requirements of the plugin.
	 virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

private:
	// Disable copying.
	PadsPartGenerator(PadsPartGenerator const& rhs);

	// Disable assignment.
	PadsPartGenerator& operator=(PadsPartGenerator& rhs);
   
    /*!
     *  Writes the PADS part file.
     *
     *      @param [in] file            The file to write to.
     *      @param [in] partDescription The part description to write.
     *
     *      @return True, if part file was successfully written, otherwise false.
     */
    bool generatePartFile(QFile* file, const QString& partDescription);

    /*!
     *  Writes the PADS CAE decal file.
     *
     *      @param [in] file            The file to write to.
     *      @param [in] partTitle       The part title displayed on all gates.
     *      @param [in] partDescription The PADS part description.
     *
     *      @return True, if CAE file was successfully written, otherwise false.
     */
    bool generateCAEFile(QFile* file, const QString& partTitle, const QString& partDescription);

    /*!
     *  Generates the CAE decal terminals and prints them into an output stream.
     *
     *      @param [in] output      The output stream for generation.
     *      @param [in] drawSymbol  The rectangle used as a drawing symbol for decal.
     *      @param [in] pinCount    The number of terminals in the decal.
     */
    void insertCAETerminals( QTextStream& output, QRect const& drawSymbol, int pinCount );  

    /*!
     *  Generates the CAE decal and prints it into an output stream.
     *
     *      @param [in] output          The output stream for generation.
     *      @param [in] drawSymbol      The rectangle used as a drawing symbol for decal.
     *      @param [in] gateTitle       The title of the gate.
     *      @param [in] gateName        The name of the gate used as a decal name.      
     */
    void insertCAEDecal(QTextStream& output, QRect const& drawSymbol,
        QString const& gateTitle, QString const& gateName); 
    

    /*!
     *  Generates the CAE header lines and prints them into an output stream.
     *
     *      @param [in] output      The output stream for generation.
     *      @param [in] timestamp   Header line with timestamp to insert.
     *      @param [in] gateName    The name of the gate used as a decal name.
     *      @param [in] pinCount    The number of terminals in the decal.
     */
    void insertCAEHeader( QTextStream& output, QString const& timestamp, QString const& gateName, int pinCount );
   

    // void generateCAETerminals( QTextStream& output, gateGenerationType generationType );

    /*!
     *  Adds a file to a file set with defined file types.
     *
     *      @param [in] fileSet         The file set.
     *      @param [in] filePath        The path to file which is added.
     *      @param [in] basePath        The base path of the component xlm file.
     *      @param [in] fileTypes       The filetypes of the added file.
     *
     *      @return True, if the file was successfully added to file set, otherwise false.
     */
    bool addFileToFileset(QSharedPointer<FileSet> fileSet, const QString& filePath, QString const& basePath,
        const QStringList& fileTypes);

};
#endif // PADSPARTGENERATOR_H
