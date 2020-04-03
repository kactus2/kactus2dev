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
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Component.h>

#include <QDir>
#include <QRegularExpression>

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
	virtual ~VerilogSourceAnalyzer() = default;

	// Disable copying.
	VerilogSourceAnalyzer(VerilogSourceAnalyzer const& rhs) = delete;
	VerilogSourceAnalyzer& operator=(VerilogSourceAnalyzer const& rhs) = delete;

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
    virtual QWidget* getSettingsWidget();

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel(){return NULL;}

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
	 *  Finds the items (modules, include files) already available in the file sets.
	 *
	 *      @param [in] component       The component whose file sets to search.
	 *      @param [in] componentPath   The path to the component xml file.
	 *
	 *      @return Found items in filesets where key is the item identifier and value the path to the file.
	 */
	QMap<QString, QString> findItemsInFilesets(Component const* component, QString const& componentPath);

	/*!
	 *  Check if the given file is of supported file type.
	 *
	 *      @param [in] file        The file to check.
	 *
	 *      @return True, if the file is of supported type, otherwise false.
	 */
	bool isOfSupportedFileType(QSharedPointer<File> file);

	/*!
    *  Finds the dependencies of a file from include directives.
    *
    *      @param [in] fileContent         The file content to analyze.
	*      @param [in] sourceFileInfo      The source file (being analyzed) information.
	*      @param [in] itemsInFilesets     The available items in file sets.
    *
    *      @return The file dependencies for includes.
    */
	QList<FileDependencyDesc> findIncludeDependencies(QString const& fileContent,
		QFileInfo const& sourceFileInfo, 
		QMap<QString, QString> const& itemsInFilesets) const;

	/*!
	*  Finds the dependencies of a file with given pattern.
	*
	*      @param [in] fileContent       The file content to analyze.
	*      @param [in] matchPattern      The pattern for matching dependent items.
	*
	*      @return The identifiers for items found with the given pattern.
	*/
	QStringList findDependencies(QString const& fileContent, QRegularExpression const& matchPattern) const;

	/*!
	 *  Finds the dependencies of a file from module instantiations.
	 *
	 *      @param [in] fileContent         The file content to analyze.
	 *      @param [in] sourceFileInfo      The source file (being analyzed) information.
	 *      @param [in] itemsInFilesets     The available items in file sets.
	 *
	 *      @return The file dependencies for instantiations.
 	*/
	QList<FileDependencyDesc> findInstantiationDependencies(QString const& fileContent,
		QFileInfo const& sourceFileInfo,
		QMap<QString, QString> const& itemsInFilesets) const;

};

#endif // VERILOGSOURCEANALYZER_H
