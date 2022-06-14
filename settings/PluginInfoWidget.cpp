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
    QFormLayout* topLayout = new QFormLayout(this);
    
    if (plugin_)
    {
        topLayout->addRow(tr("Name:"), new QLabel(plugin_->getName(), this));    
        topLayout->addRow(tr("Version:"), new QLabel(plugin_->getVersion(), this));
        topLayout->addRow(tr("Vendor:"), new QLabel(plugin_->getVendor(), this));
        topLayout->addRow(tr("Licensed to:"), new QLabel(plugin_->getLicenseHolder(), this));
        topLayout->addRow(tr("License:"), new QLabel(plugin_->getLicense(), this));
     
        QLabel* descriptionLabel = new QLabel(plugin_->getDescription(), this);
        descriptionLabel->setWordWrap(true); 
        topLayout->addRow(tr("Description:"), descriptionLabel);
    }
}

