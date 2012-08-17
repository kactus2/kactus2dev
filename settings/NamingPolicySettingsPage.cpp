//-----------------------------------------------------------------------------
// File: NamingPolicySettingsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.6.2011
//
// Description:
// General settings property page.
//-----------------------------------------------------------------------------

#include "NamingPolicySettingsPage.h"

#include <common/widgets/listManager/listeditor.h>

#include <QVBoxLayout>
#include <QDir>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QCoreApplication>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: NamingPolicySettingsPage()
//-----------------------------------------------------------------------------
NamingPolicySettingsPage::NamingPolicySettingsPage(QSettings& settings)
    : settings_(settings),
      categories_(),
      curCategoryIndex_(-1),
      categoryLabel_(new QLabel(tr("Show naming policy for:"), this)),
      categoryCombo_(new QComboBox(this)),
      valuesLabel_(new QLabel(tr("Values:"), this)),
      valuesList_(new ListEditor(this))
{
    // Add categories.
    categories_.append(PolicyCategory("policies/hwviewname", "HW View Name", POLICY_ENUMERATION));
    categories_.append(PolicyCategory("policies/swviewname", "SW View Name", POLICY_ENUMERATION));
    categories_.append(PolicyCategory("policies/sysviewname", "System View Name", POLICY_ENUMERATION));

    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(categoryLabel_);
    layout->addWidget(categoryCombo_);
    layout->addWidget(valuesLabel_);
    layout->addWidget(valuesList_);
    layout->addStretch(1);

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
// Function: ~NamingPolicySettingsPage()
//-----------------------------------------------------------------------------
NamingPolicySettingsPage::~NamingPolicySettingsPage()
{
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool NamingPolicySettingsPage::validate()
{
    Q_ASSERT(prevalidate());
    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NamingPolicySettingsPage::apply()
{
    if (curCategoryIndex_ != -1)
    {
        categories_[curCategoryIndex_].values = valuesList_->items();
    }

    foreach (PolicyCategory const& category, categories_)
    {
        settings_.setValue(category.key, category.values);
    }
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool NamingPolicySettingsPage::onPageChange()
{
    // Do not change the page if the settings are invalid.
    if (!validate())
    {
        return false;
    }

    // Allow the page to be changed.
    return true;
}

//-----------------------------------------------------------------------------
// Function: NamingPolicySettingsPage::onCategoryChanged()
//-----------------------------------------------------------------------------
void NamingPolicySettingsPage::onCategoryChanged(int index)
{
    // Save the values of the previously selected category.
    if (curCategoryIndex_ != -1)
    {
        categories_[curCategoryIndex_].values = valuesList_->items();
    }

    valuesList_->setItems(categories_.at(index).values);
    curCategoryIndex_ = index;
}
