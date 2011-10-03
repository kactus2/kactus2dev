//-----------------------------------------------------------------------------
// File: NewComponentPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.6.2011
//
// Description:
// New component property page.
//-----------------------------------------------------------------------------

#ifndef NEWCOMPONENTPAGE_H
#define NEWCOMPONENTPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>
#include <common/KactusAttribute.h>

#include <QLineEdit>

class LibraryInterface;
class VLNVEditor;
class KactusAttributeEditor;
class VLNV;

//-----------------------------------------------------------------------------
//! NewComponentPage class.
//-----------------------------------------------------------------------------
class NewComponentPage : public PropertyPageView
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] libInterface The library interface.
     *      @param [in] parentDlg    The parent dialog.
     */
    NewComponentPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  Destructor.
     */
    ~NewComponentPage();

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
     *  Asks the user to select a directory.
     */
    void selectDirectory();

    /*!
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

    //! Called when the user changes the product hierarchy attribute.
    void onProductHierarchyChanged();

signals:
    //! Signaled when a component should be created.
    void createComponent(KactusAttribute::ProductHierarchy prodHier,
                         KactusAttribute::Firmness firmness,
                         VLNV const& vlnv, QString const& directory);

private:
    // Disable copying.
    NewComponentPage(NewComponentPage const& rhs);
    NewComponentPage& operator=(NewComponentPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libInterface_;

    //! Attribute editor.
    KactusAttributeEditor* attributeEditor_;

    //! VLNV editor.
    VLNVEditor* vlnvEditor_;

    //! Directory line edit.
    QLineEdit* directoryEdit_;
};

//-----------------------------------------------------------------------------

#endif // NEWCOMPONENTPAGE_H
