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

#include "IPlugin.h"

#include <QSharedPointer>
#include <QtPlugin>

class Component;

//-----------------------------------------------------------------------------
//! Interface for import parser plugins.
//-----------------------------------------------------------------------------
class ImportParser : public IPlugin
{
public:
    
    //! The destructor.
    virtual ~ImportParser() {};

    /*!
     *  Returns the supported import file types.
     *
     *      @return The file types the import parser supports.
     */
    virtual QStringList acceptedFileTypes() const = 0;

    /*!
     *  Runs the import by parsing the given input and applying the parsed elements to the given component.
     *
     *      @param [in] input               The input text to parse.
     *      @param [in] targetComponent     The component to apply all imported changes to.
     */
    virtual void runParser(QString const& input, QSharedPointer<Component> targetComponent) = 0;
};

Q_DECLARE_INTERFACE(ImportParser, "com.tut.Kactus2.ImportParser/1.0")

#endif // IMPORTPARSER_H
