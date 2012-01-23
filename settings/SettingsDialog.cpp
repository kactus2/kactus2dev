//-----------------------------------------------------------------------------
// File: SettingsDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 10.6.2011
//
// Description:
// Settings dialog.
//-----------------------------------------------------------------------------

#include "SettingsDialog.h"

#include "GeneralSettingsPage.h"
#include "LibrarySettingsPage.h"
#include "CodeEditorSettingsPage.h"

//-----------------------------------------------------------------------------
// Function: SettingsDialog()
//-----------------------------------------------------------------------------
SettingsDialog::SettingsDialog(QWidget* parent) : PropertyPageDialog(QSize(48, 48), APPLY_ALL, parent),
                                                  settings_()
{
    setFixedSize(600, 440);
    setWindowTitle(tr("Settings"));

    addPage(QIcon(":icons/graphics/settings-general.png"), tr("General"),
            new GeneralSettingsPage(settings_));
    addPage(QIcon(":icons/graphics/settings-library.png"), tr("Library"),
            new LibrarySettingsPage(settings_));
    addPage(QIcon(":icons/graphics/settings-code_editor.png"), tr("Code Editor"),
            new CodeEditorSettingsPage(settings_));

    finalizePages();
}

//-----------------------------------------------------------------------------
// Function: ~SettingsDialog()
//-----------------------------------------------------------------------------
SettingsDialog::~SettingsDialog()
{
}

//-----------------------------------------------------------------------------
// Function: accept()
//-----------------------------------------------------------------------------
void SettingsDialog::accept()
{
    PropertyPageDialog::accept();
    settings_.sync();
}
