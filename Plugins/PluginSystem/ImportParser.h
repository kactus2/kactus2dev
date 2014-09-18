//-----------------------------------------------------------------------------
// File: ImportParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.09.2014
//
// Description:
// Interface for import parsers.
//-----------------------------------------------------------------------------

#ifndef IMPORTPARSER_H
#define IMPORTPARSER_H

#include <QSharedPointer>

class Component;

class ImportParser
{
public:
    
    virtual ~ImportParser() {};

    /*!
     *  <Method description>.
     *
     *      @param [in] input               The input text to parse.
     *      @param [in] targetComponent     The component to apply all imported changes to.
     */
    virtual void runParser(QString const& input, QSharedPointer<Component> targetComponent) = 0;
};

#endif // IMPORTPARSER_H
