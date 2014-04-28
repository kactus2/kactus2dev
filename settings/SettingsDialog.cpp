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
#include "CodeEditorSettingsPage.h"
#include "NamingPolicySettingsPage.h"
#include "PluginSettingsPage.h"
#include "FileTypeSettingsPage.h"
#include "k2envvariablesettingspage.h"

//-----------------------------------------------------------------------------
// Function: SettingsDialog()
//-----------------------------------------------------------------------------
SettingsDialog::SettingsDialog(PluginManager& pluginMgr, QWidget* parent)
    : PropertyPageDialog(QSize(24, 24), 1, VIEW_LIST, APPLY_ALL, parent),
      settings_()
{
    //setFixedSize(600, 440);
    setWindowTitle(tr("Settings"));

    addPage(QIcon(":icons/common/graphics/settings-general.png"), tr("General"),
            new GeneralSettingsPage(settings_));

    addPage(QIcon(":icons/common/graphics/naming-policy.png"), tr("Naming Policies"),
        new NamingPolicySettingsPage(settings_));

	addPage(QIcon(":icons/common/graphics/settings-code_editor.png"), tr("Code Editor"),
            new CodeEditorSettingsPage(settings_));

    addPage(QIcon(":icons/common/graphics/settings-file_types.png"), tr("File Types"),
            new FileTypeSettingsPage(settings_));

    addPage(QIcon(":icons/common/graphics/settings-plugins.png"), tr("Plugins"),
            new PluginSettingsPage(settings_, pluginMgr));

	 addPage(QIcon(":icons/common/graphics/settings-environment_vars.png"), tr("Environment variables"),
		 new K2EnvVariableSettingsPage(settings_, pluginMgr));

    finalizePages();
    resize(sizeHint().width(), minimumHeight());
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
}
