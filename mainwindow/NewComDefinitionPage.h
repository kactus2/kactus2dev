//-----------------------------------------------------------------------------
// File: NewComDefinitionPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.4.2012
//
// Description:
// New COM definition page.
//-----------------------------------------------------------------------------

#ifndef NEWCOMDEFINITIONPAGE_H
#define NEWCOMDEFINITIONPAGE_H

#include <mainwindow/NewPage.h>
#include <common/dialogs/propertyPageDialog/PropertyPageView.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QPushButton>

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
     *      @param [in] libInterface The library interface.
     *      @param [in] parentDlg    The parent dialog.
     */
    NewComDefinitionPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    ~NewComDefinitionPage();

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
    //! Signaled when a COM definition should be created.
    void createComDefinition(VLNV const& vlnv, QString const& directory);

private:
    // Disable copying.
    NewComDefinitionPage(NewComDefinitionPage const& rhs);
    NewComDefinitionPage& operator=(NewComDefinitionPage const& rhs);
    
};

//-----------------------------------------------------------------------------

#endif // NEWCOMDEFINITIONPAGE_H
