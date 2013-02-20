//-----------------------------------------------------------------------------
// File: PluginSettingsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.02.2013
//
// Description:
// Settings page for adjusting plugin settings.
//-----------------------------------------------------------------------------

#include "PluginSettingsPage.h"

#include <PluginSystem/IPlugin.h>
#include <PluginSystem/IGeneratorPlugin.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::PluginSettingsPage()
//-----------------------------------------------------------------------------
PluginSettingsPage::PluginSettingsPage(QSettings& settings, PluginManager& pluginMgr)
    : settings_(settings),
      pluginMgr_(pluginMgr),
      pluginsTree_(this),
      settingsStack_(this)
{
    // Initialize the widgets and create the layout.
    pluginsTree_.setHeaderHidden(true);
    pluginsTree_.setColumnCount(1);
    pluginsTree_.setSelectionBehavior(QAbstractItemView::SelectItems);
    pluginsTree_.setMaximumHeight(140);

    QGroupBox* settingsGroup = new QGroupBox(tr("Plugin settings"), this);

    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);
    settingsLayout->addWidget(&settingsStack_);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Available plugins:"), this));
    layout->addWidget(&pluginsTree_);
    layout->addWidget(settingsGroup, 1);

    // Create the root tree item for generators.
    QTreeWidgetItem* generatorRoot = new QTreeWidgetItem();
    generatorRoot->setText(0, tr("Generators"));
    generatorRoot->setIcon(0, QIcon(":icons/graphics/settings-general.png"));
    generatorRoot->setData(0, Qt::UserRole + 1, 0);

    QFont rootFont = generatorRoot->font(0);
    rootFont.setBold(true);
    generatorRoot->setFont(0, rootFont);

    settingsStack_.addWidget(new PluginSettingsWidget());

    // Enumerate all plugins and add them to the correct root.
    foreach (IPlugin* plugin, pluginMgr_.getPlugins())
    {
        // Create the tree item.
        QTreeWidgetItem* pluginItem = new QTreeWidgetItem();
        pluginItem->setText(0, plugin->getName() + " (" + plugin->getVersion() + ")");
        pluginItem->setToolTip(0, plugin->getDescription());
        //pluginItem->setCheckState(0, Qt::Checked);
        pluginItem->setData(0, Qt::UserRole, qVariantFromValue(static_cast<void*>(plugin)));
        
        IGeneratorPlugin* generator = dynamic_cast<IGeneratorPlugin*>(plugin);

        if (generator != 0)
        {
            pluginItem->setIcon(0, generator->getIcon());
            generatorRoot->addChild(pluginItem);
        }

        // Retrieve the settings widget.
        PluginSettingsWidget* settingsWidget = plugin->getSettingsWidget();
        settingsWidget->loadSettings(settings_);

        settingsStack_.addWidget(settingsWidget);
        pluginItem->setData(0, Qt::UserRole + 1, settingsStack_.count() - 1);
    }

    pluginsTree_.addTopLevelItem(generatorRoot);
    pluginsTree_.expandAll();

    connect(&pluginsTree_, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(onTreeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::~PluginSettingsPage()
//-----------------------------------------------------------------------------
PluginSettingsPage::~PluginSettingsPage()
{

}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::validate()
//-----------------------------------------------------------------------------
bool PluginSettingsPage::validate()
{
    for (int i = 0; i < settingsStack_.count(); ++i)
    {
        PluginSettingsWidget* settingsWidget = static_cast<PluginSettingsWidget*>(settingsStack_.widget(i));

        if (!settingsWidget->validate())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::apply()
//-----------------------------------------------------------------------------
void PluginSettingsPage::apply()
{
    for (int i = 0; i < settingsStack_.count(); ++i)
    {
        PluginSettingsWidget* settingsWidget = static_cast<PluginSettingsWidget*>(settingsStack_.widget(i));
        settingsWidget->saveSettings(settings_);
    }
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::onPageChange()
//-----------------------------------------------------------------------------
bool PluginSettingsPage::onPageChange()
{
    // Do not change the page if the settings are invalid.
    if (!validate())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::onTreeItemChanged()
//-----------------------------------------------------------------------------
void PluginSettingsPage::onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* prev)
{
    int index = 0;

    if (current != 0)
    {
        index = current->data(0, Qt::UserRole + 1).toInt();
    }

    settingsStack_.setCurrentIndex(index);
}
