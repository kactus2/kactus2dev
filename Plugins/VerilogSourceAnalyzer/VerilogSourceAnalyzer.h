//-----------------------------------------------------------------------------
// File: VerilogSourceAnalyzer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.04.2015
//
// Description:
// Verilog source file analyzer for finding file dependencies.
//-----------------------------------------------------------------------------

#ifndef VERILOGSOURCEANALYZER_H
#define VERILOGSOURCEANALYZER_H

#include "verilogsourceanalyzer_global.h"

#include <Plugins/PluginSystem/ISourceAnalyzerPlugin.h>
#include <QObject>

class Component;
class FileSet;

//-----------------------------------------------------------------------------
//! Verilog source file analyzer for finding file dependencies.
//-----------------------------------------------------------------------------
class VERILOGSOURCEANALYZER_EXPORT VerilogSourceAnalyzer : public QObject, public ISourceAnalyzerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.VerilogSourceAnalyzer" FILE "VerilogSourceAnalyzer.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(ISourceAnalyzerPlugin)

public:

	//! The constructor.
	VerilogSourceAnalyzer();

	//! The destructor.
	~VerilogSourceAnalyzer();

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
     *  Returns the licence holder of the plugin.
     */
    virtual QString getLicenceHolder() const;

    /*!
     *  Returns the settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

	//! Returns the external program requirements of the plugin.
	 virtual QList<ExternalProgramRequirement> getProgramRequirements();

    /*!
     *  Returns the list of file types this plugin can run analysis for.
     */
    virtual QStringList getSupportedFileTypes() const;
    
    /*!
     *  Calculates a language-dependent hash for the given file.
     *
     *      @param [in] filename  The name of the file.
     *
     *      @return The hash value for the file.
     *
     *      @remarks Comments and whitespace are ignored and do not affect the hash value.
     */
    virtual QString calculateHash(QString const& filename);
     
    /*!
     *  Begins the analysis for the given component.
     *
     *      @param [in] component      The component.
     *      @param [in] componentPath  The path to the directory where the component is located.
     *
     *      @remarks Any preparations needed for the file dependency analysis should be made here.
     */
    virtual void beginAnalysis(Component const* component, QString const& componentPath);
    
    /*!
     *  Ends the analysis for the given component.
     *
     *      @param [in] component      The component.
     *      @param [in] componentPath  The path to the directory where the component is located.
     *
     *      @remarks Any cleanups needed should be made here.
     */
    virtual void endAnalysis(Component const* component, QString const& componentPath);
    
    /*!
     *  Retrieves all file dependencies the given file has.
     *
     *      @param [in]  component      The component to which the dependency scan is being run.
     *      @param [in]  componentPath  The path to the directory where the component is located.
     *      @param [in]  filename       The name of the file to which the analysis is run.
     *
     *      @return The list of found dependencies.
     */
    virtual QList<FileDependencyDesc> getFileDependencies(Component const* component, 
        QString const& componentPath, QString const& filename);

private:

	// Disable copying.
	VerilogSourceAnalyzer(VerilogSourceAnalyzer const& rhs);
	VerilogSourceAnalyzer& operator=(VerilogSourceAnalyzer const& rhs);

    /*!
     *  Reads the given file and removes comments and extra whitespace in it.
     *
     *      @param [in] filePath   The file to read.
     *
     *      @return The file content without comments and extra whitespace.
     */
    QString readFileContentAndRemoveComments(QString const& filePath) const;

    /*!
     *  Finds the absolute path for a given file.
     *
     *      @param [in] filename        The file whose absolute path to find.
     *      @param [in] componentPath   The path to the containing component xml file.
     *
     *      @return Absolute path to filename.
     */
    QString findAbsolutePathFor(QString const& filename, QString const& componentPath) const;

    /*!
     *  Finds the dependencies of a file from include directives.
     *
     *      @param [in] fileContent         The file content.
     *      @param [in] fileAbsolutePath    The absolute path to the file.
     *      @param [in] xmlPath             The path to the containing component xml file.
     *      @param [in] fileSets            The filesets of the containing component.
     *
     *      @return The file dependencies for includes.
     */
    QList<FileDependencyDesc> getIncludeDependencies(QString const& fileContent, 
        QString const& fileAbsolutePath, QString const& xmlPath, 
        QList<QSharedPointer<FileSet> > const& fileSets) const;

    /*!
     *  Finds the absolute path for a given file in the file sets.
     *
     *      @param [in] fileName    The name of the file to find.
     *      @param [in] xmlPath     The path to the containing component xml file.
     *      @param [in] fileSets    The filesets of the containing component.
     *
     *      @return Absolute path to the file if found in the file sets, otherwise returns fileName.
     */
    QString findAbsolutePathInFileSets(QString fileName, QString const& xmlPath, 
        QList<QSharedPointer<FileSet> > const& fileSets) const;

};

#endif // VERILOGSOURCEANALYZER_H
