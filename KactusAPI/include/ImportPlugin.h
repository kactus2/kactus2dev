//-----------------------------------------------------------------------------
// File: ImportParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.09.2014
//
// Description:
// Import plugins can be used to create elements e.g. ports in components from implementation source files
// e.g. VHDL files. The import can be run in component wizard. After selecting the top-level file, all active
// import plugins, whose accepted file ypes match the selected file, will be run.
//-----------------------------------------------------------------------------

#ifndef IMPORTPARSER_H
#define IMPORTPARSER_H

#include <KactusAPI/include/IPlugin.h>

#include <QSharedPointer>
#include <QtPlugin>

class Component;

class ImportPlugin : public IPlugin
{
public:
    
    /*!
     *  The destructor.
     */
    virtual ~ImportPlugin() = default;

    /*!
     *  Returns the file types supported by the plugin.
     *
     *      @return The file types the import plugin supports.
     */
    virtual QStringList getSupportedFileTypes() const = 0;

    /*!
     *  Returns a compatibility warning concerning the import plugin usage. Can be empty.
     *
     *      @return The warning text.
     */
    virtual QString getCompatibilityWarnings() const = 0;

    /*!
     *  Get component declarations from the selected input file.
     *
     *      @param [in] input   The selected input file.
     *
     *      @return List of component declarations found in the selected input.
     */
    virtual QStringList getFileComponents(QString const& input) const = 0;

    /*!
     *  Get the name of the selected component declaration.
     *
     *      @param [in] componentDeclaration    The selected component declaration.
     *
     *      @return Name of the selected component declaration.
     */
    virtual QString getComponentName(QString const& componentDeclaration) const = 0;

    /*!
     *  Runs the import by parsing the given input and applying the parsed elements to the given component.
     *
     *      @param [in] input                   The input text to parse.
     *      @param [in] componentDeclaration    Declaration of the selected component.
     *      @param [in] targetComponent         The component to apply all imported changes to.
     */
    virtual void import(QString const& input, QString const& componentDeclaration,
        QSharedPointer<Component> targetComponent) = 0;

};

Q_DECLARE_INTERFACE(ImportPlugin, "com.tut.Kactus2.ImportPlugin/1.0")

#endif // IMPORTPARSER_H
