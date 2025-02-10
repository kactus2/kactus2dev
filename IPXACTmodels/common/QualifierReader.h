//-----------------------------------------------------------------------------
// File: QualifierReader.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 07.11.2023
//
// Description:
// XML parsing for ipxact:qualifier.
//-----------------------------------------------------------------------------

#ifndef QUALIFIER_READER_H
#define QUALIFIER_READER_H

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/Qualifier.h>

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! XML parsing for ipxact:qualifier.
//-----------------------------------------------------------------------------
namespace QualifierReader
{

    /*!
     *  Parses a qualifier.
     *
     *      @param [in] qualifierNode       XML description of the IP-XACT element.
     *      @param [in] qualifier           The qualifier to read to.
     *      @param [in] revision            The standard revision of the XML document being read.

     */
    void parseQualifier(QDomNode const& qualifierNode, QSharedPointer<Qualifier> qualifier,
        Document::Revision revision);

};

#endif // DOCUMENTREADER_H