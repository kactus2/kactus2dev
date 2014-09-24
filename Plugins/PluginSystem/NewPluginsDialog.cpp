//-----------------------------------------------------------------------------
// File: NewPluginsDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.1.2014
//
// Description:
// Dialog for displaying new plugins.
//-----------------------------------------------------------------------------

#include "NewPluginsDialog.h"

#include "IGeneratorPlugin.h"
#include "IPlugin.h"
#include "PluginInfoWidget.h"
#include "ISourceAnalyzerPlugin.h"
#include "ImportPlugin/ImportPlugin.h"

#include <QPushButton>
#include <QVBoxLayout>

namespace
{   
    enum Roles
    {
        PLUGIN_POINTER_ROLE = Qt::UserRole, //<! Pointer to the plugin.
        PLUGIN_STACK_INDEX_ROLE             //<! Index of the plugin in detail stack.
    };
}

//-----------------------------------------------------------------------------
// Function: NewPluginsDialog::NewPluginsDialog()
//-----------------------------------------------------------------------------
NewPluginsDialog::NewPluginsDialog(QWidget* parent)
    : QDialog(parent),
      pluginsList_(this),
      detailsStack_(this),
      buttonBox_(QDialogButtonBox::Ok, Qt::Horizontal, this)
{
    setWindowTitle(tr("New plugins"));    

    connect(buttonBox_.button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox_.button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
        this, SLOT(reject()), Qt::UniqueConnection);
    connect(&pluginsList_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(selectionChanged()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: NewPluginsDialog::~NewPluginsDialog()
//-----------------------------------------------------------------------------
NewPluginsDialog::~NewPluginsDialog()
{
}

//-----------------------------------------------------------------------------
// Function: NewPluginsDialog::selectionChanged()
//-----------------------------------------------------------------------------
void NewPluginsDialog::selectionChanged()
{
    IPlugin* plugin = static_cast<IPlugin*>(pluginsList_.currentItem()->data(PLUGIN_POINTER_ROLE).value<void*>());
    Q_ASSERT(plugin != 0);

    int index = pluginsList_.currentItem()->data(PLUGIN_STACK_INDEX_ROLE).value<int>();
    detailsStack_.setCurrentIndex(index);
}

//-----------------------------------------------------------------------------
// Function: NewPluginsDialog::addPlugin()
//-----------------------------------------------------------------------------
void NewPluginsDialog::addPlugin(IPlugin* plugin)
{
    QListWidgetItem* item = new QListWidgetItem(plugin->getName() + " (" + plugin->getVersion() + ")");
    item->setData(PLUGIN_POINTER_ROLE, qVariantFromValue(static_cast<void*>(plugin)));
    item->setData(PLUGIN_STACK_INDEX_ROLE, detailsStack_.count());

    // Set icon for plugin.
    IGeneratorPlugin* generator = dynamic_cast<IGeneratorPlugin*>(plugin);
    if (generator != 0)
    {
        item->setIcon(generator->getIcon());
    }
    else if(dynamic_cast<ISourceAnalyzerPlugin*>(plugin) != 0)
    {
        item->setIcon(QIcon(":icons/common/graphics/plugin-source_analyzer.png"));
    }
    else if(dynamic_cast<ImportPlugin*>(plugin) != 0)
    {
        item->setIcon(QIcon(":icons/common/graphics/import.png"));
    }

    pluginsList_.addItem(item);    
    detailsStack_.addWidget(new PluginInfoWidget(plugin));
}

//-----------------------------------------------------------------------------
// Function: NewPluginsDialog::addPlugin()
//-----------------------------------------------------------------------------
void NewPluginsDialog::addPlugins(QList<IPlugin*> plugins)
{
    foreach(IPlugin* plugin, plugins)
    {
        addPlugin(plugin);
    }
}

//-----------------------------------------------------------------------------
// Function: NewPluginsDialog::setupLayout()
//-----------------------------------------------------------------------------
void NewPluginsDialog::setupLayout()
{
    resize(400, 300);        
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&pluginsList_, 1);
    layout->addWidget(&detailsStack_);
    layout->addWidget(&buttonBox_, 0, Qt::AlignHCenter);    
}

