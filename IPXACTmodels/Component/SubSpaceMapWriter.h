//-----------------------------------------------------------------------------
// File: SubSpaceMapWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.01.2022
//
// Description:
// Writer for ipxact:subSpaceMap element.
//-----------------------------------------------------------------------------

#ifndef SUBSPACEMAPWRITER_H
#define SUBSPACEMAPWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/MemoryBlockBaseWriter.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

class SubSpaceMap;

//-----------------------------------------------------------------------------
//! Writer for ipxact:subSpaceMap element.
//-----------------------------------------------------------------------------
namespace SubSpaceMapWriter
{
    /*!
     *  Write a subspace map to an XML file.
     *
     *    @param [in] writer          The used XML writer.
     *    @param [in] subMap          The subspace map to be written.
     *    @param [in] docRevision     The IP-XACT standard revision to comply to.
     */
    IPXACTMODELS_EXPORT void writeSubSpaceMap(QXmlStreamWriter& writer, QSharedPointer<SubSpaceMap> subMap, 
        Document::Revision docRevision);

    namespace Details
    {
        /*!
         *  Writes the attributes of a given subspace map into xml.
         *
         *    @param [in] writer          The xml writer to use.
         *    @param [in] subMap          The subspace map whose attributes to write.
         *    @param [in] docRevision     The IP-XACT standard revision to comply to.
         */
        void writeAttributes(QXmlStreamWriter& writer, QSharedPointer<SubSpaceMap> subMap, Document::Revision docRevision);
    }
};

#endif // SUBSPACEMAPWRITER_H