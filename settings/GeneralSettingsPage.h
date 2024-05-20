//-----------------------------------------------------------------------------
// File: GeneralSettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 13.6.2011
//
// Description:
// General settings property page.
//-----------------------------------------------------------------------------

#ifndef GENERALSETTINGSPAGE_H
#define GENERALSETTINGSPAGE_H

#include "SettingsPage.h"

#include <QComboBox>
#include <QLineEdit>
#include <QSettings>

class QCheckBox;

//-----------------------------------------------------------------------------
//! GeneralSettingsPage class.
//-----------------------------------------------------------------------------
class GeneralSettingsPage : public SettingsPage
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
    virtual ~GeneralSettingsPage() = default;

    // Disable copying.
    GeneralSettingsPage(GeneralSettingsPage const& rhs) = delete;
    GeneralSettingsPage& operator=(GeneralSettingsPage const& rhs) = delete;

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    virtual bool validate() override final;

    /*!
     *  Applies the changes that were done in the page.
     */
    virtual void apply() override final;

private:

    //! Username line edit.
    QLineEdit* usernameEdit_;

    //! Default revision selector.
    QComboBox* revisionEdit_;

    //! Checkbox for enabling/disabling the editor lock feature.
    QCheckBox* lockEnabledBox_;
};

//-----------------------------------------------------------------------------

#endif // GENERALSETTINGSPAGE_H
