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
#include <PluginSystem/ISourceAnalyzerPlugin.h>

#include <models/generaldeclarations.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::PluginSettingsPage()
//-----------------------------------------------------------------------------
PluginSettingsPage::PluginSettingsPage(QSettings& settings, PluginManager& pluginMgr)
    : settings_(settings),
    pluginMgr_(pluginMgr),
    pluginDirSelector_(QApplication::applicationDirPath(), 
        settings.value("Platform/PluginsPath", QStringList()).toStringList(), this),
    pluginsTree_(this),
    settingsStack_(this),    
    directoriesChanged_(false)
{
    // Initialize the widgets and create the layout.
    pluginDirSelector_.setTitle(tr("Plugins directories"));

    pluginsTree_.setHeaderHidden(true);
    pluginsTree_.setColumnCount(1);
    pluginsTree_.setSelectionBehavior(QAbstractItemView::SelectItems);
    pluginsTree_.setMaximumHeight(140);

    QGroupBox* settingsGroup = new QGroupBox(tr("Plugin settings"), this);

    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);
    settingsLayout->addWidget(&settingsStack_);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&pluginDirSelector_);
    layout->addWidget(new QLabel(tr("Available plugins:"), this));
    layout->addWidget(&pluginsTree_);
    layout->addWidget(settingsGroup, 1);

    refreshPluginsTree();

    connect(&pluginDirSelector_, SIGNAL(contentChanged()),
        this, SLOT(onDirectoriesChanged()), Qt::UniqueConnection);    

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
    if (directoriesChanged_)
    {
        // Save the new plugins directories and update plugin manager.
        settings_.setValue("Platform/PluginsPath", pluginDirSelector_.getDirectories());
        pluginMgr_.setPluginPaths(pluginDirSelector_.getDirectories());
    }

    settings_.beginGroup("PluginSettings");

    // Save settings and active state for each individual plugin.
    for (int i = 0; i < pluginsTree_.topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* categoryRoot = pluginsTree_.topLevelItem(i);

        for (int j = 0; j < categoryRoot->childCount(); ++j)
        {
            QTreeWidgetItem* item = categoryRoot->child(j);

            // Retrieve the plugin pointer from the data.
            IPlugin* plugin = static_cast<IPlugin*>(item->data(0, Qt::UserRole).value<void*>());
            Q_ASSERT(plugin != 0);

            // Retrieve the settings widget index from the data and save custom settings.
            int index = item->data(0, Qt::UserRole + 1).toInt();
            PluginSettingsWidget* settingsWidget = static_cast<PluginSettingsWidget*>(settingsStack_.widget(index));
            Q_ASSERT(settingsWidget != 0);

            // Save the active state.
            settings_.beginGroup(General::removeWhiteSpace(plugin->getName()));
            settings_.setValue("Active", item->checkState(0) == Qt::Checked);
            settingsWidget->saveSettings(settings_);
            settings_.endGroup();
        }
    }

    settings_.endGroup();
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
void PluginSettingsPage::onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* /*prev*/)
{
    int index = 0;

    if (current != 0)
    {
        index = current->data(0, Qt::UserRole + 1).toInt();
    }

    settingsStack_.setCurrentIndex(index);
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::createCategoryItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem* PluginSettingsPage::createCategoryItem(QString const& text, QIcon const& icon)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, text);
    item->setIcon(0, icon);
    item->setData(0, Qt::UserRole + 1, 0);

    // Use bold font for category items.
    QFont rootFont = item->font(0);
    rootFont.setBold(true);
    item->setFont(0, rootFont);

    pluginsTree_.addTopLevelItem(item);
    return item;
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::onDirectoriesChanged()
//-----------------------------------------------------------------------------
void PluginSettingsPage::onDirectoriesChanged()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    directoriesChanged_ = true;
    refreshPluginsTree();

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::refreshPluginsTree()
//-----------------------------------------------------------------------------
void PluginSettingsPage::refreshPluginsTree()
{
    pluginsTree_.clear();
    while (settingsStack_.count() > 0)
    {
        settingsStack_.removeWidget(settingsStack_.widget(0));
    }

    // Create the category items.
    QTreeWidgetItem* generatorsItem = createCategoryItem(tr("Generators"),
        QIcon(":icons/graphics/plugin-generator.png"));
    QTreeWidgetItem* analyzersItem = createCategoryItem(tr("Source Analyzers"),
        QIcon(":icons/graphics/plugin-source_analyzer.png"));

    settingsStack_.addWidget(new PluginSettingsWidget());

    // Enumerate all plugins and add them to the correct root.
    PluginManager pluginManager(pluginDirSelector_.getDirectories());
    settings_.beginGroup("PluginSettings");

    foreach (IPlugin* plugin, pluginManager.getAllPlugins())
    {
        settings_.beginGroup(General::removeWhiteSpace(plugin->getName()));

        // Create the tree item.
        QTreeWidgetItem* pluginItem = new QTreeWidgetItem();
        pluginItem->setText(0, plugin->getName() + " (" + plugin->getVersion() + ")");
        pluginItem->setToolTip(0, plugin->getDescription());

        if (settings_.value("Active", true).toBool())
        {
            pluginItem->setCheckState(0, Qt::Checked);
        }
        else
        {
            pluginItem->setCheckState(0, Qt::Unchecked);
        }

        pluginItem->setData(0, Qt::UserRole, qVariantFromValue(static_cast<void*>(plugin)));

        if (dynamic_cast<IGeneratorPlugin*>(plugin) != 0)
        {
            pluginItem->setIcon(0, dynamic_cast<IGeneratorPlugin*>(plugin)->getIcon());
            generatorsItem->addChild(pluginItem);
        }
        else if (dynamic_cast<ISourceAnalyzerPlugin*>(plugin) != 0)
        {
            analyzersItem->addChild(pluginItem);
        }

        // Retrieve the settings widget and load the current settings.
        PluginSettingsWidget* settingsWidget = plugin->getSettingsWidget();
        settingsWidget->loadSettings(settings_);

        settingsStack_.addWidget(settingsWidget);
        pluginItem->setData(0, Qt::UserRole + 1, settingsStack_.count() - 1);

        settings_.endGroup();
    }

    settings_.endGroup();

    pluginsTree_.expandAll();
}