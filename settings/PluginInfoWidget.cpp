//-----------------------------------------------------------------------------
// File: PluginInfoWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.1.2014
//
// Description:
// Widget for displaying plugin information.
//-----------------------------------------------------------------------------

#include "PluginInfoWidget.h"

#include <QFormLayout>
#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>

//-----------------------------------------------------------------------------
// Function: PluginInfoWidget::PluginInfoWidget()
//-----------------------------------------------------------------------------
PluginInfoWidget::PluginInfoWidget(IPlugin* plugin, QWidget *parent) : 
QWidget(parent),
plugin_(plugin)
{
    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: PluginInfoWidget::setupLayout()
//-----------------------------------------------------------------------------
void PluginInfoWidget::setupLayout()
{
    auto masterLayout(new QVBoxLayout(this));

    if (plugin_)
    {
        auto topLayout(new QFormLayout());
        topLayout->addRow(tr("Name:"), new QLabel(plugin_->getName(), this));    
        topLayout->addRow(tr("Version:"), new QLabel(plugin_->getVersion(), this));
        topLayout->addRow(tr("Vendor:"), new QLabel(plugin_->getVendor(), this));
        topLayout->addRow(tr("Licensed to:"), new QLabel(plugin_->getLicenseHolder(), this));
        topLayout->addRow(tr("License:"), new QLabel(plugin_->getLicense(), this));

        QLabel* descriptionLabel = new QLabel(plugin_->getDescription(), this);
        descriptionLabel->setWordWrap(true); 
        topLayout->addRow(tr("Description:"), descriptionLabel);

        masterLayout->addLayout(topLayout);

		QWidget* settingsWidget = plugin_->getSettingsWidget();
        if (settingsWidget)
        {
            settingsWidget->setParent(this);

            settingsWidget->setContentsMargins(0, 0, 0, 0);

			auto groupLayout(new QVBoxLayout());
            groupLayout->addWidget(settingsWidget);
            
			auto settingsGroup(new QGroupBox("Settings", this));
            settingsGroup->setLayout(groupLayout);

			groupLayout->setContentsMargins(0, 0, 0, 0);
            settingsGroup->setContentsMargins(0, 0, 0, 0);

            masterLayout->addWidget(settingsGroup, 2);
        }
        else
        {
            masterLayout->addStretch(2);
        }
    }
}
