//-----------------------------------------------------------------------------
// File: NewSWDesignPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 14.11.2011
//
// Description:
// New software design page.
//-----------------------------------------------------------------------------

#ifndef NEWSWDESIGNPAGE_H
#define NEWSWDESIGNPAGE_H

#include "NewPage.h"

class LibraryInterface;
//-----------------------------------------------------------------------------
//! NewSWDesignPage class.
//-----------------------------------------------------------------------------
class NewSWDesignPage : public NewPage
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] libInterface The library interface.
     *      @param [in] parentDlg    The parent dialog.
     */
    NewSWDesignPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    ~NewSWDesignPage();

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
    //! Signaled when a design should be created.
    void createSWDesign(VLNV const& vlnv, QString const& directory);

private:
    // Disable copying.
    NewSWDesignPage(NewSWDesignPage const& rhs);
    NewSWDesignPage& operator=(NewSWDesignPage const& rhs);
    
};

//-----------------------------------------------------------------------------


#endif // NEWSWDESIGNPAGE_H
