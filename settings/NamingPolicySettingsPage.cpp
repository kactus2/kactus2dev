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
#include <common/widgets/SnippetTextEdit/SnippetTextEdit.h>

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
      valuesList_(new ListEditor(this)),
      formatLabel_(new QLabel(tr("Format:"), this)),
      formatEdit_(new SnippetTextEdit(this))
{
    // Add categories.
    categories_.append(PolicyCategory("Policies/InstanceNames", "Component Instance Names", POLICY_FORMAT));
    categories_.back().magicWords.append("ComponentName");
    categories_.back().magicWords.append("InstanceNumber");

    categories_.append(PolicyCategory("Policies/HWViewNames", "HW View Names", POLICY_ENUMERATION));
    categories_.append(PolicyCategory("Policies/SWViewNames", "SW View Names", POLICY_ENUMERATION));
    categories_.append(PolicyCategory("Policies/SysViewNames", "System View Names", POLICY_ENUMERATION));

    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(categoryLabel_);
    layout->addWidget(categoryCombo_);
    layout->addWidget(valuesLabel_);
    layout->addWidget(valuesList_);
    layout->addWidget(formatLabel_);
    layout->addWidget(formatEdit_);
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
    saveCurrentValues();

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
    saveCurrentValues();

    // Update the widgets based on the category type and content.
    if (categories_.at(index).type == POLICY_ENUMERATION)
    {
        valuesList_->setItems(categories_.at(index).values);

        formatLabel_->setVisible(false);
        formatEdit_->setVisible(false);
        valuesLabel_->setVisible(true);
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

        formatLabel_->setVisible(true);
        formatEdit_->setVisible(true);
        valuesLabel_->setVisible(false);
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
