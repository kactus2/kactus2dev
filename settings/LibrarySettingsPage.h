//-----------------------------------------------------------------------------
// File: LibrarySettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.8.2011
//
// Description:
// Library settings page for the settings dialog.
//-----------------------------------------------------------------------------

#ifndef LIBRARYSETTINGSPAGE_H
#define LIBRARYSETTINGSPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>

#include <QListWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QSettings>

//-----------------------------------------------------------------------------
//! LibrarySettingsPage class.
//-----------------------------------------------------------------------------
class LibrarySettingsPage : public PropertyPageView
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in/out] settings The settings store.
     */
    LibrarySettingsPage(QSettings& settings);

    /*!
     *  Destructor.
     */
    ~LibrarySettingsPage();

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
    //! Adds a new directory to the library locations.
    void addLocation();

    //! Removes the currently selected library location.
    void removeLocation();

    //! Called when a location is selected in the locations list.
    void onSelectLocation(QListWidgetItem* cur, QListWidgetItem* prev);

    //! Called when the auto-refresh check box is toggled.
    void onToggleAutoRefresh(bool checked);

private:
    // Disable copying.
    LibrarySettingsPage(LibrarySettingsPage const& rhs);
    LibrarySettingsPage& operator=(LibrarySettingsPage const& rhs);

    /*!
     *  Loads the settings from the settings store.
     */
    void loadSettings();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The settings store.
    QSettings& settings_;

    //! The library location list.
    QListWidget* libLocationsList_;

    //! Add location button.
    QPushButton* addLocationButton_;

    //! Remove location button.
    QPushButton* removeLocationButton_;

    //! Auto-refresh on startup check box.
    QCheckBox* autoRefreshCheckBox_;

    //! Ask before refresh check box.
    QCheckBox* askBeforeRefreshCheckBox_;
};

//-----------------------------------------------------------------------------

#endif // LIBRARYSETTINGSPAGE_H
