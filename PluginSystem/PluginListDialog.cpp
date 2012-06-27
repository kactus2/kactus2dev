//-----------------------------------------------------------------------------
// File: PluginListDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// Plugin list dialog.
//-----------------------------------------------------------------------------

#include "PluginListDialog.h"

#include "PluginManager.h"
#include "IPluginInformation.h"

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: PluginListDialog::PluginListDialog()
//-----------------------------------------------------------------------------
PluginListDialog::PluginListDialog(PluginManager* pluginMgr, QWidget* parent)
    : QDialog(parent),
      pluginsList_(this),
      detailBox_(this)
{
    setWindowTitle(tr("Plugins Information"));

    // Fill the plugins list with the loaded plugins.
    foreach (QObject* plugin, pluginMgr->getPlugins())
    {
        IPluginInformation* pluginInfo = qobject_cast<IPluginInformation*>(plugin);

        if (pluginInfo != 0)
        {
            pluginsList_.addItem(pluginInfo->getName() + " (" + pluginInfo->getVersion() + ")");
        }
    }

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&pluginsList_);
    layout->addWidget(&detailBox_);
}

//-----------------------------------------------------------------------------
// Function: PluginListDialog::~PluginListDialog()
//-----------------------------------------------------------------------------
PluginListDialog::~PluginListDialog()
{

}
