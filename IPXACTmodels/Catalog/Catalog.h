//-----------------------------------------------------------------------------
// File: Catalog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.01.2017
//
// Description:
// Implementation for ipxact:catalog element.
//-----------------------------------------------------------------------------

#ifndef CATALOG_H
#define CATALOG_H


#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/ipxactmodels_global.h>
 
#include <QSharedPointer>
#include <QList>

class IpxactFile;

//-----------------------------------------------------------------------------
//! Implementation for ipxact:Catalog element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Catalog: public Document
{
public:

	//! The default constructor.
	Catalog(VLNV const & vlnv = VLNV());

	//! Copy constructor.
	Catalog(Catalog const& other);


	//! Assignment operator.
	Catalog &operator=(Catalog const& other);

	//! The destructor.
	virtual ~Catalog();

	/*!
	 *  Creates a perfect copy of the Bus Definition.
	 *
	 *      @return Copy of the bus definition.
	 */
	virtual QSharedPointer<Document> clone() const;

	/*! Set the VLNV for the bus definition.
	*
	*      @param [in] vlnv     The vlnv to set
	*/
	virtual void setVlnv(const VLNV& vlnv);

    /*!
     *  Get the catalog descriptions in the catalog.
     *
     *      @return The catalogs referenced by this catalog.
     */
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > getCatalogs() const;

    /*!
     *  Get the bus definitions descriptions in the catalog.
     *
     *      @return The bus definitions referenced by this catalog.
     */
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > getBusDefinitions() const;

    /*!
     *  Get the abstractions definitions in the catalog.
     *
     *      @return The abstractions definitions referenced by this catalog.
     */
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > getAbstractionDefinitions() const;

    /*!
     *  Get the component descriptions in the catalog.
     *
     *      @return The components referenced by this catalog.
     */
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > getComponents() const;

    /*!
     *  Get the abstractor descriptions in the catalog.
     *
     *      @return The abstractors referenced by this catalog.
     */
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > getAbstractors() const;
  
    /*!
     *  Get the design descriptions in the catalog.
     *
     *      @return The designs referenced by this catalog.
     */  
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > getDesigns() const;
    
    /*!
     *  Get the design configuration descriptions in the catalog.
     *
     *      @return The design configurations referenced by this catalog.
     */
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > getDesignConfigurations() const;

    /*!
     *  Get the generator chain descriptions in the catalog.
     *
     *      @return The generator chains referenced by this catalog.
     */
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > getGeneratorChains() const;
    
	/*! Get the VLNVs that this catalog depends on.
	 *
	 *      @return The dependency VLNVs.
	 */
	virtual QList<VLNV> getDependentVLNVs() const;

	/*! Get the file dependencies of this catalog.
     *
	 *      @return The dependency files.
     *
     *      @remark This function never returns anything because catalog only has VLNV dependencies.
	 */
	virtual QStringList getDependentFiles() const;

private:

    /*!
     *  Copies file descriptions from another catalog.
     *
     *      @param [in] other   The catalog to copy from.
     */
    void copyIpxactFiles(Catalog const& other);

    //! The catalogs referenced by this catalog.
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > catalogs_;

    //! The bus definitions referenced by this catalog.
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > busDefinitions_;

    //! The abstration definitions referenced by this catalog.
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > abstractionDefinitions_;

    //! The components referenced by this catalog.
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > components_;

    //! The abstractors referenced by this catalog.
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > abstractors_;

    //! The designs referenced by this catalog.
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > designs_;

    //! The design configurations referenced by this catalog.
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > designConfigurations_;

    //! The generator chains referenced by this catalog.
    QSharedPointer<QList<QSharedPointer<IpxactFile> > > generatorChains_;
};

#endif // CATALOG_H
