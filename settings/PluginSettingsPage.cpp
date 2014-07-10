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

#include <Plugins/PluginSystem/IPlugin.h>
#include <Plugins/PluginSystem/IGeneratorPlugin.h>
#include <Plugins/PluginSystem/ISourceAnalyzerPlugin.h>
#include <Plugins/PluginSystem/PluginListDialog.h>
#include <Plugins/PluginSystem/PluginInfoWidget.h>

#include <IPXACTmodels/XmlUtils.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <Plugins/PluginSystem/NewPluginsDialog.h>

namespace
{   
    //! Enumeration for model roles.
    enum Roles
    {
        PLUGIN_POINTER_ROLE = Qt::UserRole, //<! Pointer to the plugin.
        PLUGIN_STACK_INDEX_ROLE             //<! Index of the plugin in stacks.
    };
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::PluginSettingsPage()
//-----------------------------------------------------------------------------
PluginSettingsPage::PluginSettingsPage(QSettings& settings, PluginManager& pluginMgr)
    : SettingsPage(settings),
    settings_(settings),
    pluginMgr_(pluginMgr),
    localManager_(QStringList()),
    pluginDirSelector_(QApplication::applicationDirPath(), 
        settings.value("Platform/PluginsPath", QStringList("Plugins")).toStringList(), this),
    pluginsTree_(this),
    settingsStack_(this),    
    infoStack_(this),
    directoriesChanged_(false)
{
    // Initialize the widgets and create the layout.
    pluginDirSelector_.setPersistentDirectory("Plugins");
    pluginDirSelector_.setPersistentDirectory("/usr/share/kactus2/plugins");

    pluginsTree_.setHeaderHidden(true);
    pluginsTree_.setColumnCount(1);
    pluginsTree_.setSelectionBehavior(QAbstractItemView::SelectItems);
    pluginsTree_.setMaximumHeight(140);

    setupLayout();

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
    if (directoriesChanged_ || settings_.value("Platform/PluginsPath").toString().isEmpty())
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
            IPlugin* plugin = static_cast<IPlugin*>(item->data(0, PLUGIN_POINTER_ROLE).value<void*>());
            Q_ASSERT(plugin != 0);

            // Retrieve the settings widget index from the data and save custom settings.
            int index = item->data(0, PLUGIN_STACK_INDEX_ROLE).toInt();
            PluginSettingsWidget* settingsWidget = static_cast<PluginSettingsWidget*>(settingsStack_.widget(index));
            Q_ASSERT(settingsWidget != 0);

            // Save the active state.
            settings_.beginGroup(XmlUtils::removeWhiteSpace(plugin->getName()));
            settings_.setValue("Active", item->checkState(0) == Qt::Checked);
            settingsWidget->saveSettings(settings_);
            settings_.endGroup();
        }
    }

    settings_.endGroup();
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::onTreeItemChanged()
//-----------------------------------------------------------------------------
void PluginSettingsPage::onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* /*prev*/)
{
    int index = 0;

    if (current != 0)
    {
        index = current->data(0, PLUGIN_STACK_INDEX_ROLE).toInt();
    }

    Q_ASSERT(index < settingsStack_.count());
    settingsStack_.setCurrentIndex(index);
    infoStack_.setCurrentIndex(index);
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::createCategoryItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem* PluginSettingsPage::createCategoryItem(QString const& text, QIcon const& icon)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, text);
    item->setIcon(0, icon);
    item->setData(0, PLUGIN_STACK_INDEX_ROLE, 0);

    // Use bold font for category items.
    QFont rootFont = item->font(0);
    rootFont.setBold(true);
    item->setFont(0, rootFont);

    pluginsTree_.addTopLevelItem(item);
    return item;
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::createItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem* PluginSettingsPage::createPluginItem(IPlugin* plugin)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, plugin->getName() + " (" + plugin->getVersion() + ")");

    if (settings_.value("Active", true).toBool())
    {
        item->setCheckState(0, Qt::Checked);
    }
    else
    {
        item->setCheckState(0, Qt::Unchecked);
    }

    item->setData(0, PLUGIN_POINTER_ROLE, qVariantFromValue(static_cast<void*>(plugin)));
    item->setData(0, PLUGIN_STACK_INDEX_ROLE, settingsStack_.count());

    // Retrieve the settings widget and load the current settings.
    PluginSettingsWidget* settingsWidget = plugin->getSettingsWidget();
    settingsWidget->loadSettings(settings_);
    settingsStack_.addWidget(settingsWidget);        

    infoStack_.addWidget(new PluginInfoWidget(plugin));

    return item;
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::onDirectoriesChanged()
//-----------------------------------------------------------------------------
void PluginSettingsPage::onDirectoriesChanged()
{
    directoriesChanged_ = true;
    refreshPluginsTree(true);
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::refreshPluginsTree()
//-----------------------------------------------------------------------------
void PluginSettingsPage::refreshPluginsTree(bool displayChanges)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    QList<IPlugin*> newPlugins;
    QList<IPlugin*> oldPlugins = localManager_.getAllPlugins();    
    
    resetStacks();
    pluginsTree_.clear();

    // Create the category items.
    QTreeWidgetItem* generatorsItem = createCategoryItem(tr("Generators"),
        QIcon(":icons/common/graphics/plugin-generator.png"));
    QTreeWidgetItem* analyzersItem = createCategoryItem(tr("Source Analyzers"),
        QIcon(":icons/common/graphics/plugin-source_analyzer.png"));

    // Enumerate all plugins and add them to the correct root.
    localManager_.setPluginPaths(pluginDirSelector_.getDirectories());

    settings_.beginGroup("PluginSettings");
    foreach (IPlugin* plugin, localManager_.getAllPlugins())
    {
        settings_.beginGroup(XmlUtils::removeWhiteSpace(plugin->getName()));

        // Create the tree item.
        QTreeWidgetItem* pluginItem = createPluginItem(plugin);

        // Add item under the right parent.
        if (dynamic_cast<IGeneratorPlugin*>(plugin) != 0)
        {
            pluginItem->setIcon(0, dynamic_cast<IGeneratorPlugin*>(plugin)->getIcon());
            generatorsItem->addChild(pluginItem);
        }
        else if (dynamic_cast<ISourceAnalyzerPlugin*>(plugin) != 0)
        {
            analyzersItem->addChild(pluginItem);
        }

        // Check if the plugin has not been visible before.
        if (displayChanges && !oldPlugins.contains(plugin))
        {    
            newPlugins.append(plugin);          
        }

        settings_.endGroup();
    }

    settings_.endGroup();

    pluginsTree_.expandAll();

    QApplication::restoreOverrideCursor();

    if (displayChanges && newPlugins.size() > 0)
    {
        NewPluginsDialog pluginDialog(this);
        pluginDialog.addPlugins(newPlugins);
        pluginDialog.exec();
    }
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::resetStacks()
//-----------------------------------------------------------------------------
void PluginSettingsPage::resetStacks()
{
    while (settingsStack_.count() > 0)
    {
        settingsStack_.removeWidget(settingsStack_.widget(0));
    }

    settingsStack_.addWidget(new PluginSettingsWidget());

    while (infoStack_.count() > 0)
    {
        infoStack_.removeWidget(infoStack_.widget(0));
    }

    infoStack_.addWidget(new PluginInfoWidget());
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::setupLayout()
//-----------------------------------------------------------------------------
void PluginSettingsPage::setupLayout()
{
    QGroupBox* settingsGroup = new QGroupBox(tr("Plugin settings"), this);

    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);
    settingsLayout->addWidget(&settingsStack_);

    QGroupBox* infoGroup = new QGroupBox(tr("Plugin general information"), this);

    QVBoxLayout* infoLayout = new QVBoxLayout(infoGroup);
    infoLayout->addWidget(&infoStack_);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Plugin directories"), this));
    layout->addWidget(&pluginDirSelector_);
    layout->addWidget(new QLabel(tr("Available plugins"), this));
    layout->addWidget(&pluginsTree_);
    layout->addWidget(infoGroup);
    layout->addWidget(settingsGroup, 1);
}
