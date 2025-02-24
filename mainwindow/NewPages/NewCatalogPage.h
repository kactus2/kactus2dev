//-----------------------------------------------------------------------------
// File: NewCatalogPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.02.2017
//
// Description:
// New Catalog page.
//-----------------------------------------------------------------------------

#ifndef NEWCATALOGPAGE_H
#define NEWCATALOGPAGE_H

#include "NewPage.h"

class LibraryInterface;

//-----------------------------------------------------------------------------
//! NewCatalogPage class.
//-----------------------------------------------------------------------------
class NewCatalogPage : public NewPage
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *    @param [in] libInterface The library interface.
     *    @param [in] parentDlg    The parent dialog.
     */
    NewCatalogPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    virtual ~NewCatalogPage() = default;

    // Disable copying.
    NewCatalogPage(NewCatalogPage const& rhs) = delete;
    NewCatalogPage& operator=(NewCatalogPage const& rhs) = delete;

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *    @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    bool validate() final;

    /*!
     *  Applies the changes that were done in the page.
     */
    void apply() final;


signals:
    //! Signaled when a API definition should be created.
    void createCatalog(VLNV const& vlnv, Document::Revision revision, QString const& directory);

};

//-----------------------------------------------------------------------------

#endif // NEWCATALOGPAGE_H
