//-----------------------------------------------------------------------------
// File: QualifierWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.11.2023
//
// Description:
// XML writer for ipxact:qualifier element.
//-----------------------------------------------------------------------------

#ifndef QUALIFIER_WRITER_H
#define QUALIFIER_WRITER_H

#include <QSharedPointer>
#include <QXmlStreamWriter>

class Qualifier;

//-----------------------------------------------------------------------------
//! XML writer for ipxact:qualifier element.
//-----------------------------------------------------------------------------
class QualifierWriter
{
public:


    /*!
     *  Writes the given protocol description into XML.
     *
     *    @param [in] writer      The XML writer to use.
     *    @param [in] protocol    The protocol to write.
     */
    static void writeQualifier(QXmlStreamWriter& writer, QSharedPointer<Qualifier> qualifier);

};

#endif // QUALIFIER_WRITER_H