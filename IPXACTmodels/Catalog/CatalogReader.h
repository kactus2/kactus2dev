//-----------------------------------------------------------------------------
// File: CatalogReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.02.2017
//
// Description:
// XML reader for IP-XACT Catalog element.
//-----------------------------------------------------------------------------

#ifndef CATALOGREADER_H
#define CATALOGREADER_H

#include <QDomNode>
#include <QSharedPointer>

#include <IPXACTmodels/common/DocumentReader.h>
#include <IPXACTmodels/ipxactmodels_global.h>

class Catalog;
class IpxactFile;

//-----------------------------------------------------------------------------
//! XML reader for IP-XACT Catalog element.
//-----------------------------------------------------------------------------
namespace CatalogReader
{

    /*!
     *  Creates a catalog from XML description.
     *
     *      @param [in] document   The XML document to create the catalog from.
     *
     *      @return The created catalog.
     */
    IPXACTMODELS_EXPORT QSharedPointer<Catalog> createCatalogFrom(QDomNode const& document);
 
    namespace Details
    {

        /*!
         *  Reads the catalog file descriptions to the catalog.
         *
         *      @param [in]     catalogNode     The XML description of the catalog.
         *      @param [in/out] catalog         The catalog to insert the file descriptions into.
         */
        void parseCatalogs(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog);

        /*!
         *  Reads the bus definition file descriptions to the catalog.
         *
         *      @param [in]     catalogNode     The XML description of the catalog.
         *      @param [in/out] catalog         The catalog to insert the file descriptions into.
         */
        void parseBusDefinitions(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog);

        /*!
         *  Reads the abstraction definition file descriptions to the catalog.
         *
         *      @param [in]     catalogNode     The XML description of the catalog.
         *      @param [in/out] catalog         The catalog to insert the file descriptions into.
         */
        void parseAbstractionDefinitions(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog);

        /*!
         *  Reads the component file descriptions to the catalog.
         *
         *      @param [in]     catalogNode     The XML description of the catalog.
         *      @param [in/out] catalog         The catalog to insert the file descriptions into.
         */
        void parseComponents(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog);

        /*!
         *  Reads the abstractor file descriptions to the catalog.
         *
         *      @param [in]     catalogNode     The XML description of the catalog.
         *      @param [in/out] catalog         The catalog to insert the file descriptions into.
         */
        void parseAbstractors(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog);

        /*!
         *  Reads the design file descriptions to the catalog.
         *
         *      @param [in]     catalogNode     The XML description of the catalog.
         *      @param [in/out] catalog         The catalog to insert the file descriptions into.
         */
        void parseDesigns(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog);

        /*!
         *  Reads the design configuration file descriptions to the catalog.
         *
         *      @param [in]     catalogNode     The XML description of the catalog.
         *      @param [in/out] catalog         The catalog to insert the file descriptions into.
         */
        void parseDesignCongfigurations(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog);

        /*!
         *  Reads the generator chain file descriptions to the catalog.
         *
         *      @param [in]     catalogNode     The XML description of the catalog.
         *      @param [in/out] catalog         The catalog to insert the file descriptions into.
         */
        void parseGeneratorChains(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog);

        /*!
         *	Reads the type definition file descriptions to the catalog.
         *
         *      @param [in] catalogNode     The XML description of the catalog.
         *      @param [in/out] catalog	        The catalog to insert the file description into.
         */
        void parseTypeDefinitions(QDomNode const& catalogNode, QSharedPointer<Catalog> catalog);

        /*!
         *  Reads XML node containing ipxactFile elements and creates a list of the parsed elements.
         *
         *      @param [in] filesNode  The XML description to read.
         *      @param [in] documentType The type of IP-XACT document.
         *
         *      @return List of the parsed ipxactFiles.
         */
        QList<QSharedPointer<IpxactFile> > parseIpxactFileList(QDomElement& filesNode, VLNV::IPXactType documentType);

        /*!
         *  Parses an XML node to ipxactFile element.
         *
         *      @param [in] fileNode     The XML description to parse..
         *      @param [in] documentType The type of IP-XACT document.
         *
         *      @return The parsed ipxactFile.
         */
        QSharedPointer<IpxactFile> createIpxactFileFrom(QDomNode const& fileNode, VLNV::IPXactType documentType);
    }
};

#endif // CATALOGREADER_H