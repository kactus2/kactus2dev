//-----------------------------------------------------------------------------
// File: NewDesignPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 9.6.2011
//
// Description:
// New design property page.
//-----------------------------------------------------------------------------

#ifndef NEWDESIGNPAGE_H
#define NEWDESIGNPAGE_H

#include "NewPage.h"

#include <IPXACTmodels/common/TagData.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

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
    virtual ~NewDesignPage() = default;

    // Disable copying.
    NewDesignPage(NewDesignPage const& rhs) = delete;
    NewDesignPage& operator=(NewDesignPage const& rhs) = delete;

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    bool validate() final;

    /*!
     *  Applies the changes that were done in the page.
     */
    void apply() final;

    /*!
     *  Called when the page is to be changed and this page would be hidden.
     *
     *      @return False, if the page change should be rejected. Otherwise true.
     */
    bool onPageChange() final;
    

signals:
    //! Signaled when a design should be created.
    void createDesign(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness,
        QVector<TagData> tags, VLNV const& vlnv, Document::Revision revision, QString const& directory);

protected:

    KactusAttribute::ProductHierarchy getProductHierarchy() const final;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Attribute editor.
    KactusAttributeEditor* attributeEditor_;

};

//-----------------------------------------------------------------------------

#endif // NEWDESIGNPAGE_H
