//-----------------------------------------------------------------------------
// File: RenodeSettingsWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.06.2023
//
// Description:
// Widget for renode generator settings.
//-----------------------------------------------------------------------------

#include "RenodeSettingsWidget.h"

#include <Plugins/RenodeGenerator/RenodeSettingsModel.h>

#include <QHBoxLayout>

#include <QLabel>
#include <QPushButton>
#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: RenodeSettingsWidget::RenodeSettingsWidget()
//-----------------------------------------------------------------------------
RenodeSettingsWidget::RenodeSettingsWidget(RenodeSettingsModel* settingsModel, QWidget *parent /* = 0 */):
QWidget(parent),
settingsModel_(settingsModel),
templatePathEditor_(new QLineEdit(settingsModel_->getFilePath(), this))
{
    templatePathEditor_->setToolTip(templatePathEditor_->text());

    setupLayout();

    connect(templatePathEditor_, SIGNAL(textChanged(QString const&)), this, SLOT(onPathChanged(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: RenodeSettingsWidget::setupLayout()
//-----------------------------------------------------------------------------
void RenodeSettingsWidget::setupLayout()
{
    auto mainLayout(new QHBoxLayout(this));

	mainLayout->addWidget(new QLabel(tr("Template path:"), this));
    mainLayout->addWidget(templatePathEditor_, 1);

    auto fileBrowserButton(new QPushButton(this));
    fileBrowserButton->setIcon(QIcon(QString(":/icons/common/graphics/opened-folder.png")));

    connect(fileBrowserButton, SIGNAL(clicked()), this, SLOT(onBrowseTemplatePath()), Qt::UniqueConnection);

    mainLayout->addWidget(fileBrowserButton);
}

//-----------------------------------------------------------------------------
// Function: RenodeSettingsWidget::onBrowseTemplatePath()
//-----------------------------------------------------------------------------
void RenodeSettingsWidget::onBrowseTemplatePath()
{
	QString newTemplatePath = QFileDialog::getOpenFileName(
        this, tr("Select peripheral template configuration file"), templatePathEditor_->text(), tr("JSON file (*.json)"));

    if (!newTemplatePath.isEmpty())
    {
        templatePathEditor_->setText(newTemplatePath);
    }
}

//-----------------------------------------------------------------------------
// Function: RenodeSettingsWidget::onPathChanged()
//-----------------------------------------------------------------------------
void RenodeSettingsWidget::onPathChanged(QString const& newPath)
{
    settingsModel_->setFilePath(newPath);
    templatePathEditor_->setToolTip(newPath);
}
