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

#include <common/views/EditableListView/editablelistview.h>

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
      elementLabel_(new QLabel(tr("Element:"), this)),
      elementCombo_(new QComboBox(this)),
      namesLabel_(new QLabel(tr("Name suggestions:"), this)),
      namesListView_(new EditableListView(this))
{
    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(elementLabel_);
    layout->addWidget(elementCombo_);
    layout->addWidget(namesLabel_);
    layout->addWidget(namesListView_);
    layout->addStretch(1);
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

    // TODO:
    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void NamingPolicySettingsPage::apply()
{
    // TODO: Save the settings.

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

    // If they are valid, save them and allow the page to be changed.
    apply();
    return true;
}
