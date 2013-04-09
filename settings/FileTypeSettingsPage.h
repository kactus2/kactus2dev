//-----------------------------------------------------------------------------
// File: FileTypeSettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.2.2012
//
// Description:
// File type settings property page.
//-----------------------------------------------------------------------------

#ifndef FILETYPESETTINGSPAGE_H
#define FILETYPESETTINGSPAGE_H

#include "FileTypesModel.h"

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>
#include "filetypesview.h"

#include <QLineEdit>
#include <QSettings>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
//! FileTypeSettingsPage class.
//-----------------------------------------------------------------------------
class FileTypeSettingsPage : public PropertyPageView
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in/out] settings The settings store.
     */
    FileTypeSettingsPage(QSettings& settings);

    /*!
     *  Destructor.
     */
    ~FileTypeSettingsPage();

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

private:
    // Disable copying.
    FileTypeSettingsPage(FileTypeSettingsPage const& rhs);
    FileTypeSettingsPage& operator=(FileTypeSettingsPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The settings store.
    QSettings& settings_;

    //! The file types data model.
    FileTypesModel model_;

    //! Proxy model for sorting.
    QSortFilterProxyModel proxyModel_;

    //! The table view for file types.
    FileTypesView view_;

    //! Line edit for ignored extensions.
    QLineEdit ignoredExtensionsEdit_;
};

//-----------------------------------------------------------------------------

#endif // FILETYPESETTINGSPAGE_H
