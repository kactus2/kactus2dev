//-----------------------------------------------------------------------------
// File: NewDesignPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.6.2011
//
// Description:
// New design property page.
//-----------------------------------------------------------------------------

#ifndef NEWDESIGNPAGE_H
#define NEWDESIGNPAGE_H

#include "NewPage.h"
#include <common/KactusAttribute.h>

class KactusAttributeEditor;

//-----------------------------------------------------------------------------
//! NewDesignPage class.
//-----------------------------------------------------------------------------
class NewDesignPage : public NewPage
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] libInterface The library interface.
     *      @param [in] parentDlg    The parent dialog.
     */
    NewDesignPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    ~NewDesignPage();


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

    //! Called when the user changes the product hierarchy attribute.
    void onProductHierarchyChanged();
    

signals:
    //! Signaled when a design should be created.
    void createDesign(KactusAttribute::ProductHierarchy prodHier,
                      KactusAttribute::Firmness firmness,
                      VLNV const& vlnv, QString const& directory);

private:
    // Disable copying.
    NewDesignPage(NewDesignPage const& rhs);
    NewDesignPage& operator=(NewDesignPage const& rhs);
  
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Attribute editor.
    KactusAttributeEditor* attributeEditor_;
};

//-----------------------------------------------------------------------------

#endif // NEWDESIGNPAGE_H
