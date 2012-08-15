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
#include "NamingPolicySettingsPage.h"

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
    
	LibrarySettingsPage* libraryPage = new LibrarySettingsPage(settings_);
	connect(libraryPage, SIGNAL(scanLibrary()),
		this, SIGNAL(scanLibrary()), Qt::UniqueConnection);
	addPage(QIcon(":icons/graphics/settings-library.png"), tr("Library"),
            libraryPage);

    addPage(QIcon(":icons/graphics/naming-policy.png"), tr("Naming Policies"),
        new NamingPolicySettingsPage(settings_));

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
