//-----------------------------------------------------------------------------
// File: CatalogWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.01.2017
//
// Description:
// Writer class for IP-XACT Catalog element.
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
//! Writer class for IP-XACT Catalog element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT CatalogWriter : public DocumentWriter
{
public:

	//! The constructor.
	CatalogWriter();

	//! The destructor.
	virtual ~CatalogWriter() = default;
    
    // Disable copying.
    CatalogWriter(CatalogWriter const& rhs) = delete;
    CatalogWriter& operator=(CatalogWriter const& rhs) = delete;

    /*!
     *  Writes the given catalog into XML.
     *
     *      @param [in] writer    The XML writer to use.
     *      @param [in] catalog   The catalog to write.
     */
    void writeCatalog(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const;

private:

    void writeCatalogs(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const;
    
    void writeBusDefinitions(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const;

    void writeAbstractionDefinitions(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const;
   
    void writeComponents(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const;

    void writeAbstractors(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const;

    void writeDesigns(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const;

    void writeDesignConfigurations(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const;

    void writeGeneratorChains(QXmlStreamWriter& writer, QSharedPointer<Catalog> catalog) const;

    void writeIpxactFileGroup(QXmlStreamWriter &writer, QSharedPointer<QList<QSharedPointer<IpxactFile> > > files,
        QString const& elementName) const;

    void writeIpxactFile(QXmlStreamWriter& writer, QSharedPointer<IpxactFile> ipxactFile) const;

};    

#endif // CATALOGWRITER_H