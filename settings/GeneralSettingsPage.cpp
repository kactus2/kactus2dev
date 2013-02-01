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

#include <QVBoxLayout>
#include <QDir>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QCoreApplication>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: GeneralSettingsPage()
//-----------------------------------------------------------------------------
GeneralSettingsPage::GeneralSettingsPage(QSettings& settings):
settings_(settings),
usernameEdit_(0)
{
    // Create the username line edit and label.
    QLabel* usernameLabel = new QLabel(tr("User name:"), this);

    QString username = settings_.value("General/Username", getenv("USERNAME")).toString();
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
    settings_.setValue("General/Username", usernameEdit_->text());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool GeneralSettingsPage::onPageChange()
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
