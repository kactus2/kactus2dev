//-----------------------------------------------------------------------------
// File: VHDLSourceAnalyzer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Honkonen, Joni-Matti M‰‰tt‰
// Date: 18.1.2013
//
// Description:
// VHDL source analyzer plugin.
//-----------------------------------------------------------------------------

#ifndef VHDLSOURCEANALYZER_H
#define VHDLSOURCEANALYZER_H

#include <Plugins/PluginSystem/ISourceAnalyzerPlugin.h>

#include <QFile>

class IPluginUtility;

//-----------------------------------------------------------------------------
//! VHDLSourceAnalyzer
//-----------------------------------------------------------------------------
class VHDLSourceAnalyzer : public QObject, public ISourceAnalyzerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.VHDLSourceAnalyzer" FILE "vhdlPlugin.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(ISourceAnalyzerPlugin)

public:
    //! The constructor.
	VHDLSourceAnalyzer();

	//! The destructor.
    virtual ~VHDLSourceAnalyzer() = default;

	// Disable copying.
	VHDLSourceAnalyzer(VHDLSourceAnalyzer const& rhs) = delete;
	VHDLSourceAnalyzer& operator=(VHDLSourceAnalyzer const& rhs) = delete;

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
    virtual QWidget* getSettingsWidget();

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel(){return nullptr;}

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

    //! Returns the external program requirements of the plugin.
	virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

private:

    /*!
     *  Reads source file data from given file.
     *
     *      @param [in] filename    The file that is read.
     *
     *      @return The meaningful source data of the file, with comments and empty lines removed.
     */
    QString getSourceData(QString const& filename);

    /*!
     *
     * Scans all the files in the component to find VHDL entities and packages.
     *
     *      @param [in] component       The component from which files are scanned.
     *      @param [in] componentPath   The path of of the component in the file system.
     */
    void scanDefinitions(Component const* component, QString const& componentPath);

    /*!
     *  Scans entity declarations in the given source string.
     *
     *      @param [in] source    The source string.
     *      @param [in] filename  The name of the correspondent file.
     */
    void scanEntities(QString const& source, QString const& filename);

    /*!
     *  Scans package declarations in the given source string.
     *
     *      @param [in] source    The source string.
     *      @param [in] filename  The name of the correspondent file.
     */
    void scanPackages(QString const& source, QString const& filename);

    /*!
     *  Scans entity references in the source string.
     *
     *      @param [in]  source        The source string.
     *      @param [in]  filename      The name of the corresponding file.
     *      @param [out] dependencies  The list of dependencies.
     */
    void scanEntityReferences(QString const& source, QString const& filename,
                              QList<FileDependencyDesc>& dependencies);


    /*!
     *  Scans package references in the source string.
     *
     *      @param [in]  source        The source string.
     *      @param [in]  filename      The name of the corresponding file.
     *      @param [out] dependencies  The list of dependencies.
     */
    void scanPackageReferences(QString const& source, QString const& filename,
                               QList<FileDependencyDesc>& dependencies);

    /*!
     *  Adds a new entity dependency to the list of dependencies.
     *
     *      @param [in] componentName  The name of the instantiated component.
     *      @param [in] filename       The name of the file containing the instantiation.
     *      @param [out] dependencies  The list of dependencies.
     */
    void addEntityDependency(QString const& componentName, QString const& filename,
                             QList<FileDependencyDesc> &dependencies);

    /*!
     *  Adds a new package dependency to the list of dependencies.
     *
     *      @param [in] componentName  The name of the package.
     *      @param [in] filename       The name of the file containing the dependency.
     *      @param [out] dependencies  The list of dependencies.
     */
    void addPackageDependency(QString const& packageName, QString const& filename,
                              QList<FileDependencyDesc> &dependencies);
	/*!
	 *  Adds a new dependency to the list of dependencies, if it is unique i.e. not in the list already.
	 *
	 *      @param [in] dependency		The dependency to add.
	 *      @param [out] dependencies	The list of dependencies.
	 */
	void addUniqueDependency(FileDependencyDesc const& dependency, QList<FileDependencyDesc>& dependencies);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The supported file types.
    QStringList fileTypes_ = QStringList("vhdlSource");

    //! The container used for caching scanned entities.
    QMap<QString, QStringList> cachedEntities_;

    //! The container used for caching scanned packages.
    QMap<QString, QStringList> cachedPackages_;
};

#endif // VHDLSOURCEANALYZER_H
