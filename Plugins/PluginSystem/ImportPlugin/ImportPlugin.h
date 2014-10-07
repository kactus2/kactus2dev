//-----------------------------------------------------------------------------
// File: ImportParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.09.2014
//
// Description:
// Interface for import parser plugins.
//-----------------------------------------------------------------------------

#ifndef IMPORTPARSER_H
#define IMPORTPARSER_H

#include <Plugins/PluginSystem/IPlugin.h>

#include <QSharedPointer>
#include <QtPlugin>

class Component;

//-----------------------------------------------------------------------------
//! Interface for import parser plugins.
//-----------------------------------------------------------------------------
class ImportPlugin : public IPlugin
{
public:
    
    //! The destructor.
    virtual ~ImportPlugin() {};

    /*!
     *  Returns the supported import file types.
     *
     *      @return The file types the import plugin supports.
     */
    virtual QStringList getSupportedFileTypes() const = 0;

    /*!
     *  Runs the import by parsing the given input and applying the parsed elements to the given component.
     *
     *      @param [in] input               The input text to parse.
     *      @param [in] targetComponent     The component to apply all imported changes to.
     */
    virtual void import(QString const& input, QSharedPointer<Component> targetComponent) = 0;
};

Q_DECLARE_INTERFACE(ImportPlugin, "com.tut.Kactus2.ImportPlugin/1.0")

#endif // IMPORTPARSER_H
