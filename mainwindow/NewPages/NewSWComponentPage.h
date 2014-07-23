//-----------------------------------------------------------------------------
// File: NewSWComponentPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.8.2011
//
// Description:
// New SW component page.
//-----------------------------------------------------------------------------

#ifndef NEWSWCOMPONENTPAGE_H
#define NEWSWCOMPONENTPAGE_H

#include "NewPage.h"
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

class LibraryInterface;
class VLNVEditor;
class KactusAttributeEditor;
class VLNV;

//-----------------------------------------------------------------------------
//! NewSWComponentPage class.
//-----------------------------------------------------------------------------
class NewSWComponentPage : public NewPage
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] libInterface The library interface.
     *      @param [in] parentDlg    The parent dialog.
     */
    NewSWComponentPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    ~NewSWComponentPage();

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
    //! Signaled when a SW component should be created.
    void createSWComponent(VLNV const& vlnv, QString const& directory);

private:
    // Disable copying.
    NewSWComponentPage(NewSWComponentPage const& rhs);
    NewSWComponentPage& operator=(NewSWComponentPage const& rhs);
};

//-----------------------------------------------------------------------------

#endif // NEWSWCOMPONENTPAGE_H
