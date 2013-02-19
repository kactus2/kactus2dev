//-----------------------------------------------------------------------------
// File: SettingsDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
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
     */
    SettingsDialog(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~SettingsDialog();

public slots:
    virtual void accept();

signals:
	//! \brief Emitted when user has changed the library paths and scan should be performed.
	void scanLibrary();

private:
    // Disable copying.
    SettingsDialog(SettingsDialog const& rhs);
    SettingsDialog& operator=(SettingsDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The settings store.
    QSettings settings_;
};

//-----------------------------------------------------------------------------

#endif // SETTINGSDIALOG_H
