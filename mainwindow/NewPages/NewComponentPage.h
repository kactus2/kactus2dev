//-----------------------------------------------------------------------------
// File: NewComponentPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 9.6.2011
//
// Description:
// New component property page.
//-----------------------------------------------------------------------------

#ifndef NEWCOMPONENTPAGE_H
#define NEWCOMPONENTPAGE_H

#include "NewPage.h"

#include <IPXACTmodels/common/TagData.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

class LibraryInterface;
class KactusAttributeEditor;
class VLNV;

//-----------------------------------------------------------------------------
//! NewComponentPage class.
//-----------------------------------------------------------------------------
class NewComponentPage : public NewPage
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] libInterface    The library interface.
     *      @param [in] parentDlg       The parent dialog.
     */
    NewComponentPage(LibraryInterface* libInterface, QWidget* parentDlg);

    /*!
     *  The destructor.
     */
    ~NewComponentPage();

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
     *  Called when the user changes the product hierarchy attribute.
     */
    void onProductHierarchyChanged();

signals:

    /*!
     *  Signaled when a component should be created.
     */
    void createComponent(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness,
        QVector<TagData> tags, VLNV const& vlnv, QString const& directory);

private:
    // Disable copying.
    NewComponentPage(NewComponentPage const& rhs);
    NewComponentPage& operator=(NewComponentPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Attribute editor.
    KactusAttributeEditor* attributeEditor_;
};

//-----------------------------------------------------------------------------

#endif // NEWCOMPONENTPAGE_H
