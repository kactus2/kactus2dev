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

#include <models/component.h>

#include <QString>
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
     *  Checks whether the plugin supports analysis for the given file type.
     *
     *      @param [in] fileType The file type to check.
     *
     *      @return True, if the plugin supports the file type; false if it doesn't.
     */
    virtual bool checkFileTypeSupport(QString const& fileType) = 0;

    /*!
     *  Calculates a language-dependent hash for the given file.
     *
     *      @param [in] utility   The plugin utility interface.
     *      @param [in] filename  The name of the file.
     *
     *      @return The hash value for the file.
     *
     *      @remarks Comments and whitespace are ignored and do not affect the hash value.
     */
    virtual QString calculateHash(IPluginUtility* utility, QString const& filename) = 0;

    /*!
     *  Retrieves all file dependencies the given file has.
     *
     *      @param [in]  utility       The plugin utility interface.
     *      @param [in]  component     The component to which the dependency scan is being run.
     *      @param [in]  filename      The name of the file to which the analysis is run.
     *      @param [out] dependencies  The list of found dependencies.
     */
    virtual void getFileDependencies(IPluginUtility* utility,
                                     QSharedPointer<Component const> component,
                                     QString const& filename,
                                     QList<FileDependencyDesc>& dependencies) = 0;
};

//-----------------------------------------------------------------------------

Q_DECLARE_INTERFACE(ISourceAnalyzerPlugin, "com.tut.Kactus2.ISourceAnalyzerPlugin/1.0")

#endif // ISOURCEANALYZER_H
