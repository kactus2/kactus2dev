//-----------------------------------------------------------------------------
// File: SettingsDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 10.6.2011
//
// Description:
// Settings dialog.
//-----------------------------------------------------------------------------

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <common/dialogs/propertyPageDialog/PropertyPageDialog.h>

#include <QSettings>

//-----------------------------------------------------------------------------
//! SettingsDialog class.
//-----------------------------------------------------------------------------
class SettingsDialog : public PropertyPageDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent    The parent widget.
     */
    SettingsDialog(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~SettingsDialog() = default;

    // Disable copying.
    SettingsDialog(SettingsDialog const& rhs) = delete;
    SettingsDialog& operator=(SettingsDialog const& rhs) = delete;

private:

    //! The settings store.
    QSettings settings_;
};

//-----------------------------------------------------------------------------

#endif // SETTINGSDIALOG_H
