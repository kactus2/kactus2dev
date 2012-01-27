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

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/KactusAttribute.h>
#include <common/widgets/LibraryPathSelector/librarypathselector.h>

class LibraryInterface;
class KactusAttributeEditor;

//-----------------------------------------------------------------------------
//! NewDesignPage class.
//-----------------------------------------------------------------------------
class NewDesignPage : public PropertyPageView
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

    //! The library interface.
    LibraryInterface* libInterface_;

    //! Attribute editor.
    KactusAttributeEditor* attributeEditor_;

    //! VLNV editor.
    VLNVEditor* vlnvEditor_;

	//! \brief The editor to select the directory to save to. 
	LibraryPathSelector* directoryEdit_;
};

//-----------------------------------------------------------------------------

#endif // NEWDESIGNPAGE_H
