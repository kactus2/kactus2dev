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

#include "SettingsPage.h"
#include "filetypesview.h"

#include <QLineEdit>
#include <QSettings>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
//! FileTypeSettingsPage class.
//-----------------------------------------------------------------------------
class FileTypeSettingsPage : public SettingsPage
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
    virtual ~FileTypeSettingsPage();

    /*!
     *  Applies the changes that were done in the page.
     */
    virtual void apply();

private:
    // Disable copying.
    FileTypeSettingsPage(FileTypeSettingsPage const& rhs);
    FileTypeSettingsPage& operator=(FileTypeSettingsPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

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
