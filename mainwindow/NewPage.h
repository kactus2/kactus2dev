//-----------------------------------------------------------------------------
// File: NewPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.8.2013
//
// Description:
// Base class for all New pages.
//-----------------------------------------------------------------------------

#ifndef NEWPAGE_H
#define NEWPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>
#include <common/KactusAttribute.h>
#include <library/LibraryManager/vlnv.h>

#include <QLabel>
#include <QPushButton>

class LibraryInterface;
class VLNVEditor;
class KactusAttributeEditor;
class LibrarySelectorWidget;

//-----------------------------------------------------------------------------
//! NewPage class.
//-----------------------------------------------------------------------------
class NewPage : public PropertyPageView
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] libInterface The library interface.
     *      @param [in] parentDlg    The parent dialog.
     */
    NewPage(LibraryInterface* libInterface, VLNV::IPXactType vlnvType, QString title = QString(), 
        QString description = QString(), QWidget* parentDlg = 0);

    /*!
     *  Destructor.
     */
    ~NewPage();

    /*!
     *  Pre-validates the contents of the page. This is used for enabling/disabling the OK button.
     *
     *      @return True, if the contents are valid and OK button should be enabled.
     *              False, if the contents are invalid and OK button should be disabled.
     *
     *      @remarks Must not show any message boxes.
     */
    virtual bool prevalidate() const;

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    virtual bool validate();

    /*!
     *  Called when the page is to be changed and this page would be hidden.
     *
     *      @return False, if the page change should be rejected. Otherwise true.
     */
    virtual bool onPageChange();
    
public slots:

    /*!
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

protected:

    /*!
     *  Sets the basic layout for the page.
     */
   void setupLayout();
    
	//! The library interface.
    LibraryInterface* libInterface_;

    //! VLNV editor.
    VLNVEditor* vlnvEditor_;

    //! Library selector.
    LibrarySelectorWidget* librarySelector_;

private:
    // Disable copying.
    NewPage(NewPage const& rhs);
    NewPage& operator=(NewPage const& rhs);

    //! Page title.
    QLabel* titleLabel_;
    
    //! Page description.
    QLabel* descLabel_;

};

//-----------------------------------------------------------------------------

#endif // NEWPAGE_H
