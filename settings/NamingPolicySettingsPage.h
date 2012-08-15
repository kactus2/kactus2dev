//-----------------------------------------------------------------------------
// File: NamingPolicySettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.6.2011
//
// Description:
// settings property page for specifying naming policies.
//-----------------------------------------------------------------------------

#ifndef NAMINGPOLICYSETTINGSPAGE_H
#define NAMINGPOLICYSETTINGSPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>

#include <QComboBox>
#include <QLabel>
#include <QSettings>

class EditableListView;

//-----------------------------------------------------------------------------
//! NamingPolicySettingsPage class.
//-----------------------------------------------------------------------------
class NamingPolicySettingsPage : public PropertyPageView
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in/out] settings The settings store.
     */
    NamingPolicySettingsPage(QSettings& settings);

    /*!
     *  Destructor.
     */
    ~NamingPolicySettingsPage();

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
    NamingPolicySettingsPage(NamingPolicySettingsPage const& rhs);
    NamingPolicySettingsPage& operator=(NamingPolicySettingsPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The settings store.
    QSettings& settings_;

    //! Label for element selection.
    QLabel* elementLabel_;

    //! Element selection combo box.
    QComboBox* elementCombo_;

    //! Label for suggested names.
    QLabel* namesLabel_;

    //! Suggested names list edit.
    EditableListView* namesListView_;
};

//-----------------------------------------------------------------------------

#endif // NAMINGPOLICYSETTINGSPAGE_H
