//-----------------------------------------------------------------------------
// File: CppSourceAnalyzer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Honkonen, Joni-Matti M‰‰tt‰
// Date: 18.1.2013
//
// Description:
// C/C++ source analyzer plugin.
//-----------------------------------------------------------------------------

#ifndef CPPSOURCEANALYZER_H
#define CPPSOURCEANALYZER_H

#include <KactusAPI/include/ISourceAnalyzerPlugin.h>

#include <QFile>
#include <QString>

//-----------------------------------------------------------------------------
// C/C++ source analyzer plugin.
//-----------------------------------------------------------------------------
class CppSourceAnalyzer : public QObject, public ISourceAnalyzerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.CppSourceAnalyzer" FILE "CppSourceAnalyzer.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(ISourceAnalyzerPlugin)

public:

    //! The constructor.
    CppSourceAnalyzer();

    // Disable copying.
    CppSourceAnalyzer(CppSourceAnalyzer const& rhs) = delete;
    CppSourceAnalyzer& operator=(CppSourceAnalyzer const& rhs) = delete;

    //! The destructor.
    virtual ~CppSourceAnalyzer() = default;

    /*!
     *  Returns the name of the plugin.
     */
    virtual QString getName() const override final;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString getVersion() const override final;

    /*!
     *  Returns the description of the plugin.
     */
    virtual QString getDescription() const override final;

    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString getVendor() const override final;

    /*!
     *  Returns the licence of the plugin.
     */
    virtual QString getLicence() const override final;

    /*!
     *  Returns the holder of the licence of the plugin.
     */
    virtual QString getLicenceHolder() const override final;

    /*!
     *  Returns the settings widget.
     */
    virtual QWidget* getSettingsWidget() override final;

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel() override final  {return nullptr;}

    /*!
     *  Returns the list of file types this plugin can run analysis for.
     */
    virtual QStringList getSupportedFileTypes() const override final;

    /*!
     *  Calculates a language-dependent hash for the given file.
     *
     *      @param [in] filename  The name of the file.
     *
     *      @return The hash value for the file.
     *
     *      @remarks Comments and whitespace are ignored and do not affect the hash value.
     */
    virtual QString calculateHash(QString const& filename) override final;

    /*!
     *  Begins the analysis for the given component.
     *
     *      @param [in] component       The component to which the dependency scan is being run.
     *      @param [in] componentPath  The path to the directory where the component is located.
     *
     *      @remarks Any preparations needed for the file dependency analysis should be made here.
     */
    virtual void beginAnalysis(Component const* component, QString const& componentPath) override final;

    /*!
     *  Ends the analysis for the given component.
     *
     *      @param [in] component       The component to which the dependency scan is being run.
     *      @param [in] componentPath  The path to the directory where the component is located.
     *
     *      @remarks Any cleanups needed should be made here.
     */
    virtual void endAnalysis(Component const* component, QString const& componentPath) override final;

    /*!
     *  Retrieves all file dependencies the given file has.
     *
     *      @param [in]  component      The component to which the dependency scan is being run.
     *      @param [in]  componentPath  The path to the directory where the component is located.
     *      @param [in]  filename       The name of the file to which the analysis is run.
     *      @param [out] dependencies   The list of found dependencies.
     */
    virtual QList<FileDependencyDesc> getFileDependencies(Component const* component,
                                     QString const& componentPath,
                                     QString const& filename) override final;

	 //! Returns the external program requirements of the plugin.
	 virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements() override final;

private:

    /*!
     *
     * Reads source file data from given file. Used by calulateHash and getFileDependencies.
     *
     *      @param [in] file    The file that is read.
     *
     *      @return The meaningful source data of the file, with comments and empty lines removed.
     */
    QString getSourceData(QFile& file);

    /*!
     *
     * Removes comments from a string that consists of a C/C++ source code
     *
     *      @param [inout] data    The input source code with comments.
     *
     *      @return The meaningful source data of the file, with comments removed.
     */
    QString removeComments(QString const& source);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The supported file types.
    QStringList fileTypes_;
};

#endif // CPPSOURCEANALYZER_H
