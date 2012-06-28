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
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: PluginListDialog::PluginListDialog()
//-----------------------------------------------------------------------------
PluginListDialog::PluginListDialog(QWidget* parent)
    : QDialog(parent),
      pluginsList_(this),
      detailBox_(this),
      buttonBox_(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this)
{
    setWindowTitle(tr("Choose Plugin"));
    resize(400, 240);

    // Initialize the widgets.
    detailBox_.setFixedHeight(80);
    detailBox_.setReadOnly(true);

    QPalette pal = detailBox_.palette();
    pal.setColor(QPalette::Base, pal.color(QPalette::Disabled, QPalette::Base));
    detailBox_.setPalette(pal);

    buttonBox_.button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(buttonBox_.button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox_.button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()), Qt::UniqueConnection);
    connect(&pluginsList_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(selectionChanged()), Qt::UniqueConnection);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&pluginsList_, 1);
    layout->addWidget(&detailBox_);
    layout->addWidget(&buttonBox_);
}

//-----------------------------------------------------------------------------
// Function: PluginListDialog::~PluginListDialog()
//-----------------------------------------------------------------------------
PluginListDialog::~PluginListDialog()
{

}

//-----------------------------------------------------------------------------
// Function: PluginListDialog::getSelectedPlugin()
//-----------------------------------------------------------------------------
QObject* PluginListDialog::getSelectedPlugin() const
{
    return pluginsList_.currentItem()->data(Qt::UserRole).value<QObject*>();
}

//-----------------------------------------------------------------------------
// Function: PluginListDialog::selectionChanged()
//-----------------------------------------------------------------------------
void PluginListDialog::selectionChanged()
{
    IPluginInformation* pluginInfo = qobject_cast<IPluginInformation*>(getSelectedPlugin());
    Q_ASSERT(pluginInfo != 0);

    detailBox_.setPlainText(pluginInfo->getDescription());
}

//-----------------------------------------------------------------------------
// Function: PluginListDialog::addPlugin()
//-----------------------------------------------------------------------------
void PluginListDialog::addPlugin(QObject* plugin)
{
    IPluginInformation* pluginInfo = qobject_cast<IPluginInformation*>(plugin);

    if (pluginInfo != 0)
    {
        QListWidgetItem* item = new QListWidgetItem(pluginInfo->getName() + " (" + pluginInfo->getVersion() + ")");
        item->setData(Qt::UserRole, qVariantFromValue(plugin));
        pluginsList_.addItem(item);
        buttonBox_.button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}
