//-----------------------------------------------------------------------------
// File: IndirectInterfaceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.08.2017
//
// Description:
// XML writer class for IP-XACT Indirect Interface element.
//-----------------------------------------------------------------------------

#ifndef INDIRECTINTERFACEWriter_H
#define INDIRECTINTERFACEWriter_H

#include <QSharedPointer>
#include <QDomNode>

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/ipxactmodels_global.h>

class IndirectInterface;

//-----------------------------------------------------------------------------
//! XML writer class for IP-XACT Indirect Interface element.
//-----------------------------------------------------------------------------
namespace  IndirectInterfaceWriter
{
    /*!
     *  Write the name group of the indirect interface.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] indirectInterface   The indirect interface being written.
     *      @param [in] docRevision         The applied IP-XACT standard revision.
     */
    IPXACTMODELS_EXPORT void writeIndirectInterface(QXmlStreamWriter& writer, QSharedPointer<IndirectInterface> indirectInterface,
        Document::Revision docRevision) ;

    namespace Details
    {
        /*!
         *  Write the indirect address and data fields of the indirect interface.
         *
         *      @param [in] writer              The used XML writer.
         *      @param [in] indirectInterface   The indirect interface being written.
         */
        void writeIndirectFields(QXmlStreamWriter& writer, QSharedPointer<IndirectInterface> indirectInterface);

        /*!
         *  Write the transparent bridges of the indirect interface.
         *
         *      @param [in] writer              The used XML writer.
         *      @param [in] indirectInterface   The indirect interface being written.
         *      @param [in] docRevision         The applied IP-XACT standard revision.
         */
        void writeTransparentBridges(QXmlStreamWriter& writer,
            QSharedPointer<IndirectInterface> indirectInterface, Document::Revision docRevision);
    }
};

#endif // INDIRECTINTERFACEWriter_H
