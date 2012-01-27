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

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/LibraryPathSelector/librarypathselector.h>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! NewSWDesignPage class.
//-----------------------------------------------------------------------------
class NewSWDesignPage : public PropertyPageView
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
     *  Pre-validates the contents of the page. This is used for enabling/disabling the OK button.
     *
     *      @return True, if the contents are valid and OK button should be enabled.
     *              False, if the contents are invalid and OK button should be disabled.
     *
     *      @remarks Must not show any message boxes.
     */
    bool prevalidate() const;

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

    /*!
     *  Called when the page is to be changed and this page would be hidden.
     *
     *      @return False, if the page change should be rejected. Otherwise true.
     */
    bool onPageChange();

public slots:

    /*!
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

signals:
    //! Signaled when a design should be created.
    void createSWDesign(VLNV const& vlnv, QString const& directory);

private:
    // Disable copying.
    NewSWDesignPage(NewSWDesignPage const& rhs);
    NewSWDesignPage& operator=(NewSWDesignPage const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libInterface_;

    //! VLNV editor.
    VLNVEditor* vlnvEditor_;

	//! \brief The editor to select the directory to save to. 
	LibraryPathSelector* directoryEdit_;
};

//-----------------------------------------------------------------------------


#endif // NEWSWDESIGNPAGE_H
