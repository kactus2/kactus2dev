//-----------------------------------------------------------------------------
// File: NewApiDefinitionPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 17.4.2012
//
// Description:
// New API definition page.
//-----------------------------------------------------------------------------

#ifndef NEWAPIDEFINITIONPAGE_H
#define NEWAPIDEFINITIONPAGE_H

#include "NewPage.h"

class LibraryInterface;

//-----------------------------------------------------------------------------
//! NewApiDefinitionPage class.
//-----------------------------------------------------------------------------
class NewApiDefinitionPage : public NewPage
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *    @param [in] libInterface The library interface.
     *    @param [in] parentDlg    The parent dialog.
     */
    NewApiDefinitionPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    virtual ~NewApiDefinitionPage() = default;

    // Disable copying.
    NewApiDefinitionPage(NewApiDefinitionPage const& rhs) = delete;
    NewApiDefinitionPage& operator=(NewApiDefinitionPage const& rhs) = delete;

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
    void createApiDefinition(VLNV const& vlnv, Document::Revision revision, QString const& directory);

       
};

//-----------------------------------------------------------------------------

#endif // NEWAPIDEFINITIONPAGE_H
