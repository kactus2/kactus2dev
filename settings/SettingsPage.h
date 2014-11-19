//-----------------------------------------------------------------------------
// File: SettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.07.2014
//
// Description:
// Base class for all settings pages.
//-----------------------------------------------------------------------------

#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>

#include <QSettings>

//-----------------------------------------------------------------------------
//! SettingsPage class.
//-----------------------------------------------------------------------------
class SettingsPage : public PropertyPageView
{
    Q_OBJECT
public:
    /*!
     *  Constructor.
     *
     *      @param [in/out] settings The settings store.
     */
    SettingsPage(QSettings& settings);

    /*!
     *  Destructor.
     */
    virtual ~SettingsPage();

    /*!
     *  Called when the page is to be changed and this page would be hidden.
     *
     *      @return False, if the page change should be rejected. Otherwise true.
     */
    virtual bool onPageChange();

protected:

    /*!
     *  Returns the settings to the subclasses.
     *
     */
    QSettings& settings() const;

private:
    // Disable copying.
    SettingsPage(SettingsPage const& rhs);
    SettingsPage& operator=(SettingsPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The settings store.
    QSettings& settings_;

};

//-----------------------------------------------------------------------------

#endif // SETTINGSPAGE_H
