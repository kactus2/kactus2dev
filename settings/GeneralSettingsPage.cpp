//-----------------------------------------------------------------------------
// File: GeneralSettingsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.6.2011
//
// Description:
// General settings property page.
//-----------------------------------------------------------------------------

#include "GeneralSettingsPage.h"

#include <common/utils.h>

#include <QCoreApplication>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: GeneralSettingsPage()
//-----------------------------------------------------------------------------
GeneralSettingsPage::GeneralSettingsPage(QSettings& settings):
SettingsPage(settings),
usernameEdit_(0)
{
    // Create the username line edit and label.
    QLabel* usernameLabel = new QLabel(tr("User name:"), this);

    QString username = settings.value("General/Username", Utils::getCurrentUser()).toString();
    usernameEdit_ = new QLineEdit(username, this);

    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(usernameLabel);
    layout->addWidget(usernameEdit_);
    layout->addStretch(1);
}

//-----------------------------------------------------------------------------
// Function: ~GeneralSettingsPage()
//-----------------------------------------------------------------------------
GeneralSettingsPage::~GeneralSettingsPage()
{

}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool GeneralSettingsPage::validate()
{
    Q_ASSERT(prevalidate());

    // Check for a valid username.
    if (usernameEdit_->text().isEmpty())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("The user name must be set."), QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void GeneralSettingsPage::apply()
{
    // Save the settings.
    settings().setValue("General/Username", usernameEdit_->text());
}
