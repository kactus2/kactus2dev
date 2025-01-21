//-----------------------------------------------------------------------------
// File: NewComDefinitionPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 17.4.2012
//
// Description:
// New COM definition page.
//-----------------------------------------------------------------------------

#ifndef NEWCOMDEFINITIONPAGE_H
#define NEWCOMDEFINITIONPAGE_H

#include "NewPage.h"

#include <common/widgets/vlnvEditor/vlnveditor.h>

class LibraryInterface;
//-----------------------------------------------------------------------------
//! NewComDefinitionPage class.
//-----------------------------------------------------------------------------
class NewComDefinitionPage : public NewPage
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *    @param [in] libInterface The library interface.
     *    @param [in] parentDlg    The parent dialog.
     */
    NewComDefinitionPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    virtual ~NewComDefinitionPage() = default;

    // Disable copying.
    NewComDefinitionPage(NewComDefinitionPage const& rhs) = delete;
    NewComDefinitionPage& operator=(NewComDefinitionPage const& rhs) = delete;

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
    //! Signaled when a COM definition should be created.
    void createComDefinition(VLNV const& vlnv, Document::Revision revision, QString const& directory);

};

//-----------------------------------------------------------------------------

#endif // NEWCOMDEFINITIONPAGE_H
