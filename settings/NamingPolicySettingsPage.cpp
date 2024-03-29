//-----------------------------------------------------------------------------
// File: NamingPolicySettingsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 13.6.2011
//
// Description:
// General settings property page.
//-----------------------------------------------------------------------------

#include "NamingPolicySettingsPage.h"

#include <common/widgets/listManager/listmanager.h>
#include <common/widgets/SnippetTextEdit/SnippetTextEdit.h>

#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: NamingPolicySettingsPage()
//-----------------------------------------------------------------------------
NamingPolicySettingsPage::NamingPolicySettingsPage(QSettings& settings):
SettingsPage(settings),
    categories_(),
    curCategoryIndex_(-1),
    categoryCombo_(new QComboBox(this)),
    valuesList_(new ListManager(tr("Suggested values"), this)),
    formatGroup_(new QGroupBox(tr("Format"), this)),
    formatEdit_(new SnippetTextEdit(this))
{
    // Add categories.
    categories_.append(PolicyCategory("Policies/InstanceNames", "Component Instance Names", POLICY_FORMAT));
    categories_.back().magicWords.append("ComponentName");
    categories_.back().magicWords.append("InstanceNumber");

    categories_.append(PolicyCategory("Policies/HWViewNames", "HW View Names", POLICY_ENUMERATION));
    categories_.append(PolicyCategory("Policies/SWViewNames", "SW View Names", POLICY_ENUMERATION));
    categories_.append(PolicyCategory("Policies/SysViewNames", "System View Names", POLICY_ENUMERATION));

    QVBoxLayout* formatLayout = new QVBoxLayout(formatGroup_);
    formatLayout->addWidget(formatEdit_);

    // Setup the layout.
    QFormLayout* policyLayout = new QFormLayout();
    policyLayout->addRow(tr("Naming policy:"), categoryCombo_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(policyLayout);
    topLayout->addWidget(valuesList_);
    topLayout->addWidget(formatGroup_);
    topLayout->addStretch(1);

    // Setup connections.
    connect(categoryCombo_, SIGNAL(currentIndexChanged(int)), 
        this, SLOT(onCategoryChanged(int)), Qt::UniqueConnection);

    // Read current values from settings and fill in the combo box.
    for (int i = 0; i < categories_.size(); ++i)
    {
        PolicyCategory& category = categories_[i];
        category.values = settings.value(category.key).toStringList();
        categoryCombo_->addItem(category.name);
    }
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NamingPolicySettingsPage::apply()
{
    saveCurrentValues();

    foreach (PolicyCategory const& category, categories_)
    {
        settings().setValue(category.key, category.values);
    }
}

//-----------------------------------------------------------------------------
// Function: NamingPolicySettingsPage::onCategoryChanged()
//-----------------------------------------------------------------------------
void NamingPolicySettingsPage::onCategoryChanged(int index)
{
    // Save the values of the previously selected category.
    saveCurrentValues();

    // Update the widgets based on the category type and content.
    if (categories_.at(index).type == POLICY_ENUMERATION)
    {
        valuesList_->initialize(categories_.at(index).values);

        formatGroup_->setVisible(false);
        valuesList_->setVisible(true);
    }
    else
    {
        formatEdit_->clearMagicWords();
        formatEdit_->addMagicWords(categories_.at(index).magicWords);

        if (categories_.at(index).values.isEmpty())
        {
            formatEdit_->clear();
        }
        else
        {
            formatEdit_->setPlainText(categories_.at(index).values.first());
        }

        formatGroup_->setVisible(true);
        valuesList_->setVisible(false);
    }

    curCategoryIndex_ = index;
}

//-----------------------------------------------------------------------------
// Function: NamingPolicySettingsPage::saveCurrentValues()
//-----------------------------------------------------------------------------
void NamingPolicySettingsPage::saveCurrentValues()
{
    if (curCategoryIndex_ != -1)
    {
        if (categories_.at(curCategoryIndex_).type == POLICY_ENUMERATION)
        {
            categories_[curCategoryIndex_].values = valuesList_->items();
        }
        else
        {
            categories_[curCategoryIndex_].values = QStringList(formatEdit_->toPlainText());
        }
    }
}
