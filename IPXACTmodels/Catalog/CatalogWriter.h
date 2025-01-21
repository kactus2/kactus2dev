//-----------------------------------------------------------------------------
// File: CatalogWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.01.2017
//
// Description:
// Writer for IP-XACT Catalog element.
//-----------------------------------------------------------------------------

#ifndef CATALOGWRITER_H
#define CATALOGWRITER_H

#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <IPXACTmodels/common/DocumentWriter.h>

#include <IPXACTmodels/ipxactmodels_global.h>

class Catalog;
class PortAbstraction;
class VLNV;
class IpxactFile;
class WirePort;

//-----------------------------------------------------------------------------
//! Writer for IP-XACT Catalog element.
//-----------------------------------------------------------------------------
namespace CatalogWriter
{
    
    /*!
     *  Writes the given catalog into XML.
     *
     *    @param [in] writer    The XML writer to use.
     *    @param [in] catalog   The catalog to write.
     */
    IPXACTMODELS_EXPORT void writeCatalog(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog);

    namespace Details
    {
        void writeCatalogs(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog);

        void writeBusDefinitions(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog);

        void writeAbstractionDefinitions(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog);

        void writeComponents(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog);

        void writeAbstractors(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog);

        void writeDesigns(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog);

        void writeDesignConfigurations(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog);

        void writeGeneratorChains(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog);

        void writeTypeDefinitions(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog);

        void writeIpxactFileGroup(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<IpxactFile> > > files,
            QString const& elementName);

        void writeIpxactFile(QXmlStreamWriter& writer, QSharedPointer<IpxactFile> ipxactFile);
    }
};    

#endif // CATALOGWRITER_H