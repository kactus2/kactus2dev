//-----------------------------------------------------------------------------
// File: GeneralSettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.6.2011
//
// Description:
// General settings property page.
//-----------------------------------------------------------------------------

#ifndef GENERALSETTINGSPAGE_H
#define GENERALSETTINGSPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>

#include <QLineEdit>
#include <QSettings>

//-----------------------------------------------------------------------------
//! GeneralSettingsPage class.
//-----------------------------------------------------------------------------
class GeneralSettingsPage : public PropertyPageView
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in/out] settings The settings store.
     */
    GeneralSettingsPage(QSettings& settings);

    /*!
     *  Destructor.
     */
    ~GeneralSettingsPage();

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
    GeneralSettingsPage(GeneralSettingsPage const& rhs);
    GeneralSettingsPage& operator=(GeneralSettingsPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The settings store.
    QSettings& settings_;

    //! Username line edit.
    QLineEdit* usernameEdit_;
};

//-----------------------------------------------------------------------------

#endif // GENERALSETTINGSPAGE_H
