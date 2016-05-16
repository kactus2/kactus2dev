//-----------------------------------------------------------------------------
// File: ISourceAnalyzerPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 14.01.2013
//
// Description:
// Source analyzer plugin interface for dependency analysis.
//-----------------------------------------------------------------------------

#ifndef ISOURCEANALYZER_H
#define ISOURCEANALYZER_H

#include "IPlugin.h"
#include "IPluginUtility.h"

#include <QString>
#include <QStringList>
#include <QSharedPointer>
#include <QList>
#include <QtPlugin>

class Component;
class FileDependency;

//-----------------------------------------------------------------------------
//! File dependency description structure.
//-----------------------------------------------------------------------------
struct FileDependencyDesc
{
	//! The name of the dependent file, for example "componentName.vhd".
    QString filename;
	//! Possible automatically deduced description for the dependency, for example
	//! "Component instantiation for entity componentName".
    QString description;    

    /*!
     *  Default constructor.
     */
    FileDependencyDesc() : filename(), description()
    {
    }
};

//-----------------------------------------------------------------------------
//! Kactus2 contains a dependency analyzer to manage and visualize dependencies between source files of
//! a component. Source analyzer plugins are used in the dependency analyzer to add support for different code
//! languages and even custom file types. Single source analyzer plugin implements support for a single language
//! or predefined file types.
//-----------------------------------------------------------------------------
class ISourceAnalyzerPlugin : public IPlugin
{
public:
    /*!
     *  Destructor.
     */
    virtual ~ISourceAnalyzerPlugin() {}

    /*!
     *  Returns the list of file types this plugin can run analysis for. File type is described as an IP-XACT style string,
	 *  e.g. cppSource or vhdlSource. Kactus2 includes a settings page for setting which file extensions map to which
	 *  file types so that the analyzer plugin doesn't have to hard-code the supported extensions.
     */
    virtual QStringList getSupportedFileTypes() const = 0;

    /*!
     *  Calculates a language-dependent hash for the analyzed file. Hash calculation here may ignore
	 *  whitespace and comments.
     *
     *      @param [in] filename  The name of the file.
     *
     *      @return The hash value for the file.
     */
    virtual QString calculateHash(QString const& filename) = 0;

    /*!
     *  This function is called once when the dependency analysis scan is started. It gives the plugin the
	 *  ability to do preparations before any file is analyzed.
     *
     *      @param [in] component       The component to which the dependency scan is being run.
     *      @param [in] componentPath  The path to the directory where the component is located.
     *
     *      @remarks Any preparations needed for the file dependency analysis should be made here.
     */
    virtual void beginAnalysis(Component const* component, QString const& componentPath) = 0;

    /*!
     *  This function is called once after the dependency analysis scan has completed. It allows the plugin to
	 *  make cleanup operations after the dependency scan has been finished, e.g destroy internal data structures.
     *
     *      @param [in] component       The component to which the dependency scan is being run.
     *      @param [in] componentPath  The path to the directory where the component is located.
     *
     *      @remarks Any cleanups needed should be made here.
     */
    virtual void endAnalysis(Component const* component, QString const& componentPath) = 0;

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
        QString const& componentPath, 
        QString const& filename) = 0;
};

//-----------------------------------------------------------------------------

Q_DECLARE_INTERFACE(ISourceAnalyzerPlugin, "com.tut.Kactus2.ISourceAnalyzerPlugin/1.0")

#endif // ISOURCEANALYZER_H
