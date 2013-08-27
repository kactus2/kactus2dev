//-----------------------------------------------------------------------------
// File: NewApiDefinitionPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.4.2012
//
// Description:
// New API definition page.
//-----------------------------------------------------------------------------

#ifndef NEWAPIDEFINITIONPAGE_H
#define NEWAPIDEFINITIONPAGE_H

#include "NewPage.h"
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QPushButton>

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
     *      @param [in] libInterface The library interface.
     *      @param [in] parentDlg    The parent dialog.
     */
    NewApiDefinitionPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    ~NewApiDefinitionPage();

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    bool validate();

    /*!
     *  Applies the changes that were done in the page.
     */
    void apply();


signals:
    //! Signaled when a API definition should be created.
    void createApiDefinition(VLNV const& vlnv, QString const& directory);

private:
    // Disable copying.
    NewApiDefinitionPage(NewApiDefinitionPage const& rhs);
    NewApiDefinitionPage& operator=(NewApiDefinitionPage const& rhs);
       
};

//-----------------------------------------------------------------------------

#endif // NEWAPIDEFINITIONPAGE_H
