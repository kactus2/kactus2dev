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

#include <IPXACTmodels/component.h>

#include <QString>
#include <QStringList>
#include <QSharedPointer>
#include <QList>
#include <QtPlugin>

class FileDependency;

//-----------------------------------------------------------------------------
//! File dependency description structure.
//-----------------------------------------------------------------------------
struct FileDependencyDesc
{
    QString filename;       //!< The name of the dependent file.
    QString description;    //!< Possible automatically deduced description for the dependency.

    /*!
     *  Default constructor.
     */
    FileDependencyDesc() : filename(), description()
    {
    }
};

//-----------------------------------------------------------------------------
//! Source analyzer plugin interface for dependency analysis.
//-----------------------------------------------------------------------------
class ISourceAnalyzerPlugin : public IPlugin
{
public:
    /*!
     *  Destructor.
     */
    virtual ~ISourceAnalyzerPlugin() {}

    /*!
     *  Returns the list of file types this plugin can run analysis for.
     */
    virtual QStringList const& getSupportedFileTypes() const = 0;

    /*!
     *  Calculates a language-dependent hash for the given file.
     *
     *      @param [in] filename  The name of the file.
     *
     *      @return The hash value for the file.
     *
     *      @remarks Comments and whitespace are ignored and do not affect the hash value.
     */
    virtual QString calculateHash(QString const& filename) = 0;

    /*!
     *  Begins the analysis for the given component.
     *
     *      @param [in] component      The component.
     *      @param [in] componentPath  The path to the directory where the component is located.
     *
     *      @remarks Any preparations needed for the file dependency analysis should be made here.
     */
    virtual void beginAnalysis(Component const* component, QString const& componentPath) = 0;

    /*!
     *  Ends the analysis for the given component.
     *
     *      @param [in] component      The component.
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
     *      @param [out] dependencies   The list of found dependencies.
     */
    virtual void getFileDependencies(Component const* component,
                                     QString const& componentPath,
                                     QString const& filename,
                                     QList<FileDependencyDesc>& dependencies) = 0;
};

//-----------------------------------------------------------------------------

Q_DECLARE_INTERFACE(ISourceAnalyzerPlugin, "com.tut.Kactus2.ISourceAnalyzerPlugin/1.0")

#endif // ISOURCEANALYZER_H
