//-----------------------------------------------------------------------------
// File: CatalogValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.02.2017
//
// Description:
// Validator for ipxact:catalog.
//-----------------------------------------------------------------------------

#ifndef CATALOGVALIDATOR_H
#define CATALOGVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QVector>
#include <QList>
#include <QSharedPointer>

class Catalog;
class IpxactFile;

//-----------------------------------------------------------------------------
//! Validator for ipxact:catalog.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT CatalogValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    CatalogValidator();

	//! The destructor.
	virtual ~CatalogValidator();

    /*!
     *  Validates the given Catalog.
     *
     *    @param [in] Catalog           The Catalog to validate.
     *
     *    @return True, if the catalog is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<Catalog> catalog) const;

    /*!
     *  Finds possible errors in a catalog and creates a list of them.
     *
     *    @param [in] errors      List of found errors.
     *    @param [in] Catalog     The catalog whose errors to find.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<Catalog> catalog) const;

private:

    // Disable copying.
    CatalogValidator(CatalogValidator const& rhs);
    CatalogValidator& operator=(CatalogValidator const& rhs);

    /*!
     *  Validates the given set of ipxactFiles.
     *
     *    @param [in] files   The files to validate.
     *
     *    @return True, if all files are valid, otherwise false.
     */
    bool validateFiles(QSharedPointer<QList<QSharedPointer<IpxactFile> > > files) const;
    
    /*!
     *  Validates the given ipxactFile.
     *
     *    @param [in] files   The file to validate.
     *
     *    @return True, if the file is valid, otherwise false.
     */
    bool validateFile(QSharedPointer<IpxactFile> file) const;

    /*!
     *  Finds possible errors in the given set of files and creates a list of them.
     *
     *    @param [in/out] errors  List of found errors.
     *    @param [in] files       The files whose errors to find.
     *    @param [in] context     Context for the files to  help locate errors.
     */
    void findErrorsInFiles(QVector<QString>& errors, 
        QSharedPointer<QList<QSharedPointer<IpxactFile> > > files,
        QString const& context) const;

};

#endif // CATALOGVALIDATOR_H
