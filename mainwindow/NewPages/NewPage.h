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

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QLabel>
#include <QComboBox>

class LibraryInterface;
class VLNVEditor;
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
     *    @param [in] libInterface The library interface.
     *    @param [in] parentDlg    The parent dialog.
     */
    NewPage(LibraryInterface* libInterface, VLNV::IPXactType vlnvType, QString title = QString(), 
        QString description = QString(), QWidget* parentDlg = 0);

    /*!
     *  Destructor.
     */
    virtual ~NewPage() = default;

    // Disable copying.
    NewPage(NewPage const& rhs) = delete;
    NewPage& operator=(NewPage const& rhs) = delete;

    /*!
     *  Pre-validates the contents of the page. This is used for enabling/disabling the OK button.
     *
     *    @return True, if the contents are valid and OK button should be enabled.
     *              False, if the contents are invalid and OK button should be disabled.
     *
     *      @remarks Must not show any message boxes.
     */
    virtual bool prevalidate() const;

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *    @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    virtual bool validate();

    /*!
     *  Called when the page is to be changed and this page would be hidden.
     *
     *    @return False, if the page change should be rejected. Otherwise true.
     */
    virtual bool onPageChange();
    
public slots:

    /*!
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

    //! Updates the VLNV library based on selected product hierarchy.
    void onProductHierarchyChanged();

protected:
    
    /*!
     *  Gets the selected product hierarchy level, if selected.
     *
     *    @return The selected product hierarchy.
     */
    virtual KactusAttribute::ProductHierarchy getProductHierarchy() const;

    /*!
     *  Checks if the given VLNV is not found in the library.
     *
     *    @param [in] vlnv   The VLNV to check.
     *
     *    @return True, if the VLNV is not already in the library, otherwise false.
     */
    bool isUnusedVLNV(VLNV const& vlnv) const;

    /*!
     *  Gets the selected path within the library.
     *
     *    @return The selected path.
     */
    QString selectedPath() const;

    /*!
     *  Shows an error message for a VLNV already found in the library.
     *
     *    @param [in] vlnv    The reserved VLNV.     
     */
    void showErrorForReservedVLVN(VLNV const& vlnv);

    /*!
     *  Gets the name of the VLNV type
     *
     *    @param [in] type    The type whose name to get.
     *
     *    @return The name of the VLNV document type.
     */
    QString type2Show(VLNV::IPXactType const& type);

    /*!
     *  Gets the selected document revision.
     *
     *    @return The selected IP-XACT revision.
     */
    Document::Revision selectedRevision() const;

    //! VLNV editor.
    VLNVEditor* vlnvEditor_;

    //! The library interface.
    LibraryInterface* libInterface_;

private:

    /*!
     *  Sets the basic layout for the page.
     */
    void setupLayout();

    //! IP-XACT standard revision selector.
    QComboBox* revisionSelector_;

    //! Library selector.
    LibrarySelectorWidget* librarySelector_;

    //! Page title.
    QLabel* titleLabel_;
    
    //! Page description.
    QLabel* descLabel_;

};

//-----------------------------------------------------------------------------

#endif // NEWPAGE_H
